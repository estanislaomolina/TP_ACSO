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

        case 0x2A0: // beq
            if (CURRENT_STATE.FLAG_Z == 1) {
                NEXT_STATE.PC = CURRENT_STATE.PC + (instruction & 0x3FFFFFF) * 4;
            }
            break;

        case 0x650: // eors
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] ^ CURRENT_STATE.REGS[(instruction >> 16) & 0x1F];
            break;

        case 0x694: // movz (creo que mov tambien)
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = (instruction >> 5) & 0xFFFF;
            break;

        case 0x69B: // lsl
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] << (instruction & 0x3F);
            break;

        case 0x7C0: // stur
            mem_write_32(CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + (instruction & 0xFFF), CURRENT_STATE.REGS[(instruction >> 0) & 0x1F]);
            break;
        
        case 0x1C0: // sturb
            // Adaptación para escribir 8 bits usando mem_write_32
            {
                uint64_t address = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + (instruction & 0xFFF);
                uint32_t value = mem_read_32(address);
                value = (value & 0xFFFFFF00) | (CURRENT_STATE.REGS[(instruction >> 0) & 0x1F] & 0xFF);
                mem_write_32(address, value);
            }
            break;

        case 0x7C2: // ldur
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = mem_read_32(CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + (instruction & 0xFFF));
            break;

        case 0x1C2: // ldurb
            // Adaptación para leer 8 bits usando mem_read_32
            {
                uint64_t address = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] + (instruction & 0xFFF);
                uint32_t value = mem_read_32(address);
                NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = value & 0xFF;
            }
            break;
        
        case 0x788: // subs
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] - (instruction & 0xFFF);
            break;

        default:
            printf("Unknown instruction with opcode: 0x%X\n", opcode);
            break;

        
    }

}

