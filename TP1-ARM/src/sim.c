#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras

void process_instruction()
{

    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */

    // Fetch the instruction from memory
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    // Print the instruction in hexadecimal format
    printf("Executing instruction: 0x%08X\n", instruction);

    uint32_t opcode = (instruction >> 21) & 0x7FF;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    // Decodificar y ejecutar la instrucción
    // Los valores de cada case me los tiro el chat. 
    // Se supone que son los opcodes de cada instrucción y los valores se pueden encontrar en TP1-ARM/ref/DDI0487B_a_armv8_arm.pdf
    switch (opcode) {
        case 0x6A2:  // HLT (Halt)
            printf("HLT: Halting simulation.\n");
            RUN_BIT = 0;
            break;

        case 0x458:  // ADD
            printf("ADD instruction\n");
            // Implementación de ADD aquí
            break;
        
        case 0x659:  // SUB
            printf("SUB instruction\n");
            // Implementación de SUB aquí
            break;
        
        case 0x758:  // ADDS
            printf("ADDS instruction\n");
            // Implementación de ADDS aquí
            break;
        
        case 0x758:  // SUBS
            printf("SUBS instruction\n");
            // Implementación de SUBS aquí
            break;
        
        case 0x550:  // MUL
            printf("MUL instruction\n");
            // Implementación de MUL aquí
            break;
        
        case 0x450:  // ANDS
            printf("ANDS instruction\n");
            // Implementación de ANDS aquí
            break;
        
        case 0x650:  // ORR
            printf("ORR instruction\n");
            // Implementación de ORR aquí
            break;
        
        case 0x750:  // EOR
            printf("EOR instruction\n");
            // Implementación de EOR aquí
            break;
        
        case 0x6B0:  // CMP
            printf("CMP instruction\n");
            // Implementación de CMP aquí
            break;
        
        case 0x0A0:  // B (Branch)
            printf("B instruction\n");
            // Implementación de B aquí
            break;
        
        case 0x6F0:  // BR (Branch Register)
            printf("BR instruction\n");
            // Implementación de BR aquí
            break;
        
        case 0x5A0:  // CBZ (Compare and Branch on Zero)
            printf("CBZ instruction\n");
            // Implementación de CBZ aquí
            break;
        
        case 0x5A8:  // CBNZ (Compare and Branch on Non-Zero)
            printf("CBNZ instruction\n");
            // Implementación de CBNZ aquí
            break;
        
        case 0x7C2:  // STUR
            printf("STUR instruction\n");
            // Implementación de STUR aquí
            break;
        
        case 0x7C0:  // STURB
            printf("STURB instruction\n");
            // Implementación de STURB aquí
            break;
        
        case 0x7C1:  // STURH
            printf("STURH instruction\n");
            // Implementación de STURH aquí
            break;
        
        case 0x5C2:  // LDUR
            printf("LDUR instruction\n");
            // Implementación de LDUR aquí
            break;
        
        case 0x5C0:  // LDURB
            printf("LDURB instruction\n");
            // Implementación de LDURB aquí
            break;
        
        case 0x5C1:  // LDURH
            printf("LDURH instruction\n");
            // Implementación de LDURH aquí
            break;
        
        case 0x69A:  // LSL
            printf("LSL instruction\n");
            // Implementación de LSL aquí
            break;
        
        case 0x69B:  // LSR
            printf("LSR instruction\n");
            // Implementación de LSR aquí
            break;
        
        case 0x528:  // MOVZ
            printf("MOVZ instruction\n");
            // Implementación de MOVZ aquí
            break;
        
        default:
            printf("Unknown instruction with opcode: 0x%X\n", opcode);
            break;
    }

}

