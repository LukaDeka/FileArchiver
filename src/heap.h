#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEN 256

#define LEFT(x)   (2 * x + 1)
#define RIGHT(x)  (2 * x + 2)
#define PARENT(x) ((x - 1) / 2)

typedef struct _Node {
    uint8_t ch;
    size_t freq;
    bool is_leaf;
    struct _Node *left;
    struct _Node *right;
} Node;

typedef struct {
    Node** arr;
    size_t size;
} HeapArray;

// insert element into heap
void insert_heap(HeapArray* heap, Node* node);

// prints tree to stdout for debugging purposes
void print_tree(Node* curr, size_t depth);

// free Huffman tree
void free_tree(Node* curr);

// min-heapify
void heapify(HeapArray* heap, size_t i);

// returns min from min-heap and calls heapify
Node* extract_min(HeapArray* heap);

// creates and initializes HeapArray
HeapArray* new_heap();


#endif
