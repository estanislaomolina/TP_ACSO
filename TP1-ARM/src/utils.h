#include <stdio.h>
#include <stdint.h>

void print_binary(uint32_t num);
uint32_t extract_bits(uint32_t num, uint8_t start, uint8_t end);
int64_t sign_extend(uint32_t value, int bits);