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

void b(uint32_t instr) {
    // Extract the 26-bit immediate value (imm26)
    int32_t imm26 = (instr >> 0) & 0x3FFFFFF;  // Immediate value (bits [25:0])

    // Perform sign extension for the 26-bit immediate value
    if (imm26 & (1 << 25)) { // Check if the 26th bit (sign bit) is set
        imm26 |= 0xFC000000; // Sign-extend to 32 bits
    }

    // Calculate the branch offset (shift left by 2 as per ARM spec)
    int32_t offset = imm26 << 2;

    // Update the Program Counter (PC) to the branch target
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
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


void movz(uint32_t instr) {
    // Extract the 5-bit destination register (rd), 16-bit immediate value (imm16), and shift
    uint8_t rd = (instr >> 0) & 0x1F;  // Destination register
    uint16_t imm16 = (instr >> 5) & 0xFFFF;  // Immediate value (bits [20:5])
    uint8_t shift = (instr >> 21) & 0b11;  // Shift amount (bits [22:21])

    // Shift the immediate value by (shift * 16) bits
    uint64_t value = (uint64_t)imm16 << (shift * 16);

    // Store the value in the destination register
    NEXT_STATE.REGS[rd] = value;
}

void lsl(uint32_t instr) {
    // Extraer campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;   // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;   // Registro fuente
    uint32_t imms = extract_bits(instr, 10, 15); // Inmediato de 6 bits
    uint64_t immr = extract_bits(instr, 16, 21); // Inmediato de 6 bits
    
    uint64_t shift = 63 - imms;
    uint64_t result = CURRENT_STATE.REGS[rn] << shift;
    NEXT_STATE.REGS[rd] = result;
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result<0);
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero

}

void lsr(uint32_t instr) {
    // Extraer campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;   // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;   // Registro fuente
    uint32_t imms = extract_bits(instr, 10, 15); // Inmediato de 6 bits
    uint64_t immr = extract_bits(instr, 16, 21); // Inmediato de 6 bits
    
    uint64_t shift = immr;
    uint64_t result = CURRENT_STATE.REGS[rn] >> shift;
    NEXT_STATE.REGS[rd] = result;
    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result<0);
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero

}

void shifts(uint32_t instr){
    printf("Shifts function enter\n");
    uint32_t rd = extract_bits(instr, 0, 4);
    uint32_t rn =extract_bits(instr, 5, 9);
    uint32_t imms = extract_bits(instr, 10, 15); 
    uint32_t immr = extract_bits(instr, 16, 21); 
  
    if(imms == 0b111111){
        printf("LSR: Logical Shift Right\n");
        lsr(instr);
    }else {
        printf("LSL: Logical Shift Left\n");
        lsl(instr);
    }
}

void stur(uint32_t instr) {
    // Extract fields from instruction
    uint8_t rt = (instr >> 0) & 0x1F;   // Register to store
    uint8_t rn = (instr >> 5) & 0x1F;   // Base register
    uint16_t imm12 = (instr >> 10) & 0xFFF;  // 12-bit immediate offset

    uint64_t address = CURRENT_STATE.REGS[rn] + imm12;
    mem_write_32(address, CURRENT_STATE.REGS[rt]);
}

void sturb(uint32_t instr) {
    printf("STURB: Storing byte to memory.\n");

    // Extraer campos de la instrucción
    uint8_t rt = extract_bits(instr, 0, 4);   // Registro destino (bits 0-4)
    uint8_t rn = extract_bits(instr, 5, 9);   // Registro base (bits 5-9)
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Offset de 9 bits

    // Calcular la dirección efectiva
    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;

    // Obtener el byte a almacenar
    uint32_t data = CURRENT_STATE.REGS[rt] & 0xFF;



    uint32_t values = mem_read_32(address);
    values = values & 0xFFFFFF00;
    values = values | data;

    // Escribir el byte en memoria
    mem_write_32(address, values);
}

void sturh(uint32_t instr) {
    // Extract fields from instruction
    uint8_t rt = (instr >> 0) & 0x1F;   // Register to store
    uint8_t rn = (instr >> 5) & 0x1F;   // Base register
    uint16_t imm12 = (instr >> 10) & 0xFFF;  // 12-bit immediate offset

    uint64_t address = CURRENT_STATE.REGS[rn] + imm12;
    mem_write_32(address, CURRENT_STATE.REGS[rt]);
}


