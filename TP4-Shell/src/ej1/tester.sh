#!/bin/bash
set -u

# Configuración
BIN=./ring
VALGRIND_AVAILABLE=$(command -v valgrind || echo "")
VALGRIND_OUT=$(mktemp)
STDERR_TMP=$(mktemp)
VERBOSE=false
[[ "${1:-}" == "--verbose" ]] && VERBOSE=true

TOTAL=0
PASSED=0
FAILED=0
MEM_CLEAN=0
MEM_FAIL=0

# Paleta de colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[1;34m'
NC='\033[0m'

# Compilación
echo -e "${BLUE}Compilando...${NC}"
make -s
[[ ! -f "$BIN" ]] && echo -e "${RED}Error: binario 'ring' no generado${NC}" && exit 1

# Funciones de test
run_test() {
    local n=$1 c=$2 s=$3 expected=$4 desc="$5"
    ((TOTAL++))
    echo -e "${YELLOW}Test $TOTAL: $desc${NC}"
    echo -e "  ${BLUE}$BIN $n $c $s${NC}"

    if output=$("$BIN" "$n" "$c" "$s" 2> "$STDERR_TMP"); then
        if echo "$output" | grep -q "Valor final.*: $expected"; then
            echo -e "  ${GREEN}✔ Funcionalidad OK${NC}"
            ((PASSED++))
        else
            echo -e "  ${RED}✘ Funcionalidad FALLÓ${NC}"
            echo -e "  ${BLUE}Obtenido: $(echo "$output" | grep 'Valor final' || echo N/A)${NC}"
            ((FAILED++))
        fi
    else
        echo -e "  ${RED}✘ Error en ejecución${NC}"
        ((FAILED++))
    fi

    if [ -s "$STDERR_TMP" ]; then
        echo -e "  ${YELLOW}⚠️  stderr detectado, revisar salida manualmente:${NC}"
        head -3 "$STDERR_TMP" | sed 's/^/     /'
    fi

    if [ "$n" -ge 500 ]; then
        echo -e "  ${YELLOW}Valgrind omitido por tamaño${NC}"
    elif [[ -n "$VALGRIND_AVAILABLE" ]]; then
        valgrind --leak-check=full --error-exitcode=42 "$BIN" "$n" "$c" "$s" >/dev/null 2>> "$VALGRIND_OUT"
        if [ $? -eq 0 ]; then
            echo -e "  ${GREEN}✔ Memoria limpia${NC}"
            ((MEM_CLEAN++))
        else
            echo -e "  ${RED}✘ Leak detectado${NC}"
            ((MEM_FAIL++))
            $VERBOSE && cat "$VALGRIND_OUT"
        fi
    else
        echo -e "  ${YELLOW}Valgrind no disponible${NC}"
    fi

    echo ""
}

run_invalid() {
    local cmd="$1" desc="$2"
    ((TOTAL++))
    echo -e "${YELLOW}Test $TOTAL: $desc${NC}"
    echo -e "  ${BLUE}$cmd${NC}"

    eval "$cmd" >/dev/null 2> "$STDERR_TMP"
    if [ $? -eq 0 ]; then
        echo -e "  ${RED}✘ Debería fallar, pero pasó${NC}"
        ((FAILED++))
    else
        echo -e "  ${GREEN}✔ Falló como se esperaba${NC}"
        ((PASSED++))
    fi

    if [ -s "$STDERR_TMP" ]; then
        echo -e "  ${YELLOW}⚠️  stderr detectado, revisar salida manualmente:${NC}"
        head -3 "$STDERR_TMP" | sed 's/^/     /'
    fi
    echo ""
}

# Casos de prueba
echo -e "${BLUE}● Básicos:${NC}"
run_test 3   0    0    3   "3 procesos, valor 0, start 0"
run_test 5  10    2   15   "5 procesos, valor 10, start 2"
run_test 4  -5    3   -1   "4 procesos, valor -5, start 3"
run_test 6 1000   5 1006   "6 procesos, valor 1000, start 5"

