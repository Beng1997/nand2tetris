#include "coder.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

char* int_to_binary_string(uint16_t value, int bits) {
    char* binary = (char*)malloc(bits + 1); // +1 for null terminator
    if (binary == NULL) {
        return NULL; // Allocation failed
    }
    /** Here we go from least significant bit to most significant bit
     * filling the string in reverse order while comparing each bit of value with 1
     * to determine if it should be '1' or '0'.
     * then we right shift the value to process the next bit.
     */
    binary[bits] = '\0'; // Null-terminate the string
    for (int i = bits - 1; i >= 0; i--) {
        binary[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    return binary;
}


char* encode_a_instruction(int value) {
    char* binary = int_to_binary_string((uint16_t)value, 16);
    if (binary == NULL) {
        return NULL; // Allocation failed
    }
    binary[0] = '0'; // Set the MSB to 0 for A-instruction
    return binary;
}

char* encode_c_instruction(const char* dest, const char* comp, const char* jump) {
    const char* comp_bits = encode_comp(comp);
    const char* dest_bits = encode_dest(dest);
    const char* jump_bits = encode_jump(jump);
    if (comp_bits == NULL || dest_bits == NULL || jump_bits == NULL) {
        return NULL; // Encoding failed
    }
    char* binary = (char*)malloc(17); // 16 bits + null terminator
    if (binary == NULL) {
        return NULL; // Allocation failed
    }
    snprintf(binary, 17, "111%s%s%s", comp_bits, dest_bits, jump_bits);// rembmer the first 3 bits are 111 for C-instruction
    return binary;
}


char* encode_dest(const char* dest) {
    // dest bits: d1 d2 d3 (A, D, M)
    static const char* dest_table[] = {
        "000", // 000  null
        "001", // 001  M
        "010", // 010  D
        "011", // 011  MD
        "100", // 100  A
        "101", // 101  AM
        "110", // 110  AD
        "111"  // 111  AMD
    };
    int index = 0;
    if (strchr(dest, 'A')) index |= 4; // d1
    if (strchr(dest, 'D')) index |= 2; // d2
    if (strchr(dest, 'M')) index |= 1; // d3
    return dest_table[index];
}

char* encode_comp(const char* comp) {
    // comp bits: a c1 c2 c3 c4 c5 c6
    struct { // a-bit + 6 c-bits
        const char* mnemonic;
        const char* bits;
    } 
    comp_table[] = {
        {"0",   "0101010"},
        {"1",   "0111111"},
        {"-1",  "0111010"},
        {"D",   "0001100"},
        {"A",   "0110000"},
        {"!D",  "0001101"},
        {"!A",  "0110001"},
        {"-D",  "0001111"},
        {"-A",  "0110011"},
        {"D+1", "0011111"},
        {"A+1", "0110111"},
        {"D-1", "0001110"},
        {"A-1", "0110010"},
        {"D+A", "0000010"},
        {"D-A", "0010011"},
        {"A-D", "0000111"},
        {"D&A", "0000000"},
        {"D|A", "0010101"},
        {"M",   "1110000"},
        {"!M",  "1110001"},
        {"-M",  "1110011"},
        {"M+1", "1110111"},
        {"M-1", "1110010"},
        {"D+M", "1000010"},
        {"D-M", "1010011"},
        {"M-D", "1000111"},
        {"D&M", "1000000"},
        {"D|M", "1010101"}
    };
    size_t num_comp = sizeof(comp_table) / sizeof(comp_table[0]); // Number of entries in the table but safe for future changes
    for (size_t i = 0; i < num_comp; i++) {
        if (strcmp(comp, comp_table[i].mnemonic) == 0) {
            return comp_table[i].bits; // Found matching comp
        }
    }
    return NULL; // Not found
}

char* encode_jump(const char* jump) {
    // jump bits: j1 j2 j3
    static const char* jump_table[] = {
        "000", // 000  null
        "001", // 001  JGT
        "010", // 010  JEQ
        "011", // 011  JGE
        "100", // 100  JLT
        "101", // 101  JNE
        "110", // 110  JLE
        "111"  // 111  JMP
    };
    if (strcmp(jump, "JGT") == 0) return jump_table[1];
    if (strcmp(jump, "JEQ") == 0) return jump_table[2];
    if (strcmp(jump, "JGE") == 0) return jump_table[3];
    if (strcmp(jump, "JLT") == 0) return jump_table[4];
    if (strcmp(jump, "JNE") == 0) return jump_table[5];
    if (strcmp(jump, "JLE") == 0) return jump_table[6];
    if (strcmp(jump, "JMP") == 0) return jump_table[7];
    return jump_table[0]; // No jump
}

