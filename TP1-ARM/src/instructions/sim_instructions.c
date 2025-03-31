#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../shell.h" // Includes the definition of CPU_State and other structures
#include "sim_instructions.h"
#include "../utils.h"

// Halts the simulation
void hlt()
{
    printf("HLT: Halting simulation.\n");
    RUN_BIT = 0;
}

// Adds an immediate value to a register and updates flags
void adds_imm(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);       // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);       // Source register
    uint16_t imm12 = extract_bits(instr, 10, 21); // Immediate value
    uint8_t shift = extract_bits(instr, 22, 23);  // Shift type

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = imm12;

    // Apply the shift (0 = no shift, 1 = 12-bit shift)
    if (shift == 0b01)
    {
        operand2 <<= 12;
    }

    uint64_t result = operand1 + operand2;

    // Store the result in the destination register
    NEXT_STATE.REGS[rd] = result;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Adds two registers with an extended shift and updates flags
void adds_ext(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);      // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);      // First source register
    uint8_t rm = extract_bits(instr, 16, 20);    // Second source register
    uint8_t shamt = extract_bits(instr, 10, 15); // Shift amount

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    operand2 <<= shamt; // Apply shift

    uint64_t result = operand1 + operand2;

    // Store the result in the destination register
    NEXT_STATE.REGS[rd] = result;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Subtracts an immediate value from a register and updates flags
void subs_imm(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);      // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);      // Source register
    uint16_t imm = extract_bits(instr, 10, 21);  // Immediate value
    uint8_t shift = extract_bits(instr, 22, 23); // Shift type

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = imm;

    if (shift == 0b01)
    {
        operand2 <<= 12; // Apply shift
    }

    uint64_t result = operand1 - operand2;

    // Store the result in the destination register
    NEXT_STATE.REGS[rd] = result;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Subtracts two registers with an extended shift and updates flags
void subs_ext(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);      // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);      // First source register
    uint8_t rm = extract_bits(instr, 16, 20);    // Second source register
    uint8_t shamt = extract_bits(instr, 10, 15); // Shift amount

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm] << shamt; // Apply shift

    uint64_t result = operand1 - operand2;

    // Store the result in the destination register
    NEXT_STATE.REGS[rd] = result;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Performs a bitwise AND operation and updates flags
void ands(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);   // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);   // First source register
    uint8_t rm = extract_bits(instr, 16, 20); // Second source register

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];

    uint64_t result = operand1 & operand2;

    // Store the result in the destination register
    NEXT_STATE.REGS[rd] = result;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Performs a bitwise XOR operation
void eors(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);   // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);   // First source register
    uint8_t rm = extract_bits(instr, 16, 20); // Second source register

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Performs a bitwise OR operation
void orr(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);   // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);   // First source register
    uint8_t rm = extract_bits(instr, 16, 20); // Second source register

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Unconditional branch
void b(uint32_t instr)
{
    int32_t imm26 = sign_extend(extract_bits(instr, 0, 25), 26); // Immediate offset
    int32_t offset =  imm26 << 2;           // Convert to byte offset
    NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
}

// Branch to address in register
void br(uint32_t instr)
{
    uint8_t rn = extract_bits(instr, 5, 9); // Register containing target address
    uint64_t target_address = CURRENT_STATE.REGS[rn];
    NEXT_STATE.PC = target_address - 4;
}

// Compare two registers and update flags
void cmp(uint32_t instr)
{
    uint8_t rn = extract_bits(instr, 5, 9);      // First source register
    uint8_t rm = extract_bits(instr, 16, 20);    // Second source register
    uint8_t shamt = extract_bits(instr, 10, 15); // Shift amount

    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm] << shamt; // Apply shift
    uint64_t result = operand1 - operand2;

    // Update flags
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;
}

