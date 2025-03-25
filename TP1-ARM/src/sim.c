#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras

void hlt(){
    printf("HLT: Halting simulation.\n");
    RUN_BIT = 0;
}

void adds_imm(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint16_t imm = (instr >> 10) & 0xFFF;  // Inmediato de 12 bits

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = imm;
    
    // Realizar la suma
    uint64_t result = operand1 + operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}



void adds_ext(uint32_t instr){
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F;  // Registro fuente 2
    uint8_t shamt = (instr >> 10) & 0x3F;  // Cantidad de bits a rotar

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm] << shamt;
    
    // Realizar la suma
    uint64_t result = operand1 + operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}

void subs_imm(uint32_t rd, uint32_t rn, uint32_t imm12){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] - imm12;
}

void subs_ext(uint32_t rd, uint32_t rn, uint32_t imm12){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] - imm12;
}

void ands(uint32_t rd, uint32_t rn, uint32_t rm){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];
}

void beq(uint32_t imm26){
    if (CURRENT_STATE.FLAG_Z == 1) {
        NEXT_STATE.PC = CURRENT_STATE.PC + imm26 * 4;
    }
}

void eors(uint32_t rd, uint32_t rn, uint32_t rm){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
}

void movz(uint32_t rd, uint32_t imm16){
    NEXT_STATE.REGS[rd] = imm16;
}

void lsl(uint32_t rd, uint32_t rn, uint32_t shamt){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] << shamt;
}

void stur(uint32_t rt, uint32_t rn, uint32_t imm12){
    mem_write_32(CURRENT_STATE.REGS[rn] + imm12, CURRENT_STATE.REGS[rt]);
}

void sturb(uint32_t rt, uint32_t rn, uint32_t imm12){
    // Adaptación para escribir 8 bits usando mem_write_32
    {
        uint64_t address = CURRENT_STATE.REGS[rn] + imm12;
        uint32_t value = mem_read_32(address);
        value = (value & 0xFFFFFF00) | (CURRENT_STATE.REGS[rt] & 0xFF);
        mem_write_32(address, value);
    }
}

void ldur(uint32_t rt, uint32_t rn, uint32_t imm12){
    NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rn] + imm12);
}

void ldurb(uint32_t rt, uint32_t rn, uint32_t imm12){
    // Adaptación para leer 8 bits usando mem_read_32
    {
        uint64_t address = CURRENT_STATE.REGS[rn] + imm12;
        uint32_t value = mem_read_32(address);
        NEXT_STATE.REGS[rt] = value & 0xFF;
    }
}

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
            hlt();
            break;
        
        case 0x588: // adds immediate
            adds_imm(instruction);
            break;

        case 0x558: // adds extended
            adds_ext(instruction);
            break;

        case 0x750: // ands
            NEXT_STATE.REGS[(instruction >> 0) & 0x1F] = CURRENT_STATE.REGS[(instruction >> 5) & 0x1F] & CURRENT_STATE.REGS[(instruction >> 16) & 0x1F];
            break;

        case 0x758: //subs immediate
            subs_imm((instruction >> 0) & 0x1F, (instruction >> 5) & 0x1F, instruction & 0xFFF);
            break;

        case 0x2A0: // beq
            if (CURRENT_STATE.FLAG_Z == 1) {
                NEXT_STATE.PC = CURRENT_STATE.PC + (instruction & 0x3FFFFFF) * 4;
            }
            break;

        case 0x650: // eors
            eors((instruction >> 0) & 0x1F, (instruction >> 5) & 0x1F, (instruction >> 16) & 0x1F);
            break;

        case 0x694: // movz (creo que mov tambien)
            movz((instruction >> 0) & 0x1F, instruction & 0xFFFF);
            break;

        case 0x69B: // lsl
            void lsl(uint32_t rd, uint32_t rn, uint32_t shamt);
            break;

        case 0x7C0: // stur
            void stur(uint32_t rt, uint32_t rn, uint32_t imm12);
            break;
        
        case 0x1C0: // sturb
            void sturb(uint32_t rt, uint32_t rn, uint32_t imm12);
            break;

        case 0x7C2: // ldur
            void ldur(uint32_t rt, uint32_t rn, uint32_t imm12);
            break;

        case 0x1C2: // ldurb
            void ldurb(uint32_t rt, uint32_t rn, uint32_t imm12);
            break;
        
        case 0x788: // subs extended
            subs_ext((instruction >> 0) & 0x1F, (instruction >> 5) & 0x1F, instruction & 0xFFF);
            break;

        default:
            printf("Unknown instruction with opcode: 0x%X\n", opcode);
            break;
    }

}

