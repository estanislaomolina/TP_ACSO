#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras
#include "./instructions/sim_instructions.h"
#include "sim.h"
#include "utils.h"

const instruction_t instructions_list[] = {
    
    {hlt, 0b11010100, 8}, //FUNCAN BIEN
    {adds_imm, 0b10110001, 8}, //FUNCAN BIEN
    {adds_ext, 0b10101011, 8}, //FUNCAN BIEN
    {subs_imm, 0b11110001, 8}, //FUNCAN BIEN
    {cmp, 0b11101011, 8}, // CMP (compare registers)
    {subs_ext, 0b11101011, 8}, // SUBS (extended register)
    {ands, 0b11101010, 8}, //FUNCAN BIEN
    {eors, 0b11001010, 8}, //FUNCAN BIEN
    {orr, 0b10101010, 8}, //FUNCAN BIEN
    {b, 0b00010111, 8},  // FUNCA BIEN
    {br, 0b11010110, 8}, //FUNCAN BIEN
    {bcond, 0b01010100, 8},  // REVISAR
    {stur, 0b1111100000, 10}, //FUNCAN BIEN
    {sturb, 0b0011100000 , 10}, //FUNCAN BIEN
    {sturh, 0b0111100000, 10}, //FUNCAN BIEN
    {ldur, 0b1111100001, 10}, //FUNCAN BIEN
    {ldurb, 0b00111000, 8}, //FUNCAN BIEN
    {ldurh, 0b0111100001, 10}, //FUNCAN BIEN
    {shifts, 0b11010011, 8}, //FUNCAN BIEN
    {mov, 0b11010010, 8}, //FUNCAN BIEN
    {cbz, 0b10110100, 8}, // REVISAR
    {cbnz, 0b10110101, 8}, // FUNCA BIEN
    {mul, 0b10011011, 8}, // REVISAR
    {add_imm, 0b10010001, 8},
    {add_ext, 0b10101011, 8}
};


void process_instruction(){

    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */

    // Fetch the instruction from memory
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    // Print the instruction in binary format
    printf("Executing instruction: ");
    print_binary(instruction);
    
    for (int i = 0; i < sizeof(instructions_list) / sizeof(instruction_t); i++) {
        uint8_t size = instructions_list[i].size;
        uint32_t instr_opcode = instructions_list[i].opcode;
        uint32_t opcode = extract_bits(instruction, 32 - size, 32 - size + size - 1);
        
        if (opcode == instr_opcode) {
            // Special case for distinguishing between CMP and SUBS_EXT
            if (opcode == 0b11101011) {
                uint8_t rd = instruction & 0x1F;  // Extract destination register
                if (rd == 31 && instructions_list[i].instruction == cmp) {
                    // It's a CMP instruction (Rd = XZR = 31)
                    instructions_list[i].instruction(instruction);
                    NEXT_STATE.PC += 4;
                    return;
                } else if (rd != 31 && instructions_list[i].instruction == subs_ext) {
                    // It's a SUBS_EXT instruction
                    instructions_list[i].instruction(instruction);
                    NEXT_STATE.PC += 4;
                    return;
                }
                // If neither condition is met, continue to next instruction
                continue;
            }
            
            // For all other instructions, proceed normally
            instructions_list[i].instruction(instruction);
            NEXT_STATE.PC += 4;
            return;
        }
    }
    printf("Unknown instruction opcode: ");
    print_binary(instruction); // Print unknown instruction for debugging
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

// hago el commit y me tira que no hay cambios
