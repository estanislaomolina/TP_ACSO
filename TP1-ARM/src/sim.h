#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <stdint.h>

#ifndef _SIM_H_
#define _SIM_H_

typedef struct
{
    void (*instruction)(uint32_t);
    uint32_t opcode;
    uint8_t size;
} instruction_t;

const instruction_t instructions_list[];
void process_instruction();

#endif