// Branch if equal (zero flag set)
void beq(uint32_t instr)
{
    int32_t imm19 = (instr >> 5) & 0x7FFFF; // Immediate offset
    if (imm19 & (1 << 18))
    { // Sign-extend if negative
        imm19 |= 0xFFF80000;
    }
    int32_t offset = imm19 << 2; // Convert to byte offset
    if (CURRENT_STATE.FLAG_Z == 1)
    { // Check zero flag
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Branch if not equal (zero flag clear)
void bne(uint32_t instr)
{
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (!(CURRENT_STATE.FLAG_Z))
    { // Check zero flag
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Branch if greater than (zero flag clear and negative flag clear)
void bgt(uint32_t instr)
{
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.FLAG_Z == 0 && CURRENT_STATE.FLAG_N == 0)
    { // Check flags
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Branch if less than (negative flag set)
void blt(uint32_t instr)
{
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.FLAG_N == 1)
    { // Check negative flag
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Branch if greater than or equal (negative flag clear)
void bge(uint32_t instr)
{
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.FLAG_N == 0)
    { // Check negative flag
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Branch if less than or equal (zero flag set or negative flag set)
void ble(uint32_t instr)
{
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.FLAG_Z == 1 || CURRENT_STATE.FLAG_N == 1)
    { // Check flags
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4;
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC;
    }
}

// Conditional branch based on condition code
void bcond(uint32_t instr)
{
    uint8_t cond = extract_bits(instr, 0, 3); // Condition code
    printf("Condition:");
    print_binary(cond);
    printf("\n");

    if (cond == 0b0000)
    { // BEQ
        beq(instr);
    }
    else if (cond == 0b0001)
    { // BNE
        bne(instr);
    }
    else if (cond == 0b1100)
    { // BGT
        bgt(instr);
    }
    else if (cond == 0b1010)
    { // BLT
        bge(instr);
    }
    else if (cond == 0b1011)
    { // BGE
        blt(instr);
    }
    else if (cond == 0b1101)
    { // BLE
        ble(instr);
    }
    else
    {
        printf("Unknown condition: ");
        hlt();
    }
}

// Move wide with zero: Moves an immediate value into a register with optional shift
void movz(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);           // Destination register
    uint16_t imm16 = extract_bits(instr, 5, 20);      // Immediate value
    uint8_t shift = extract_bits(instr, 21, 22);      // Shift amount
    uint64_t value = (uint64_t)imm16 << (shift * 16); // Apply shift
    NEXT_STATE.REGS[rd] = value;                      // Store result in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;                 // Update program counter
}

// Logical shift left: Shifts a register value to the left
void lsl(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);            // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);            // Source register
    uint32_t imms = extract_bits(instr, 10, 15);       // Immediate shift value
    uint64_t shift = 63 - imms;                        // Calculate shift amount
    uint64_t result = CURRENT_STATE.REGS[rn] << shift; // Perform shift
    NEXT_STATE.REGS[rd] = result;                      // Store result in destination register
    NEXT_STATE.FLAG_N = (result < 0);                  // Update negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;         // Update zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;                  // Update program counter
}

// Logical shift right: Shifts a register value to the right
void lsr(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);            // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);            // Source register
    uint64_t immr = extract_bits(instr, 16, 21);       // Immediate shift value
    uint64_t shift = immr;                             // Calculate shift amount
    uint64_t result = CURRENT_STATE.REGS[rn] >> shift; // Perform shift
    NEXT_STATE.REGS[rd] = result;                      // Store result in destination register
    NEXT_STATE.FLAG_N = (result < 0);                  // Update negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;         // Update zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;                  // Update program counter
}

// Shifts: Determines whether to perform logical shift left or right
void shifts(uint32_t instr)
{
    printf("Shifts function enter\n");
    uint32_t imms = extract_bits(instr, 10, 15); // Immediate shift value
    if (imms == 0b111111)
    { // Check if logical shift right
        lsr(instr);
    }
    else
    { // Otherwise, perform logical shift left
        lsl(instr);
    }
}

// Store register: Stores a register value into memory
void stur(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);            // Source register
    uint8_t rn = extract_bits(instr, 5, 9);            // Base register
    uint16_t imm12 = extract_bits(instr, 10, 21);      // Immediate offset
    uint64_t address = CURRENT_STATE.REGS[rn] + imm12; // Calculate address
    mem_write_32(address, CURRENT_STATE.REGS[rt]);     // Write value to memory
    NEXT_STATE.PC = CURRENT_STATE.PC;                  // Update program counter
}

// Store register byte: Stores the least significant byte of a register into memory
void sturb(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                     // Source register
    uint8_t rn = extract_bits(instr, 5, 9);                     // Base register
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Immediate offset
    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;           // Calculate address
    uint32_t data = CURRENT_STATE.REGS[rt] & 0xFF;              // Extract least significant byte
    uint32_t values = mem_read_32(address);                     // Read memory
    values = (values & 0xFFFFFF00) | data;                      // Update byte in memory
    mem_write_32(address, values);                              // Write back to memory
    NEXT_STATE.PC = CURRENT_STATE.PC;                           // Update program counter
}

// Store register halfword: Stores the least significant halfword of a register into memory
void sturh(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                      // Source register
    uint8_t rn = extract_bits(instr, 5, 9);                      // Base register
    uint16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Immediate offset
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;            // Calculate address
    uint32_t data = CURRENT_STATE.REGS[rt];                      // Extract halfword
    mem_write_32(address, data);                                 // Write to memory
    NEXT_STATE.PC = CURRENT_STATE.PC;                            // Update program counter
}

// Load register: Loads a 64-bit value from memory into a register
void ldur(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                     // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);                     // Base register
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Immediate offset
    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;           // Calculate address
    uint64_t values = mem_read_32(address + 4);                 // Read upper 32 bits
    values = (values << 32) | mem_read_32(address);             // Combine with lower 32 bits
    NEXT_STATE.REGS[rt] = values;                               // Store in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;                           // Update program counter
}

// Load register byte: Loads the least significant byte from memory into a register
void ldurb(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                     // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);                     // Base register
    int16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9); // Immediate offset
    uint32_t address = CURRENT_STATE.REGS[rn] + imm9;           // Calculate address
    uint64_t values = mem_read_32(address) & 0xFF;              // Read and mask byte
    NEXT_STATE.REGS[rt] = values;                               // Store in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;                           // Update program counter
}

// Load register halfword: Loads a 16-bit value from memory into a register
void ldurh(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                                   // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);                                   // Base register
    uint16_t imm9 = sign_extend(extract_bits(instr, 12, 20), 9);              // Immediate offset
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;                         // Calculate address
    uint32_t word = mem_read_32(address & ~0x3);                              // Read aligned word
    uint16_t value = (address & 0x2) ? (word >> 16) & 0xFFFF : word & 0xFFFF; // Extract halfword
    NEXT_STATE.REGS[rt] = (uint64_t)value;                                    // Store in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;                                         // Update program counter
}

// Move immediate: Moves an immediate value into a register
void mov(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);         // Destination register
    uint16_t imm16 = extract_bits(instr, 5, 20);    // Immediate value
    uint8_t shift = extract_bits(instr, 21, 22);    // Shift amount
    uint64_t imm = (uint64_t)imm16 << (shift * 16); // Apply shift
    NEXT_STATE.REGS[rd] = imm;                      // Store in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;               // Update program counter
}