void ldur(uint32_t instr) {
    printf("LDUR: Loading value from memory.\n");
    // Extract fields
    uint8_t rt = extract_bits(instr, 0, 4);   // Destination register (e.g., X1)
    uint8_t rn = extract_bits(instr, 5, 9);   // Base register (e.g., X2)
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Sign-extended 9-bit offset


    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;

    
    uint64_t values = mem_read_32(address + 4);
    values = values << 32;
    values = values | mem_read_32(address);

    // Store the loaded value into the destination register
    NEXT_STATE.REGS[rt] = values;
}

void ldurb(uint32_t instr) {
    uint8_t rt = extract_bits(instr, 0, 4);   // Destination register (e.g., X1)
    uint8_t rn = extract_bits(instr, 5, 9);   // Base register (e.g., X2)
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Sign-extended 9-bit offset


    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;

    
    uint64_t values = mem_read_32(address);
    values = values & 0xFF;

    // Store the loaded value into the destination register
    NEXT_STATE.REGS[rt] = values;
}

void ldurh(uint32_t instr) {
    // Extract fields from instruction
    uint8_t rt = (instr >> 0) & 0x1F;   // Register to load
    uint8_t rn = (instr >> 5) & 0x1F;   // Base register
    uint16_t imm12 = (instr >> 10) & 0xFFF;  // 12-bit immediate offset

    uint64_t address = CURRENT_STATE.REGS[rn] + imm12;
    mem_write_32(address, CURRENT_STATE.REGS[rt]);
}

void mov(uint32_t instr) {
    // Extract the 5-bit destination register (rd)
    uint8_t rd = (instr >> 0) & 0x1F;  

    // Extract the 16-bit immediate (imm16)
    uint16_t imm16 = (instr >> 5) & 0xFFFF;  

    // Extract shift amount (2 bits) to determine where to place imm16
    uint8_t shift = (instr >> 21) & 0x3;  

    // Apply the shift (shift values are 0, 16, 32, or 48 bits)
    uint64_t imm = (uint64_t)imm16 << (shift * 16);

    // Load the immediate value into the destination register
    NEXT_STATE.REGS[rd] = imm;

}

void cbnz(uint32_t instr) {
    // Extract the 5-bit register (rt) and 19-bit immediate value (imm19)
    uint8_t rt = (instr >> 0) & 0x1F;  // Test register
    int32_t imm19 = (instr >> 5) & 0x7FFFF;  // Immediate value (bits [23:5])

    // Perform sign extension for the 19-bit immediate value
    if (imm19 & (1 << 18)) { // Check if the 19th bit (sign bit) is set
        imm19 |= 0xFFF80000; // Sign-extend to 32 bits
    }

    // Calculate the branch offset (shift left by 2 as per ARM spec)
    int32_t offset = imm19 << 2;

    // Check if the register value is non-zero
    if (CURRENT_STATE.REGS[rt] != 0) {
        // Update the Program Counter (PC) to the branch target
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        // If the condition is not met, increment PC to the next instruction
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void cbz(uint32_t instr) {
    // Extract the 5-bit register (rt) and 19-bit immediate value (imm19)
    uint8_t rt = (instr >> 0) & 0x1F;  // Test register
    int32_t imm19 = (instr >> 5) & 0x7FFFF;  // Immediate value (bits [23:5])

    // Perform sign extension for the 19-bit immediate value
    if (imm19 & (1 << 18)) { // Check if the 19th bit (sign bit) is set
        imm19 |= 0xFFF80000; // Sign-extend to 32 bits
    }

    // Calculate the branch offset (shift left by 2 as per ARM spec)
    int32_t offset = imm19 << 2;

    // Check if the register value is zero
    if (CURRENT_STATE.REGS[rt] == 0) {
        // Update the Program Counter (PC) to the branch target
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        // If the condition is not met, increment PC to the next instruction
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

}

void mul(uint32_t instr) {
    // Extraer los campos de la instrucción
    uint8_t rd = (instr >> 0) & 0x1F;  // Registro destino
    uint8_t rn = (instr >> 5) & 0x1F;  // Registro fuente
    uint8_t rm = (instr >> 16) & 0x1F; // Registro fuente 2

    // Obtener el valor de los registros fuente
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    // Realizar la multiplicación
    uint64_t result = operand1 * operand2;

    // Almacenar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;

    // Actualizar los flags NZCV
    NEXT_STATE.FLAG_N = (result >> 63) & 1;  // Flag de negativo
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;  // Flag de cero
}