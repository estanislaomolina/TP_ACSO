.text
    mov  X1, 0xFFFF0000  // Carga un valor en X1
    lsr  X2, X1, 8       // Desplazamiento lógico a la derecha de X1 en 8 bits, resultado en X2

    mov  X3, 0x1000      // Dirección base en X3
    lsl  X3, X3, 16      // Ajuste de dirección (X3 = 0x10000000)

    mov  X4, 0xABCD      // Valor de 16 bits para almacenar
    sturh W4, [X3, 0x2]  // Guarda el valor de 16 bits en la dirección X3 + 2

    ldurh W5, [X3, 0x2]  // Carga de nuevo el valor almacenado en W5

    mov  X6, 5           // Carga un valor en X6
    cbnz X6, loop_start  // Si X6 no es cero, salta a loop_start

loop_start:
    subs  X6, X6, 1       // Resta 1 a X6
    cbnz X6, loop_start  // Si X6 no es cero, sigue en el bucle

    mov  X7, 6           // Carga valores para la multiplicación
    mov  X8, 7
    mul  X9, X7, X8      // Multiplica X7 * X8 y guarda el resultado en X9

    hlt  0               // Finaliza el programa

    