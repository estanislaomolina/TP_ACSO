#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras
#include "./instructions/sim_instructions.h"
#include "sim.h"
#include "utils.h"

const instruction_t instructions_list[] = {
    
    {hlt, 0b11010100, 8},
    {adds_imm, 0b10110001, 8},
    {adds_ext, 0b10101011, 8},
    {subs_imm, 0b11110001, 8},
    {subs_ext, 0b11101011, 8},
    {ands, 0b11101010, 8},
    {eors, 0b11001010, 8},
    {orr, 0b10101010, 8},
    {bcond, 0b01010100, 8},
    {stur, 0b1111100000, 10},
    {sturb, 0b0011100000 , 10},
    {sturh, 0b0111100000, 10},
    {ldur, 0b1111100001, 10},
    {ldurb, 0b00111000, 8},
    {lsl, 0b11010011, 8},
    {lsr, 0b11010011, 8},
    {mov, 0b11010010, 8},
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

    // Decodificar y ejecutar la instrucción
    // Los valores de cada case me los tiro el chat. 
    // Se supone que son los opcodes de cada instrucción y los valores se pueden encontrar en TP1-ARM/ref/DDI0487B_a_armv8_arm.pdf
    for (int i = 0; i < sizeof(instructions_list) / sizeof(instruction_t); i++) {
        uint8_t size = instructions_list[i].size;
        uint32_t instr_opcode = instructions_list[i].opcode;
        uint32_t opcode = extract_bits(instruction, 32 - size, 32 - size + size - 1); // Adjusted bit range
        if (opcode == instr_opcode) {
            // Llamar a la función correspondiente
            instructions_list[i].instruction(instruction);

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }
    }
    printf("Unknown instruction opcode: ");
    print_binary(instruction); // Print unknown instruction for debugging
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

