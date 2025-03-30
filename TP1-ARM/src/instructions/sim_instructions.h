#include <stdint.h>

#ifndef SIM_INSTRUCTIONS_H
#define SIM_INSTRUCTIONS_H

void hlt(); // FUNCIONAN
void adds_imm(uint32_t instr); // FUNCIONAN
void adds_ext(uint32_t instr); // FUNCIONAN
void subs_imm(uint32_t instr); // FUNCIONAN
void subs_ext(uint32_t instr); // FUNCIONAN
void ands(uint32_t instr); // FUNCIONAN
void eors(uint32_t instr); // FUNCIONAN


void orr(uint32_t instr); // FUNCIONAN
void b(uint32_t instr); //funciona
void br(uint32_t instr); //revisar
void cmp(uint32_t instr); //FUNCIONAN
void beq(uint32_t instr); //FUNCIONAN
void bne(uint32_t instr); //FUNCIONAN
void bgt(uint32_t instr); //FUNCIONAN
void bge(uint32_t instr); //FUNCIONAN
void blt(uint32_t instr); //FUNCIONAN
void ble(uint32_t instr); //FUNCIONAN
void bcond(uint32_t instr); //FUNCIONAN

void movz(uint32_t instr); // FUNCIONAN

void cbz(uint32_t instr); //FUNCIONA
void cbnz(uint32_t instr); //FUNCIONA
void mul(uint32_t instr); //FUNCIONA


void lsl(uint32_t instr); // FUNCIONAN
void lsr(uint32_t instr); // FUNCIONAN
void shifts(uint32_t instr); // FUNCIONAN
void stur(uint32_t instr); // FUNCIONAN
void sturb(uint32_t instr); // FUNCIONAN
void sturh(uint32_t instr); // FUNCIONAN
void ldur(uint32_t instr); // FUNCIONAN
void ldurb(uint32_t instr); // FUNCIONAN
void ldurh(uint32_t instr); // FUNCIONAN
void mov(uint32_t instr); // FUNCIONAN

void add_imm(uint32_t instr); // FUNCIONAN
void add_ext(uint32_t instr); // FUNCIONAN


#endif
