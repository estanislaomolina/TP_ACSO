#!/usr/bin/env bash

set -e

# -------------------------------------------------------------------
# TP3 Unix V6: Script de pruebas
# Limpia, compila y ejecuta los tests sobre las imágenes de disco,
# y verifica que no haya salida extra inesperada a la consola.
# -------------------------------------------------------------------

echo
echo "=== Limpiando y compilando ==="
make clean && make

echo
echo "=== Ejecutando tests del TP3 Unix V6 ==="
TESTDISK_DIR="./samples/testdisks"
IMAGES=("basicDiskImage" "depthFileDiskImage" "dirFnameSizeDiskImage")
ALL_PASSED=true

for IMG in "${IMAGES[@]}"; do
    echo
    echo "Probando imagen: $IMG"

    OUTPUT_FILE="output_${IMG}.txt"
    STDERR_FILE="stderr_${IMG}.log"
    GOLD_FILE="${TESTDISK_DIR}/${IMG}.gold"

    # Ejecutar el programa, capturando stdout y stderr por separado
    ./diskimageaccess -ip "${TESTDISK_DIR}/${IMG}" > "$OUTPUT_FILE" 2> "$STDERR_FILE"

    TEST_OK=true

    # Comparar con archivo .gold
    if diff -q "$OUTPUT_FILE" "$GOLD_FILE" >/dev/null; then
        echo "  ✔️ Output coincide con el esperado."
    else
        echo "  ❌ Test FALLÓ para $IMG - diferencia con .gold"
        echo "     Diferencias (primeras 5 líneas):"
        diff "$OUTPUT_FILE" "$GOLD_FILE" | head -n 5
        TEST_OK=false
    fi

    # Verificar que no haya salida inesperada por stderr
    if [[ -s "$STDERR_FILE" ]]; then
        echo "  ⚠️  ADVERTENCIA: tus tests mostraron dump de consola (stderr no vacío)"
        echo "     Ejemplo de stderr (primeras 3 líneas):"
        head -n 3 "$STDERR_FILE"
        TEST_OK=false
    fi

    if ! $TEST_OK; then
        ALL_PASSED=false
    fi
done

echo
if $ALL_PASSED; then
    echo "=== ✅ TODOS LOS TESTS PASARON CORRECTAMENTE ==="
else
    echo "=== ❌ ALGUNOS TESTS FALLARON O MOSTRARON MENSAJES INESPERADOS ==="
fi

echo
echo "Limpiando archivos temporales..."
rm -f output_*.txt stderr_*.log
make clean
