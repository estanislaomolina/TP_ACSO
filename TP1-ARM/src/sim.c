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
        
        case 0x588: // adds
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + CURRENT_STATE.REGS[(instruction >> 16) & 0x1F];
            break;

        case 0x558: // adds
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + (instruction & 0x3FF);
            break;

        case 0x750: // ands
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] & CURRENT_STATE.REGS[(instruction >> 16) & 0x1F];
            break;

        case 0x758: //subs
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] - CURRENT_STATE.REGS[(instruction >> 16) & 0x1F];
            break;

        
        default:
            printf("Unknown instruction with opcode: 0x%X\n", opcode);
            break;

        
    }

}

