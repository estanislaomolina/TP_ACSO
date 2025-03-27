#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <stdint.h>  // Para tipos de datos como uint64_t
#include "../shell.h"   // Incluye la definición de CPU_State y otras estructuras
#include "sim_instructions.h"
#include "../utils.h"


void hlt(){
    printf("HLT: Halting simulation.\n");
    RUN_BIT = 0;
}

void adds_imm(uint32_t instr) {
    // Extraer los campos de la instrucción

    uint8_t rd = extract_bits(instr, 0, 4);
    uint8_t rn = extract_bits(instr, 5, 9);
    uint16_t imm12 = extract_bits(instr, 10, 21);
    uint8_t shift = extract_bits(instr, 22, 23);
    

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = imm12;

    // Aplicar el shift (0 = sin shift, 1 = shift de 12 bits)
    if (shift == 0b01) {
        operand2 <<= 12;
    }
    
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
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino (5 bits)
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente (5 bits)
    uint8_t rm = (instr >> 16) & 0x1F; // Registro fuente 2 (5 bits)
    uint8_t shamt = (instr >> 10) & 0x3F; // Cantidad de bits a desplazar (6 bits)

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    operand2 <<= shamt;
    
    // Realizar la suma
    uint64_t result = operand1 + operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}

void subs_imm(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint16_t imm = (instr >> 10) & 0xFFF;  // Inmediato de 12 bits

    uint8_t shift = (instr >> 22) & 0b11;

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = imm;

    if (shift == 0b01) {
        operand2 <<= 12;
    }
    
    // Realizar la resta
    uint64_t result = operand1 - operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}

void subs_ext(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F;  // Registro fuente 2
    uint8_t shamt = (instr >> 10) & 0x3F;  // Cantidad de bits a rotar

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm] << shamt;
    
    // Realizar la resta
    uint64_t result = operand1 - operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}

void ands(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F;  // Registro fuente 2

    // Obtener el valor del registro fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    
    // Realizar la operación AND
    uint64_t result = operand1 & operand2;
    
    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;
    
    // Actualizar los flags NZ
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}

void eors(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F;  // Registro fuente 2

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
}

void orr(uint32_t instr){
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F;  // Registro fuente 2

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
}

void b(uint32_t instr){
    // Extraer los campos de la instrucción
    uint8_t imm26 = (instr >> 0) & 0x3FFFFFF;  // Inmediato de 26 bits

    // Calcular la dirección de salto
    uint64_t offset = imm26 << 2;
    uint64_t target = CURRENT_STATE.PC + offset;

    // Saltar a la dirección
    NEXT_STATE.PC = target;
}

void br(uint32_t instr){
    // Extraer los campos de la instrucción
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente

    // Saltar a la dirección
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
}

void beq(uint32_t instr) {
    // Extract the 19-bit immediate value (bits [23:5])
    int32_t imm19 = (instr >> 5) & 0x7FFFF;

    // Perform sign extension for the 19-bit immediate value
    if (imm19 & (1 << 18)) { // Check if the 19th bit (sign bit) is set
        imm19 |= 0xFFF80000; // Sign-extend to 32 bits
    }

    // Calculate the branch offset (shift left by 2 as per ARM spec)
    int32_t offset = imm19 << 2;

    // Check if the Zero flag (Z) is set
    if (CURRENT_STATE.FLAG_Z == 1) {
        // Update the Program Counter (PC) to the branch target
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        // If the condition is not met, increment PC to the next instruction
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void bne(uint32_t instr) {
    // Extract the 4-bit immediate value after the first 8 bits opcode
    uint8_t cond = (instr >> 0) & 0xF;

}

void bgt(uint32_t instr) {
    // Extract the 4-bit immediate value after the first 8 bits opcode
    uint8_t cond = (instr >> 0) & 0xF;
}

void bge(uint32_t instr) {
    // Extract the 4-bit immediate value after the first 8 bits opcode
    uint8_t cond = (instr >> 0) & 0xF;
}

void blt(uint32_t instr) {
    // Extract the 19-bit immediate value (bits [23:5])
    int32_t imm19 = (instr >> 5) & 0x7FFFF;

    // Perform sign extension for the 19-bit immediate value
    if (imm19 & (1 << 18)) { // Check if the 19th bit (sign bit) is set
        imm19 |= 0xFFF80000; // Sign-extend to 32 bits
    }

    // Calculate the branch offset (shift left by 2 as per ARM spec)
    int32_t offset = imm19 << 2;

    // Check if the Negative flag (N) is set and Zero flag (Z) is clear
    if (CURRENT_STATE.FLAG_N == 1 && CURRENT_STATE.FLAG_Z == 0) {
        // Update the Program Counter (PC) to the branch target
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        // If the condition is not met, increment PC to the next instruction
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void ble(uint32_t instr) {
    // Extract the 4-bit immediate value after the first 8 bits opcode
    uint8_t cond = (instr >> 0) & 0xF;
}

void bcond(uint32_t instr){
    // Extraer los campos de la instrucción
    uint8_t cond = (instr >> 0) & 0xF;  // Condición de salto
    uint8_t imm19 = (instr >> 5) & 0x7FFFF;  // Inmediato de 19 bits

    // Calcular la dirección de salto
    uint64_t offset = imm19 << 2;
    uint64_t target = CURRENT_STATE.PC + offset;

    // Saltar a la dirección si se cumple la condición
    // Los valores de cond representan los bits [3:0] de la instrucción

    if (cond == 0b0000) {  // BEQ
        beq(instr);
    } else if (cond == 0b0001) {  // BNE
        bne(instr);
    } else if (cond == 0b1010) {  // BGT
        bgt(instr);
    } else if (cond == 0b1011) {  // BLT
        bge(instr);
    } else if (cond == 0b1100) {  // BGE
        blt(instr);
    } else if (cond == 0b1101) {  // BLE
        ble(instr);
  
    } else {
        printf("Unknown condition: ");
        hlt();
    }

    // Saltar a la dirección
    //NEXT_STATE.PC = target;
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