echo -e "${BLUE}● Inválidos:${NC}"
run_invalid "$BIN"                         "Sin argumentos"
run_invalid "$BIN 2 0"                     "Solo dos argumentos"
run_invalid "$BIN 3 0 3"                   "Start fuera de rango"
run_invalid "$BIN 4 0 -1"                  "Start negativo"
run_invalid "$BIN a b c"                  "Argumentos no numéricos"
run_invalid "$BIN 3"                       "Un único argumento"

echo -e "${BLUE}● Límites válidos:${NC}"
run_test 3 -100000 2  -99997                "Valor muy negativo"
run_test 3 2147483640 1 2147483643          "Cercano a INT_MAX"
run_test 500 0 499 500                      "Anillo de 500 procesos"
run_test 3 0 2 3                            "Start en último proceso"
run_test 5 42 4 47                          "Start en el último"
run_test 3 1 2 4                            "Start = n - 1"

echo -e "${BLUE}● Overflow int:${NC}"
run_test 10 2147483637 0 2147483647         "INT_MAX exacto"
run_invalid "$BIN 10 2147483638 0"          "INT_MAX + 1"
run_test 10 -2147483638 0 -2147483628       "INT_MIN exacto"

echo -e "${BLUE}● Timeout / Deadlock:${NC}"
((TOTAL++))
echo -e "${YELLOW}Test $TOTAL: Detección de deadlock${NC}"
if timeout 2s $BIN 3 0 0 >/dev/null 2> "$STDERR_TMP"; then
    echo -e "  ${GREEN}✔ Terminó correctamente${NC}"
    ((PASSED++))
else
    echo -e "  ${RED}✘ Timeout o error${NC}"
    ((FAILED++))
fi
if [ -s "$STDERR_TMP" ]; then
    echo -e "  ${YELLOW}⚠️  stderr detectado, revisar salida manualmente:${NC}"
    head -3 "$STDERR_TMP" | sed 's/^/     /'
fi
echo ""

echo -e "${BLUE}● Prueba con tester.c:${NC}"
((TOTAL++))
gcc -Wall -Wextra -std=c11 -o tester tester.c
if [[ ! -f ./tester ]]; then
    echo -e "  ${RED}✘ No se compiló tester.c${NC}"
    ((FAILED++))
else
    if [[ -n "$VALGRIND_AVAILABLE" ]]; then
        valgrind --leak-check=full --error-exitcode=42 ./tester > /dev/null 2> "$VALGRIND_OUT"
        if [ $? -eq 0 ]; then
            echo -e "  ${GREEN}✔ tester.c sin leaks${NC}"
            ((PASSED++))
            ((MEM_CLEAN++))
        else
            echo -e "  ${RED}✘ tester.c falló o tiene leaks${NC}"
            ((FAILED++))
            ((MEM_FAIL++))
            $VERBOSE && cat "$VALGRIND_OUT"
        fi
    elif ./tester; then
        echo -e "  ${GREEN}✔ tester.c ejecutó sin Valgrind${NC}"
        ((PASSED++))
    else
        echo -e "  ${RED}✘ tester.c falló${NC}"
        ((FAILED++))
    fi
fi
echo ""

echo -e "${BLUE}● Stress test:${NC}"
run_test 1000 0 999 1000 "Anillo de 1000 procesos"

# Resumen final
echo -e "${BLUE}--------------------------------------------${NC}"
echo -e "         ${YELLOW}RESUMEN DE RESULTADOS${NC}"
echo -e "  Total:         $TOTAL"
echo -e "  ${GREEN}✔ OK:         $PASSED${NC}"
echo -e "  ${RED}✘ Fallidos:   $FAILED${NC}"
echo -e "  ${GREEN}✔ Mem OK:     $MEM_CLEAN${NC}"
echo -e "  ${RED}✘ Mem Leaks:  $MEM_FAIL${NC}"
echo -e "${BLUE}--------------------------------------------${NC}"

# Limpieza
make clean >/dev/null
rm -f "$VALGRIND_OUT" "$STDERR_TMP" tester test.txt
exit $((FAILED > 0 ? 1 : 0))
