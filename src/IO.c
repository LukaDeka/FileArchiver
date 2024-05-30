#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include "IO.h"
#include "heap.h"
#include "huffman.h"

size_t get_filesize(char* path) {
    FILE* fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

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
void encode(char* in_filename, char* out_filename) {
    FILE* in  = fopen(in_filename,  "rb");
    FILE* out = fopen(out_filename, "wb");

    // remove path from filename    
    size_t filename_len = 0;
    if (strcmp(out_filename, TEMP_FILENAME)) {
        char* temp_in_filename = strdup(in_filename);
        char* in_basename = basename(temp_in_filename);
        filename_len = strlen(in_basename);
        fwrite(in_basename, sizeof(char), filename_len, out);
        free(temp_in_filename);
    }
    fputc(0, out);

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
    fseek(out, filename_len + 1, SEEK_SET);
    byte.padding_bits = (byte.bit_i + 1) % 8;
    fputc(byte.padding_bits, out);

    fclose(in);
    fclose(out);

    // base case for multi-level encoding
    if (filename_len == 0) { return; }

    // recursively encode file again if desirable
    for(;;) {
        encode(out_filename, TEMP_FILENAME);

        if (get_filesize(out_filename) <= get_filesize(TEMP_FILENAME)) {
            remove(TEMP_FILENAME);
            break;
        }

        remove(out_filename);
        rename(TEMP_FILENAME, out_filename);
    }
}

void decode(char* in_filename) {
    FILE* in = fopen(in_filename,  "rb");

    // temporary files for storing decoding levels
    char* temp_filenames[] = { "0.tmp", "1.tmp" };
    static uint8_t temp_i = 1;
    temp_i = (temp_i + 1) % 2;

    // read filename into buffer
    char* filename_buffer = NULL;
    size_t buffer_len = 0;
    getdelim(&filename_buffer, &buffer_len, '\0', in);

    // if first char is '\0', file has been encoded more than once
    uint8_t is_multilevel = *filename_buffer == '\0';
    FILE* out = fopen(is_multilevel ? temp_filenames[temp_i] : filename_buffer, "wb");

    // initialize byte buffer
    Byte byte = { 0, -1, 0, false };
    byte.padding_bits = fgetc(in);

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

    if (is_multilevel) { // another level of encoding
        decode(temp_filenames[temp_i]);
    }

    remove("0.tmp");
    remove("1.tmp");

    free(filename_buffer);
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
