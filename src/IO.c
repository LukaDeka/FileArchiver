#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "IO.h"
#include "heap.h"
#include "huffman.h"

// typedef struct {
//     uint8_t buffer;
//     int bit_i;
//     bool is_eof;
// } ByteBuffer;

void flush_byte(FILE* out, uint8_t* byte) {
    fwrite(byte, sizeof(uint8_t), 1, out);
    *byte = 0;
}

// bit_i visualisation: 7654 3210.
// writes a single bit to a byte buffer, writes the buffer to file if full
void write_bit(FILE* out, uint8_t* byte, int* bit_i, int bit) {
    if (*bit_i < 0) { // byte buffer full
        flush_byte(out, byte);
        *bit_i = 7;
    }

    // put bit at correct index
    *byte |= bit << ((*bit_i)--);
}

uint8_t padding_bits = 0;
bool is_eof = false;

// returns the next bit or END_OF_FILE, reads a byte if necessary
uint8_t read_bit(FILE* in, uint8_t* byte, int* bit_i) {
    if (*bit_i < 0) { // byte buffer empty
        int ch = getc(in);
        if (ch == EOF) { is_eof = true; }
        *byte = ch;
        *bit_i = 7;
    }

    // peek if next character is EOF
    int next_ch = getc(in);
    if (next_ch == EOF && *bit_i < padding_bits) {
        is_eof = true;
    } else {
        ungetc(next_ch, in); // push it back if not
    }

    return (*byte >> (*bit_i)--) & 1;
}

// TODO: optimise this by reading two bytes at once
uint8_t read_byte(FILE* in, uint8_t* byte, int* bit_i) {
    uint8_t ret = 0;
    for (size_t i = 8; i--;) {
        ret = (ret << 1) | read_bit(in, byte, bit_i);
    }
    return ret;
}

int decode_char(FILE* in, Node* curr, uint8_t* byte, int* bit_i) {
    while (!curr->is_leaf) {
        uint8_t bit = read_bit(in, byte, bit_i);
        curr = bit ? curr->right : curr->left;
    }
    return curr->ch;
}

// main function for encoding files
void encode(char* in_file, char* out_file) {
    FILE* in  = fopen(in_file,  "rb");
    FILE* out = fopen(out_file, "wb");

    // write filename + '\0' + one byte-wide buffer for storing
    // the amount of padding bits at the end of the file
    // NOTE: strlen assumes argv is always null-terminated
    size_t filename_len = strlen(in_file) + 1;
    fwrite(in_file, sizeof(char), filename_len, out);
    fputc(0, out);

    // construct and get codes from Huffman tree
    Node* head = new_tree(in);
    char** codes = extract_codes_from_tree(head);
    // print_tree(head, 0);

    // write Huffman tree to file
    uint8_t byte = 0;  // byte buffer
    int     bit_i = 7; // bit index
    write_tree_recursive(out, head, &byte, &bit_i);
    free_tree(head);

    // read chars from "in" and write encoded bits to "out"
    int ch, codes_bit;
    while ((ch = getc(in)) != EOF) {
        size_t code_i = 0;
        while ((codes_bit = codes[ch][code_i++]) != '\0') {
            write_bit(out, &byte, &bit_i, codes_bit - '0');
        }
    }
    flush_byte(out, &byte);
    free_codes(codes);

    // rewind after "filename + \0" and write the amount of padding bits
    fseek(out, filename_len, SEEK_SET);
    padding_bits = (bit_i + 1) % 8;
    fputc(padding_bits, out);

    fclose(in);
    fclose(out);
}

void decode(char* in_file) {
    FILE* in  = fopen(in_file,  "rb");

    // variables for byte buffer and bit index
    uint8_t byte = 0;
    int bit_i = -1;

    // read filename into buffer
    char* filename_buffer = NULL;
    size_t buffer_len = 0;
    getdelim(&filename_buffer, &buffer_len, '\0', in);
    padding_bits = getc(in);

    puts(filename_buffer);
    FILE* out = fopen(filename_buffer, "wb");
    free(filename_buffer);

    // read Huffman tree from file
    Node* head = read_tree_recusive(in, &byte, &bit_i);

    // read chars from "in" and write decoded bits to "out"
    for (;;) {
        int decoded_char = decode_char(in, head, &byte, &bit_i);
        if (is_eof) { break; }
        fputc(decoded_char, out);
    }

    free_tree(head);
    fclose(in);
    fclose(out);
}

/*
    FUNCTIONS FOR WRITING/READING HUFFMAN TREE FROM FILE
*/

/*
    write a Huffman tree to file:
    write 1 if the node is a leaf, followed by the character of the leaf
    write 0 if the node is an internal node, recursively call left and right nodes
*/
void write_tree_recursive(FILE* out, Node* curr, uint8_t* byte, int* bit_i) {
    if (curr->is_leaf) { // write 1 bit and character of node
        write_bit(out, byte, bit_i, 1);
        for (size_t i = 8; i--;) {
            write_bit(out, byte, bit_i, (curr->ch >> i) & 1);
        }
    } else {
        write_bit(out, byte, bit_i, 0);
        write_tree_recursive(out, curr->left, byte, bit_i);
        write_tree_recursive(out, curr->right, byte, bit_i);
    }
}

/*
    read a Huffman tree from file:
    read a bit, if 1, read a byte and return a new leaf node
    if 0, read two new nodes recursively and return a new internal node
*/
Node* read_tree_recusive(FILE* in, uint8_t* byte, int* bit_i) {
    bool is_leaf = read_bit(in, byte, bit_i);

    if (is_leaf) { // create a new leaf node
        Node* new_leaf = malloc(sizeof(Node));
        new_leaf->ch = read_byte(in, byte, bit_i);
        new_leaf->is_leaf = true;
        return new_leaf;
    } else { // create a new internal node
        Node* new_node = malloc(sizeof(Node));
        new_node->left = read_tree_recusive(in, byte, bit_i);
        new_node->right = read_tree_recusive(in, byte, bit_i);
        new_node->is_leaf = false;
        return new_node;
    }
}
