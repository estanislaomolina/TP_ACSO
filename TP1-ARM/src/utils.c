#include <stdio.h>
#include <stdint.h>
#include "utils.h"

void print_binary(uint32_t num)
{
    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

uint32_t extract_bits(uint32_t num, uint8_t start, uint8_t end)
{
    uint32_t mask = 0;
    for (int i = start; i <= end; i++)
    {
        mask |= 1 << i;
    }
    return (num & mask) >> start;
}

int64_t sign_extend(uint32_t number, int bits)
{
    if ((number >> (bits - 1)))
    {
        number |= 0xFFFFFFFFFFFFFFFF << bits;
    }

    return number;
}