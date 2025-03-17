#include <stdio.h>
#include <assert.h>
#include <string.h>


#include <stdint.h>  // Para tipos de datos como uint64_t
#include "shell.h"   // Incluye la definición de CPU_State y otras estructuras

// Function prototypes for memory access
uint32_t mem_read_32(uint64_t address);
void mem_write_32(uint64_t address, uint32_t value);

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

    

    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");

}

