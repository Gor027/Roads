/** @file
 * Class interface storing the Heap structure
 *
 * @author Gor Stepanyan <gs404865@mimuw.edu.pl>
 * @copyright Gor Stepanyan
 * @date 29.08.2019
 */

#ifndef GRAPHS_HEAP_H
#define GRAPHS_HEAP_H

#include <stdint.h>

/**
 * @brief Structure storing min-heap node.
 */
typedef struct MinHeapNode {
    uint64_t id; /**< Id for each node to extract min. */
    uint64_t distance; /**< Stores distance of the min-heap node. */
    int year; /**< Stores year of the min-heap node. */
} HeapNode;

/**
 * @brief Structure for min-heap array implementation.
 */
typedef struct MinHeap {
    uint64_t size; /**< Stores the current size of the min-heap. */
    uint64_t capacity; /**< Stores the total capacity of the min-heap. */
    uint64_t *pos; /**< Stores the positions of the nodes. */
    HeapNode **array; /**< Stores the nodes for array implementation. */
} Heap;

/**
 * @brief Creates a heap node.
 * Creates a heap node with given id, distance and year.
 * @param[in] id       - id of the created node;
 * @param[in] distance - distance of the created node;
 * @param[in] year     - year of the created node.
 * @return a pointer on the created node, or NULL if malloc failed.
 */
HeapNode *HeapNode_create(uint64_t id, uint64_t distance, int year);

/**
 *  @brief Creates heap for implementing dijkstra algorithm.
 * @param[in] capacity - capacity of the heap(NUmber of vertices).
 * @return a pointer on the created heap, or NULL if malloc failed.
 */
Heap *Heap_create(uint64_t capacity);

/**
 * @brief Decrease key is used for dijkstra algorithm to decrease
 * the value of the root node.
 * @param[in] heap     - pointer of the heap;
 * @param[in] id       - id of the node to be decreased;
 * @param[in] distance - weight of the node to be decreased;
 * @param[in] year     - year of the node to be decreased.
 */
void decreaseKey(Heap *heap, uint64_t id, uint64_t distance, int year);

/**
 * @brief Checks whether heap is empty.
 * @param[in] heap - pointer on the heap.
 * @return @p true if the heap is empty, @p false if there is
 * at least one node in the heap.
 */
bool isEmpty(Heap *heap);

/**
 * @brief Extracts heap node with min distance and oldest year.
 * If heap is not empty then extracts the node with min distance
 * and with the oldest year.
 * @param[in, out] heap - pointer on the heap.
 * @return pointer on the heap node to be extracted or NULL if
 * heap is empty.
 */
HeapNode *extract_Min(Heap *heap);

/**
 * @brief Checks whether node with given id is in heap.
 * @param[in] heap - pointer on the heap;
 * @param[in] v    - id of the node.
 * @return @p true if node with id v is in heap, @p false otherwise.
 */
bool isInHeap(Heap *heap, uint64_t v);

/**
 * @brief Frees heap to prevent memory leaks.
 * @param[in, out] heap - pointer on the heap.
 */
void free_Heap(Heap *heap);

#endif //GRAPHS_HEAP_H
