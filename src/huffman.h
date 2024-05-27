#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "heap.h"

// constructs a Huffman tree, returns head node
Node* build_tree_from_array(HeapArray* heap);

// construct a new Huffman tree from file
Node* new_tree(FILE* in);

// recursive function to fill "codes" with Huffman codes
void extract_codes_from_tree_recursive(Node* curr, char** codes, char* buffer, size_t* index);

char** extract_codes_from_tree(Node* head);

void free_codes(char** codes);


#endif
