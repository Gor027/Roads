#ifndef GRAPHS_HEAP_H
#define GRAPHS_HEAP_H

#include <stdint.h>

/**
 * @brief Structure for min-heap node
 */
typedef struct MinHeapNode {
    uint64_t id;
    uint64_t distance;
    int year;
} HeapNode;

/**
 * @brief Structure for min-heap array implementation
 */
typedef struct MinHeap {
    uint64_t size;
    uint64_t capacity;
    uint64_t *pos;
    HeapNode **array;
} Heap;

/**
 * @brief Creates a heap node
 * @param[in] id       - id of the created node
 * @param[in] distance - weight of the created node
 * @return a pointer on the created node, or NULL if malloc failed
 */
HeapNode *HeapNode_create(uint64_t id, uint64_t distance);

/**
 *  @brief Creates heap for dijkstra algorithm
 * @param[in] capacity - capacity of the heap(NUmber of vertices)
 * @return a pointer on the created heap, or NULL if malloc failed
 */
Heap *Heap_create(uint64_t capacity);

/**
 * @brief decrease key is used for dijkstra algorithm to decrease
 * the value of the root node
 * @param[in] heap     - pointer of the heap
 * @param[in] id       - id of the node to be decreased
 * @param[in] distance - weight of the node to be decreased
 * @param[in] year     - year of the node to be decreased
 */
void decreaseKey(Heap *heap, uint64_t id, uint64_t distance, int year);

/**
 * @brief checks whether heap is empty or no
 * @param[in] heap - pointer on the heap
 * @return @p true if the heap is empty, @p false if there is
 * at least one node in the heap
 */
bool isEmpty(Heap *heap);

/**
 * @brief Extracts heap node with min weight and oldest year
 * @param[in] heap - pointer on the heap
 * @return pointer on the heap node to be extracted
 */
HeapNode *extract_Min(Heap *heap);

/**
 * @brief checks whether node with given id is in heap
 * @param[in] heap - pointer on the heap
 * @param[in] v    - id of the node
 * @return @p true if node with id v is in heap, @p false otherwise
 */
bool isInHeap(Heap *heap, uint64_t v);

/**
 * @brief function to free heap, prevent memory leak
 * @param[in] heap - pointer on the heap
 */
void free_Heap(Heap *heap);

#endif //GRAPHS_HEAP_H
