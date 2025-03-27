#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <stdint.h>  // Para tipos de datos como uint64_t


#ifndef _SIM_H_
#define _SIM_H_


typedef struct {
    void (*instruction)(uint32_t);
    uint32_t opcode;
} instruction_t;

const instruction_t instructions_list[];
void process_instruction();

#endif