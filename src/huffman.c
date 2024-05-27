#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "huffman.h"
#include "heap.h"

Node* build_tree_from_array(HeapArray* heap) {
    while(heap->size > 1) {
        Node* new = malloc(sizeof(Node));
        new->ch = 0;
        new->left  = extract_min(heap);
        new->right = extract_min(heap);
        new->freq = new->left->freq + new->right->freq;
        new->is_leaf = false;

        insert_heap(heap, new);
    }
    return heap->arr[0]; // the only element left, head
}

Node* new_tree(FILE* in) {
    HeapArray* heap = new_heap();

    int ch;
    while ((ch = getc(in)) != EOF) {
        heap->arr[ch]->freq++;
    }
    fseek(in, 0, SEEK_SET);

    // transform unordered array into min-heap
    ssize_t i = (heap->size - 2) / 2;
    for (; i >= 0; --i) {
        heapify(heap, i);
    }

    Node* head = build_tree_from_array(heap); 

    free(heap->arr);
    free(heap);

    return head;
}

// recursive function to fill "codes" with Huffman codes
void extract_codes_from_tree_recursive(Node* curr, char** codes, char* buffer, size_t* index) {
    if (curr->is_leaf) {
        buffer[*index] = '\0';
        codes[curr->ch] = malloc((*index + 1) * sizeof(char));
        strncpy(codes[curr->ch], buffer, *index + 1);
        return;
    }

    // traverse left node
    buffer[(*index)++] = '0';
    extract_codes_from_tree_recursive(curr->left, codes, buffer, index);
    (*index)--;

    // traverse right node
    buffer[(*index)++] = '1';
    extract_codes_from_tree_recursive(curr->right, codes, buffer, index);
    (*index)--;
}

char** extract_codes_from_tree(Node* head) {
    char** codes = malloc(MAX_LEN * sizeof(char*));
    char buffer[MAX_LEN] = {0};
    size_t index = 0;
    extract_codes_from_tree_recursive(head, codes, buffer, &index);
    return codes;
}

void free_codes(char** codes) {
    for (int i = 0; i < MAX_LEN; i++) {
        free(codes[i]);
    }
    free(codes);
}

void free_tree(Node* curr) { // inorder walk
    if (curr->is_leaf == true) {
        free(curr);
        return;
    }
    free_tree(curr->left);
    free_tree(curr->right);
    free(curr);
}

void print_tree(Node* curr, size_t depth) {
    if (curr == NULL) { return; }

    for (; depth--;) { printf("  "); }
    printf("(%03d, %zu)\n", curr->ch, curr->freq);
    
    print_tree(curr->left,  depth + 1);
    print_tree(curr->right, depth + 1);
}
