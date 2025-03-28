#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras
#include "./instructions/sim_instructions.h"
#include "sim.h"
#include "utils.h"

const instruction_t instructions_list[] = {
    
    {hlt, 0b11010100},
    {adds_imm, 0b10110001},
    {adds_ext, 0b10101011},
    {subs_imm, 0b11110001},
    {subs_ext, 0b11101011},
    {ands, 0b11101010},
    {eors, 0b11001010},
    {orr, 0b10101010},
    {bcond, 0b01010100},
    {stur, 0b11111000},
    {sturb, 0b00111000},
    {ldur, 0b11111000},
    {ldurb, 0b00111000},
    {lsl, 0b11010011},
    {mov, 0b11010010}
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

    uint32_t opcode = (instruction >> 24) & 0b11111111; // 8 bit opcode
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    uint32_t opcode12 = (instruction >> 20) & 0b111111111111; // 12 bit opcode

    uint8_t opcode6 = (instruction >> 26) & 0b111111; // 6 bit opcode  

    // Decodificar y ejecutar la instrucción
    // Los valores de cada case me los tiro el chat. 
    // Se supone que son los opcodes de cada instrucción y los valores se pueden encontrar en TP1-ARM/ref/DDI0487B_a_armv8_arm.pdf
    for(int i = 0; i < sizeof(instructions_list) / sizeof(instruction_t); i++){
        if(opcode == instructions_list[i].opcode){
            instructions_list[i].instruction(instruction);
            break;
        }
    }
}

