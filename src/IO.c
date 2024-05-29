#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include "IO.h"
#include "heap.h"
#include "huffman.h"

void flush_byte(FILE* out, Byte* byte) {
    fwrite(&byte->buffer, sizeof(uint8_t), 1, out);
    byte->buffer = 0;
}

// writes a single bit to a byte buffer, writes the buffer to file if full
void write_bit(FILE* out, Byte* byte, int bit) {
    if (byte->bit_i < 0) { // byte buffer full
        flush_byte(out, byte);
        byte->bit_i = 7;
    }
    // put bit at correct index
    byte->buffer |= bit << (byte->bit_i--);
}

// returns the next bit, reads a byte if necessary
uint8_t read_bit(FILE* in, Byte* byte) {
    if (byte->bit_i < 0) { // byte buffer empty
        int ch = getc(in);
        if (ch == EOF) { byte->is_eof = true; }
        byte->buffer = ch;
        byte->bit_i = 7;
    }

    // peek if next character is EOF
    int next_ch = getc(in);
    if (next_ch == EOF && byte->bit_i < byte->padding_bits) {
        byte->is_eof = true;
    } else {
        ungetc(next_ch, in); // push it back if not
    }

    return (byte->buffer >> byte->bit_i--) & 1;
}

// TODO: optimise this by reading two bytes at once
uint8_t read_byte(FILE* in, Byte* byte) {
    uint8_t ret = 0;
    for (size_t i = 8; i--;) {
        ret = (ret << 1) | read_bit(in, byte);
    }
    return ret;
}

int decode_char(FILE* in, Node* curr, Byte* byte) {
    while (!curr->is_leaf) {
        uint8_t bit = read_bit(in, byte);
        curr = bit ? curr->right : curr->left;
    }
    return curr->ch;
}

// main function for encoding files
void encode(char* in_file, char* out_file) {
    FILE* in  = fopen(in_file,  "rb");
    FILE* out = fopen(out_file, "wb");

    // write filename
    char* temp_in_file = strdup(in_file);
    char* in_basename = basename(temp_in_file);
    size_t filename_len = strlen(in_basename) + 1;
    fwrite(in_basename, sizeof(char), filename_len, out);
    free(temp_in_file);

    // one byte buffer for storing the amount of padding bits
    fputc(0, out);

    // construct and get codes from Huffman tree
    Node* head = new_tree(in);
    char** codes = extract_codes_from_tree(head);
    // print_tree(head, 0);

    // create byte buffer and write Huffman tree to file
    Byte byte = { 0, 7, 0, false };
    write_tree_recursive(out, head, &byte);
    free_tree(head);

    // read chars from "in" and write encoded bits to "out"
    int ch, codes_bit;
    while ((ch = getc(in)) != EOF) {
        size_t code_i = 0;
        while ((codes_bit = codes[ch][code_i++]) != '\0') {
            write_bit(out, &byte, codes_bit - '0');
        }
    }
    flush_byte(out, &byte);
    free_codes(codes);

    // rewind after "filename" and write the amount of padding bits
    fseek(out, filename_len, SEEK_SET);
    byte.padding_bits = (byte.bit_i + 1) % 8;
    fputc(byte.padding_bits, out);

    fclose(in);
    fclose(out);

    // TODO: save encoded file,
    // recursively encode file AGAIN until >= current
    // recover current
    // delete all other versions
    // text.txt -> .huff -> .huff...
    // rename file to out_file
}

void decode(char* in_file) {
    FILE* in  = fopen(in_file,  "rb");

    // read filename into buffer
    char* filename_buffer = NULL;
    size_t buffer_len = 0;
    getdelim(&filename_buffer, &buffer_len, '\0', in);

    Byte byte = { 0, -1, 0, false };
    byte.padding_bits = getc(in);

    FILE* out = fopen(filename_buffer, "wb");
    free(filename_buffer);

    // read Huffman tree from file
    Node* head = read_tree_recusive(in, &byte);

    // read chars from "in" and write decoded bits to "out"
    for (;;) {
        int decoded_char = decode_char(in, head, &byte);
        if (byte.is_eof) { break; }
        fputc(decoded_char, out);
    }

    free_tree(head);
    fclose(in);
    fclose(out);

    // TODO: if filename == .huff
    // decode again recursively until no more
    // delete all other files
}

/*
    FUNCTIONS FOR WRITING/READING HUFFMAN TREE FROM FILE
*/

/*
    write a Huffman tree to file:
    write 1 if the node is a leaf, followed by the character of the leaf
    write 0 if the node is an internal node, recursively call left and right nodes
*/
void write_tree_recursive(FILE* out, Node* curr, Byte* byte) {
    if (curr->is_leaf) {
        write_bit(out, byte, 1);
        for (size_t i = 8; i--;) {
            write_bit(out, byte, (curr->ch >> i) & 1);
        }
    } else {
        write_bit(out, byte, 0);
        write_tree_recursive(out, curr->left, byte);
        write_tree_recursive(out, curr->right, byte);
    }
}

/*
    read a Huffman tree from file:
    read a bit, if 1, read a byte and return a new leaf node
    if 0, read two new nodes recursively and return a new internal node
*/
Node* read_tree_recusive(FILE* in, Byte* byte) {
    bool is_leaf = read_bit(in, byte);

    if (is_leaf) { // create a new leaf node
        Node* new_leaf = malloc(sizeof(Node));
        new_leaf->ch = read_byte(in, byte);
        new_leaf->is_leaf = true;
        return new_leaf;
    } else { // create a new internal node
        Node* new_node = malloc(sizeof(Node));
        new_node->left = read_tree_recusive(in, byte);
        new_node->right = read_tree_recusive(in, byte);
        new_node->is_leaf = false;
        return new_node;
    }
}
