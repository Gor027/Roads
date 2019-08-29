/** @file
 * Class interface storing HashMap for cityName and reference to a City.
 *
 * @author Gor Stepanyan <gs404865@mimuw.edu.pl>
 * @copyright Gor Stepanyan
 * @date 29.08.2019
 */

#ifndef HASHMAP_HASHMAP_H
#define HASHMAP_HASHMAP_H

#include <stdint.h>

/**
 * Structure storing HashMap item.
 */
typedef struct HashMap_item hm_item;

/**
 * Structure storing HashMap/
 */
typedef struct HashMap hmap;

/**
 * @brief Creates HasMap.
 * @param[in] size - size of the array for storing hm_items .
 * @return pointer on the HashMap, or NULL if memory allocation failed.
 */
hmap *create_hmap(uint64_t size);

/**
 * @brief Searches in HashMap for given key.
 * @param[in] hm  - reference on HashMap;
 * @param[in] key - key is the name of the city.
 * @return pointer on the value if it exists for given key, otherwise NULL.
 */
void *search_hmap(hmap *hm, char *key);

/**
 * @brief Set key-value pair in HashMap.
 * If the size of the buckets in HashMap is more than the 75% of the
 * HashMap size, then does a rehash.
 * @param[in, out] hm - reference on HashMap;
 * @param[in] key     - key is the name of the city;
 * @param[in] value   - value is the reference on the City.
 */
void set_hmap(hmap *hm, void *key, void *value);

/**
 * @brief Frees the HashMap.
 * Frees to prevent memory leaks.
 * @param[in,out] hm - reference on the HashMap.
 */
void free_hmap(hmap *hm);

#endif //HASHMAP_HASHMAP_H
