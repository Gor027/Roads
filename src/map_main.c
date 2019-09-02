#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "Commands.h"

int main() {
    Map *map = newMap();

    if (map == NULL)
        exit(1);

    execCommand(map);
    deleteMap(map);
}