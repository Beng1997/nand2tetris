#ifndef CODER_H
#define CODER_H

#include <stdint.h>

char* int_to_binary_string(uint16_t value, int bits);

// A-instruction encoding: 0vvvvvvvvvvvvvvv (v = 15-bit value)
char* encode_a_instruction(int value);

// C-instruction encoding: 111accccccdddjjj
// where: a=1bit, c=6bits (comp), d=3bits (dest), j=3bits (jump)
char* encode_c_instruction(const char* dest, const char* comp, const char* jump);

// Individual field encoders for C-instruction
char* encode_dest(const char* dest);  // 3 bits
char* encode_comp(const char* comp);  // 7 bits (1 a-bit + 6 c-bits)
char* encode_jump(const char* jump);  // 3 bits

#endif // CODER_H