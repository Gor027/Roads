#include <stdio.h>
#include <stdlib.h>
#include "HashMap.h"
#include <string.h>

struct HashMap_item {
    hm_item *next;
    void *key;
    void *value;
};

struct HashMap {
    hm_item **buckets;
    uint64_t size;
    uint64_t count;
};

static hm_item *create_item(void *key, void *value) {
    hm_item *item = (hm_item *) malloc(sizeof(hm_item));
    item->key = key;
    item->value = value;

    return item;
}

static uint64_t hash(const char *ptr, uint64_t size) {
    uint64_t hash = 5381;
    uint64_t c;

    while ((c = *ptr++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;
}

hmap *create_hmap(uint64_t size) {
    hmap *hm = (hmap *) malloc(sizeof(hmap));
    hm->buckets = calloc(size, sizeof(hm_item *));
    hm->size = size;
    hm->count = 0;

    return hm;
}

void *search_hmap(hmap *hm, char *key) {
    int index = hash(key, hm->size);

    hm_item *item = hm->buckets[index];
    while (item != NULL) {
        char *item_key = (char *) item->key;
        int kmp = strcmp(key, item_key);
        if (kmp == 0) {
            return item->value;
        }
        item = item->next;
    }


    return NULL;
}

void set_hmap(hmap *hm, void *key, void *value) {
    hm_item *item;
    uint64_t index = hash(key, hm->size);

    hm_item **p = &(hm->buckets[index]);

    for (item = *p; item != NULL; item = item->next) {
        char *item_key = (char *) item->key;
        int kmp = strcmp(key, item_key);
        if (kmp == 0) {            // key already exists
            item->value = value;
            return;
        }
    }

    item = *p;
    (*p) = create_item(key, value);
    (*p)->next = item;
}


/* This function may be useful for future development */
//void delete_hmap(hmap *hm, void *key) {
//    uint64_t index = hash(key, hm->size);
//    hm_item *item = hm->buckets[index];
//
//    if (item == NULL)
//        return;
//
//    if (item->next == NULL) {
//        free(item);
//        hm->buckets[index] = NULL;
//        hm->count--;
//        return;
//    }
//
//    while (item != NULL) {
//        hm_item *next = item->next;
//
//        if (next != NULL && next->key == key) {
//            item->next = next->next;
//            hm->count--;
//            free(next);
//            return;
//        }
//
//        item = next;
//    }
//}

void free_hmap(hmap *hm) {
    for (uint64_t i = 0; i < hm->size; i++) {
        for (hm_item *item = hm->buckets[i]; item != NULL;) {
            hm_item *next = item->next;
            free(item);
            item = next;
        }
    }

    free(hm->buckets);
    free(hm);
}