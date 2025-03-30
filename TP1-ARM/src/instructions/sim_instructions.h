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
void cbnz(uint32_t instr);
void mul(uint32_t instr);


void lsl(uint32_t instr);
void lsr(uint32_t instr);
void stur(uint32_t instr);
void sturb(uint32_t instr);
void sturh(uint32_t instr);
void ldur(uint32_t instr);
void ldurb(uint32_t instr);
void ldurh(uint32_t instr);
void mov(uint32_t instr);



#endif
