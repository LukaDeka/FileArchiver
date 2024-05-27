#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heap.h"

void swap(Node* a, Node* b) {
    Node tmp = *a;
    *a = *b;
    *b = tmp;
}

void insert_heap(HeapArray* heap, Node* node) {
    size_t i = heap->size++;
    heap->arr[i] = node;
    while (i > 0 && heap->arr[PARENT(i)]->freq > heap->arr[i]->freq) {
        swap(heap->arr[PARENT(i)], heap->arr[i]);
        i = PARENT(i);
    }
}

void heapify(HeapArray* heap, size_t i) {
    Node** arr = heap->arr;
    size_t size = heap->size;

    // calculate minimum
    size_t min = i;
    if (LEFT(i) < size && arr[LEFT(i)]->freq < arr[min]->freq) {
        min = LEFT(i);
    }
    if (RIGHT(i) < size && arr[RIGHT(i)]->freq < arr[min]->freq) {
        min = RIGHT(i);
    }

    if (min != i) {
        swap(arr[i], arr[min]);
        heapify(heap, min);
    }
}

Node* extract_min(HeapArray* heap) {
    Node* ret = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
    return ret;
}

HeapArray* new_heap(void) {
    HeapArray* heap = malloc(sizeof(HeapArray));
    heap->size = MAX_LEN;
    heap->arr = malloc(heap->size * sizeof(Node*));
    for (size_t i = heap->size; i--;) { // 255-0
        heap->arr[i] = malloc(sizeof(Node));
        heap->arr[i]->ch = i;
        heap->arr[i]->freq = 0;
        heap->arr[i]->is_leaf = true;
        heap->arr[i]->left = NULL;
        heap->arr[i]->right = NULL;
    }
    return heap;
}
