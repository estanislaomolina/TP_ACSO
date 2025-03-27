#include <stdint.h>

#ifndef SIM_INSTRUCTIONS_H
#define SIM_INSTRUCTIONS_H

void hlt();
void adds_imm(uint32_t instr);
void adds_ext(uint32_t instr);
void subs_imm(uint32_t instr);
void subs_ext(uint32_t instr);
void ands(uint32_t instr);
void eors(uint32_t instr);


void orr(uint32_t instr);
void b(uint32_t instr);
void br(uint32_t instr);
void beq(uint32_t instr);
void bne(uint32_t instr);
void bgt(uint32_t instr);
void bge(uint32_t instr);
void blt(uint32_t instr);
void ble(uint32_t instr);
void bcond(uint32_t instr);

void movz(uint32_t instr);

void cbz(uint32_t instr);


void lsl(uint32_t rd, uint32_t rn, uint32_t shamt);


void stur(uint32_t rt, uint32_t rn, uint32_t imm12);
void sturb(uint32_t rt, uint32_t rn, uint32_t imm12);

void ldur(uint32_t rt, uint32_t rn, uint32_t imm12);
void ldurb(uint32_t rt, uint32_t rn, uint32_t imm12);




#endif
