#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "Heap.h"

HeapNode *HeapNode_create(uint64_t id, uint64_t distance) {
    HeapNode *heapNode = (HeapNode *) malloc(sizeof(HeapNode));
    heapNode->id = id;
    heapNode->distance = distance;
    heapNode->year = 0;

    return heapNode;
}

Heap *Heap_create(uint64_t capacity) {
    Heap *heap = (Heap *) malloc(sizeof(Heap));
    heap->pos = (uint64_t *) malloc(capacity * sizeof(uint64_t));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (HeapNode **) malloc(capacity * sizeof(HeapNode *));

    return heap;
}

static inline void swap_HeapNode(HeapNode **a, HeapNode **b) {
    HeapNode *t = *a;
    *a = *b;
    *b = t;
}

static inline bool compareYears(int year1, int year2) {
    if (year1 < 0 && year2 < 0)
        return year1 <= year2;
    else
        return year1 >= year2;
}

static inline void minHeapify(Heap *heap, uint64_t idx) {
    uint64_t smallest = idx;
    uint64_t left = 2 * idx + 1;
    uint64_t right = 2 * idx + 2;

    if (left < heap->size &&
        ((heap->array[left]->distance < heap->array[smallest]->distance) ||
         ((heap->array[left]->distance == heap->array[right]->distance) &&
          compareYears(heap->array[left]->year, heap->array[right]->year))))
        smallest = left;

    if (right < heap->size &&
        ((heap->array[right]->distance < heap->array[smallest]->distance) ||
         ((heap->array[right]->distance == heap->array[left]->distance) &&
          compareYears(heap->array[right]->year, heap->array[right]->year))))
        smallest = right;

    if (smallest != idx) {
        // The nodes to be swapped in min heap
        HeapNode *smallestNode = heap->array[smallest];
        HeapNode *idxNode = heap->array[idx];

        // Swap positions
        heap->pos[smallestNode->id] = idx;
        heap->pos[idxNode->id] = smallest;

        // Swap nodes
        swap_HeapNode(&heap->array[smallest], &heap->array[idx]);

        minHeapify(heap, smallest);
    }
}

bool isEmpty(Heap *heap) {
    return heap->size == 0;
}

HeapNode *extract_Min(Heap *heap) {
    if (isEmpty(heap))
        return NULL;

    // Store the root node
    HeapNode *root = heap->array[0];

    // Replace root node with last node
    HeapNode *lastNode = heap->array[heap->size - 1];
    heap->array[0] = lastNode;

    // Update position of last node
    heap->pos[root->id] = heap->size - 1;
    heap->pos[lastNode->id] = 0;

    // Reduce heap size and heapify root
    --heap->size;
    minHeapify(heap, 0);

    return root;
}

bool isInHeap(Heap *heap, uint64_t v) {
    if (heap->pos[v] < heap->size)
        return true;
    return false;
}

void decreaseKey(Heap *heap, uint64_t id, uint64_t distance, int year) {
    uint64_t i = heap->pos[id];

    // Get the node and update its dist value
    heap->array[i]->distance = distance;
    heap->array[i]->year = year;

    // Travel up while the complete tree is not heapified
    while (i && ((heap->array[i]->distance < heap->array[(i - 1) / 2]->distance) ||
                 (heap->array[i]->distance == heap->array[(i - 1) / 2]->distance &&
                  compareYears(heap->array[i]->year, heap->array[(i - 1) / 2]->year)))) {
        // Swap this node with its parent
        heap->pos[heap->array[i]->id] = (i - 1) / 2;
        heap->pos[heap->array[(i - 1) / 2]->id] = i;
        swap_HeapNode(&heap->array[i], &heap->array[(i - 1) / 2]);

        // move to parent index
        i = (i - 1) / 2;
    }
}

void free_Heap(Heap *heap) {
    free(heap->pos);
    free(heap->array);
    free(heap);
}