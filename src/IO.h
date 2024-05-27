#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>
#include "heap.h"

// #define END_OF_FILE 255

void flush_byte(FILE* out, uint8_t* byte);

void write_bit(FILE* out, uint8_t* byte, int* bit_i, int bit);

uint8_t read_bit(FILE* in, uint8_t* byte, int* bit_i);

uint8_t read_byte(FILE* in, uint8_t* byte, int* bit_i);

void write_tree(FILE* out, Node* head);

Node* read_tree(FILE* in);

void encode(char* in_file, char* out_file);

void decode(char* in_file);

// read bits from input, traverse Huffman tree and return value of leaf node 
int decode_char(FILE* in, Node* curr, uint8_t* byte, int* bit_i);

// assemble Huffman tree from input file
Node* read_tree_recusive(FILE* in, uint8_t* byte, int* bit_i);

void write_tree_recursive(FILE* out, Node* curr, uint8_t* byte, int* bit_i);


#endif
