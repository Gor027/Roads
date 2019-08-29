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
    char *key_str = key;
    char *keycopy = malloc(strlen(key_str) + 1);
    strcpy(keycopy, key_str);
    item->key = keycopy;
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

void rehash(hmap *hm) {
    hm_item *next;

    hm_item **current = hm->buckets;
    uint64_t s = hm->size;
    uint64_t size = s * 2;

    hm_item **buckets = calloc(size, sizeof(hm_item *));

    for (uint64_t i = 0; i < s; i++) {
        for (hm_item *item = current[i]; item != NULL; item = next) {
            uint64_t index = hash(item->key, size);
            next = item->next;
            item->next = buckets[index];
            buckets[index] = item;
        }
    }

    free(hm->buckets);
    hm->buckets = buckets;
    hm->size = size;
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

    // Rehash when current size is at least 75% of the total size
    if (++hm->count >= hm->size * 3 / 4)
        rehash(hm);
}

void free_hmap(hmap *hm) {
    for (uint64_t i = 0; i < hm->size; i++) {
        for (hm_item *item = hm->buckets[i]; item != NULL;) {
            hm_item *next = item->next;
            free(item->key);
            free(item);
            item = next;
        }
    }

    free(hm->buckets);
    free(hm);
}