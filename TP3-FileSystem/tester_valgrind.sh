#!/usr/bin/env bash

set -e

# -------------------------------------------------------------------
# TP3 Unix V6: Script de pruebas con Valgrind
# Compila, ejecuta y verifica que no haya fugas ni errores de memoria
# -------------------------------------------------------------------

echo
echo "=== Limpiando y compilando ==="
make clean && make

echo
echo "=== Ejecutando tests con Valgrind del TP3 Unix V6 ==="
TESTDISK_DIR="./samples/testdisks"
IMAGES=("basicDiskImage" "depthFileDiskImage" "dirFnameSizeDiskImage")
ALL_PASSED=true

for IMG in "${IMAGES[@]}"; do
    echo
    echo "Probando imagen con Valgrind: $IMG"

    VALGRIND_LOG="valgrind_${IMG}.log"

    # Ejecutar valgrind y guardar el log completo
    valgrind --leak-check=full --error-exitcode=1 \
        ./diskimageaccess -ip "${TESTDISK_DIR}/${IMG}" \
        > /dev/null 2> "$VALGRIND_LOG"

    # Verificar que no haya fugas ni errores
    NO_LEAKS=$(grep -F "All heap blocks were freed -- no leaks are possible" "$VALGRIND_LOG")
    NO_ERRORS=$(grep -F "ERROR SUMMARY: 0 errors from 0 contexts" "$VALGRIND_LOG")

    if [[ -n "$NO_LEAKS" && -n "$NO_ERRORS" ]]; then
        echo "  ✔️  PASÓ (sin fugas ni errores de memoria)"
    else
        echo "  ❌ FALLÓ: Valgrind detectó problemas en $IMG"

        if [[ -z "$NO_LEAKS" ]]; then
            echo "     ⚠️  Fugas de memoria detectadas"
        fi
        if [[ -z "$NO_ERRORS" ]]; then
            echo "     ⚠️  Se detectaron errores de acceso a memoria"
        fi

        echo "     👉 Fragmento del log (primeras líneas relevantes):"
        grep -A 5 -E "ERROR SUMMARY|leak-check summary|definitely lost" "$VALGRIND_LOG" | head -n 10
        ALL_PASSED=false
    fi
done

echo
if $ALL_PASSED; then
    echo "=== ✅ TODOS LOS TESTS DE VALGRIND PASARON ==="
else
    echo "=== ❌ ALGUNOS TESTS DE VALGRIND FALLARON ==="
    echo "    Revisá los archivos valgrind_*.log para más detalles."
fi

echo
echo "Limpiando binarios..."
make clean
