#ifndef HASHMAP_HASHMAP_H
#define HASHMAP_HASHMAP_H

#include <stdint.h>

typedef struct HashMap_item hm_item;
typedef struct HashMap hmap;

hmap *create_hmap(uint64_t);

void *search_hmap(hmap *, char *);

void set_hmap(hmap *, void *, void *);

//void delete_hmap(hmap *, void *);

void free_hmap(hmap *);

#endif //HASHMAP_HASHMAP_H
