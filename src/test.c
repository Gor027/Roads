#include "HashMap.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
    struct HashMap *m = create_hmap(100);
    char *k1 = "bwoxsfcals";
    char *k2 = "bwoxsfcals";
    set_hmap(m, k1, "bikt");
    char *ans = search_hmap(m, k2);
    assert(strcmp(ans, "bikt") == 0);
    for (int i = 10000000; i < 20000000; ++i) {
        char key[10];
        sprintf(key, "%d", i);
        set_hmap(m, key, "jpu");
    }

    ans = search_hmap(m, k2);
    assert(strcmp(ans, "bikt") == 0);
    for (int i = 10000000; i < 20000000; ++i) {
        char key[10];
        sprintf(key, "%d", i);
        char *ans2 = (char *) search_hmap(m, key);
        printf("%s|\n", ans2);
        assert(strcmp("jpu", ans2) == 0);
    }

}