// Compare and branch if not zero: Branches if a register is not zero
void cbnz(uint32_t instr)
{
    uint8_t rn = extract_bits(instr, 0, 4);                      // Source register
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.REGS[rn] != 0)
    {                                                  // Check if register is not zero
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4; // Branch
    }
    else
    {
        NEXT_STATE.PC = CURRENT_STATE.PC; // No branch
    }
}

// Compare and branch if zero: Branches if a register is zero
void cbz(uint32_t instr)
{
    uint8_t rt = extract_bits(instr, 0, 4);                      // Source register
    int32_t imm19 = sign_extend(extract_bits(instr, 5, 23), 19); // Immediate offset
    int32_t offset = imm19 << 2;                                 // Convert to byte offset
    if (CURRENT_STATE.REGS[rt] == 0)
    {                                                  // Check if register is zero
        NEXT_STATE.PC = CURRENT_STATE.PC + offset - 4; // Branch
    }
}

// Multiply: Multiplies two registers and stores the result
void mul(uint32_t instr)
{
    uint8_t rd = extract_bits(instr, 0, 4);     // Destination register
    uint8_t rn = extract_bits(instr, 5, 9);     // First source register
    uint8_t rm = extract_bits(instr, 16, 20);   // Second source register
    uint64_t operand1 = CURRENT_STATE.REGS[rn]; // First operand
    uint64_t operand2 = CURRENT_STATE.REGS[rm]; // Second operand
    uint64_t result = operand1 * operand2;      // Perform multiplication
    NEXT_STATE.REGS[rd] = result;               // Store result in destination register
    NEXT_STATE.PC = CURRENT_STATE.PC;           // Update program counter
}

// Add immediate: Adds an immediate value to a register
void add_imm(uint32_t instr)
{
    uint32_t rd = extract_bits(instr, 0, 4);      // Destination register
    uint32_t rn = extract_bits(instr, 5, 9);      // Source register
    uint32_t imm12 = extract_bits(instr, 10, 21); // Immediate value
    uint32_t shift = extract_bits(instr, 22, 23); // Shift type
    uint64_t operand1 = CURRENT_STATE.REGS[rn];   // First operand
    uint64_t operand2 = imm12;                    // Second operand
    if (shift == 0b01)
    { // Apply shift if specified
        operand2 <<= 12;
    }
    uint64_t result = operand1 + operand2;     // Perform addition
    NEXT_STATE.REGS[rd] = result;              // Store result in destination register
    NEXT_STATE.FLAG_N = (result >> 63) & 1;    // Update negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Update zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;          // Update program counter
}

// Add extended: Adds two registers with an extended shift
void add_ext(uint32_t instr)
{
    uint32_t rd = extract_bits(instr, 0, 4);             // Destination register
    uint32_t rn = extract_bits(instr, 5, 9);             // First source register
    uint32_t rm = extract_bits(instr, 16, 20);           // Second source register
    uint32_t shamt = extract_bits(instr, 10, 15);        // Shift amount
    uint64_t operand1 = CURRENT_STATE.REGS[rn];          // First operand
    uint64_t operand2 = CURRENT_STATE.REGS[rm] << shamt; // Apply shift to second operand
    uint64_t result = operand1 + operand2;               // Perform addition
    NEXT_STATE.REGS[rd] = result;                        // Store result in destination register
    NEXT_STATE.FLAG_N = (result >> 63) & 1;              // Update negative flag
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;           // Update zero flag
    NEXT_STATE.PC = CURRENT_STATE.PC;                    // Update program counter
}
