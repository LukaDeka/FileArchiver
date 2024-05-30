#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>
#include "heap.h"

typedef struct {
    uint8_t buffer;
    int     bit_i;
    uint8_t padding_bits;
    bool    is_eof;
} Byte;

void flush_byte(FILE* out, Byte* byte);

void write_bit(FILE* out, Byte* byte, int bit);

uint8_t read_bit(FILE* in, Byte* byte);

uint8_t read_byte(FILE* in, Byte* byte);

// read bits from input, traverse Huffman tree and return value of leaf node 
int decode_char(FILE* in, Node* curr, Byte* byte);

void write_tree(FILE* out, Node* head);

Node* read_tree(FILE* in);

void encode(char* in_file, char* out_file);

void decode(char* in_file);

// assemble Huffman tree from input file
Node* read_tree_recusive(FILE* in, Byte* byte);

void write_tree_recursive(FILE* out, Node* curr, Byte* byte);


#endif
