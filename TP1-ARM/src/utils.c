#include <stdio.h>
#include <stdint.h>

#include "utils.h"


void print_binary(uint32_t num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1); // Extract each bit and print it
    }
    printf("\n");
}

uint32_t extract_bits(uint32_t num, uint8_t start, uint8_t end) {
    uint32_t mask = 0;
    for (int i = start; i <= end; i++) {
        mask |= 1 << i;
    }
    return (num & mask) >> start;
}

int16_t sign_extend(uint32_t value, int bits) {
    int32_t mask = 1 << (bits - 1);
    return (value ^ mask) - mask;
}