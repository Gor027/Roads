#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Commands.h"

#define COMMANDS_SIZE 7
#define COMMAND_SIZE 20
#define CITYNAME_SIZE 20
#define LENGTH_SIZE 21
#define YEAR_SIZE 11
#define ROUTEID_SIZE 4

enum cmdEnum {
    ADD, REPAIR, GETROUTE, NEWROUTE, EXTEND, REMOVE, REMOVEROUTE
};

char *getCommand(int *whiteChar) {
    int input;
    int currentSize = 0;
    char *command = (char *) malloc(COMMAND_SIZE * sizeof(char));

    if (command == NULL)
        return NULL;

    while ((input = getchar()) != ';') {
        if (input == '\n' || input == EOF) {
            *whiteChar = input;
            free(command);
            return NULL;
        }

        command[currentSize] = input;
        currentSize++;
    }

    command[currentSize] = '\0';
    *whiteChar = ';';

    return command;
}

int getRouteId(int *whiteChar) {
    int input;
    int currentSize = 0;
    char *id = (char *) malloc(ROUTEID_SIZE * sizeof(char));

    while ((input = getchar()) != '\n' || input != ';') {
        if (input == EOF) {
            free(id);
            *whiteChar = EOF;
            return 0;
        }

        id[currentSize] = input;
        currentSize++;

        if (currentSize == 4) {
            free(id);
            int n;
            while ((n = getchar()) != '\n') {
                if (n == EOF) {
                    *whiteChar = EOF;
                    return 0;
                }
            }

            return 0;
        }
    }

    *whiteChar = input;
    id[currentSize] = '\0';
    int idInt = strtol(id, NULL, 10);
    free(id);

    if (idInt > 0 && idInt < 1000)
        return idInt;
    else
        return 0;
}

char *getCity1(int *whiteChar) {
    int input;
    int currentSize = 0;
    int size = CITYNAME_SIZE;
    char *cityName = (char *) malloc(size * sizeof(char));

    while ((input = getchar()) != ';') {
        if (input == EOF || input == '\n') {
            *whiteChar = input;
            free(cityName);
            return NULL;
        }

        if (currentSize == size) {
            size = 2 * currentSize;
            cityName = (char *) realloc(cityName, size * sizeof(char));
        }

        cityName[currentSize] = input;
        currentSize++;
    }

    cityName[currentSize] = '\0';
    *whiteChar = input;

    return cityName;
}

char *getCity2(int *whiteChar) {
    int input;
    int currentSize = 0;
    int size = CITYNAME_SIZE;
    char *cityName = (char *) malloc(size * sizeof(char));

    while ((input = getchar()) != ';' || input != '\n') {
        if (input == EOF) {
            *whiteChar = input;
            free(cityName);
            return NULL;
        }

        if (currentSize == size) {
            size = 2 * currentSize;
            cityName = (char *) realloc(cityName, size * sizeof(char));
        }

        cityName[currentSize] = input;
        currentSize++;
    }

    *whiteChar = input;
    cityName[currentSize] = '\0';
    return cityName;
}

uint64_t getLength(int *whiteChar) {
    int input;
    int currentSize = 0;
    char *length = (char *) malloc(LENGTH_SIZE * sizeof(char));

    while ((input = getchar()) != ';') {
        if (input == EOF || input == '\n') {
            *whiteChar = input;
            free(length);
            return 0;
        }

        length[currentSize] = input;
        currentSize++;

        if (currentSize == 21) {
            free(length);
            int n;
            while ((n = getchar()) != '\n') {
                if (n == EOF) {
                    *whiteChar = EOF;
                    return 0;
                }
            }
            return 0;
        }
    }

    *whiteChar = input;
    length[currentSize] = '\0';
    int lengthInt = strtol(length, NULL, 10);
    free(length);

    if (lengthInt > 0)
        return lengthInt;
    else
        return 0;
}

int getYear(int *whiteChar) {
    int input;
    int currentSize = 0;
    char *year = (char *) malloc(YEAR_SIZE * sizeof(char));

    while (((input = getchar()) != ';') && input != '\n') {
        if (input == EOF) {
            *whiteChar = input;
            free(year);
            return 0;
        }

        year[currentSize] = input;
        currentSize++;

        if (currentSize == 11) {
            free(year);
            int n;
            while ((n = getchar()) != '\n') {
                if (n == EOF) {
                    *whiteChar = EOF;
                    return 0;
                }
            }
            return 0;
        }
    }

    *whiteChar = input;
    year[currentSize] = '\0';
    int lengthInt = strtol(year, NULL, 10);
    free(year);

    if (lengthInt != 0)
        return lengthInt;
    else
        return 0;
}

void UnusedManual_free(Route *route) {
    if (route == NULL)
        return;

    RouteNode *current = route->routeNodeList->head;

    while (current != NULL) {
        RouteNode *next = current->next;
        free(current);
        current = next;
    }

    free(route->routeNodeList);
    free(route);
}

bool extendManualRoute(Map *map, Route *route) {
    int whiteChar;
    do {
        uint64_t length = getLength(&whiteChar);

        if (whiteChar == EOF || whiteChar == '\n')
            return false;

        int year = getYear(&whiteChar);

        if (whiteChar == EOF || whiteChar == '\n')
            return false;

        char *city = getCity2(&whiteChar);

        if (whiteChar == EOF) {
            free(city);
            return false;
        }

        RouteNode *prev = route->routeNodeList->tail;
        City *city2 = search_hmap(map->nameToCity, city);

        if (city2 == NULL) {
            addRoad(map, prev->city->cityName, city, length, year);
        }

        city2 = search_hmap(map->nameToCity, city);
        Road *roadBetween = areConnected(prev->city, city2);

        if (roadBetween != NULL && roadBetween->builtYear <= year) {
            RouteNode *routeNode = RouteNode_create(city2, length, year);
            prev->next = routeNode;
            route->routeNodeList->tail = routeNode;
        } else {
            free(city);
            return false;
        }

    } while (whiteChar == ';');

    return true;
}

void addRouteManual(Map *map, int routeNumber, int *whiteChar, int *line) {
    if (map->routes[routeNumber] != NULL) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *firstCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    uint64_t length = getLength(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    int year = getYear(whiteChar);

    if (*whiteChar == EOF || *whiteChar != ';') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *secondCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    City *city1 = search_hmap(map->nameToCity, firstCity);
    City *city2 = search_hmap(map->nameToCity, secondCity);

    if (city1 == NULL || city2 == NULL)
        addRoad(map, firstCity, secondCity, length, year);

    Road *roadbetween = areConnected(city1, city2);

    if (roadbetween != NULL && year >= roadbetween->builtYear) {
        Route *route = Route_create();
        RouteNode *head = RouteNode_create(city1, length, year);
        RouteNode *next = RouteNode_create(city2, 0, 0);
        head->next = next;
        route->routeNodeList->head = head;
        route->routeNodeList->tail = next;
        if (extendManualRoute(map, route)) {
            map->routes[routeNumber] = route;
        } else {
            UnusedManual_free(route);
            free(firstCity);
            free(secondCity);
            fprintf(stderr, "ERROR %d\n", *line);
            return;
        }
    } else {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
    printf("Bro\n");
}

void addRoadCommand(Map *map, int *whiteChar, int *line) {
    char *firstCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *secondCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    uint64_t length = getLength(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    int year = getYear(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = addRoad(map, firstCity, secondCity, length, year);

    if (isRight == false) {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void repairRoadCommand(Map *map, int *whiteChar, int *line) {
    char *firstCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *secondCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    int repairYear = getYear(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = repairRoad(map, firstCity, secondCity, repairYear);

    if (isRight == false) {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void getRouteDescrCommand(Map *map, int *whiteChar, int *line) {
    int id = getRouteId(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n' || id == 0) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    const char *result = getRouteDescription(map, id);

    if (result == NULL) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    } else {
        fprintf(stdout, "%s", result);
    }
}

void newRouteCommand(Map *map, int *whiteChar, int *line) {
    int id = getRouteId(whiteChar);

    if (*whiteChar == EOF || *whiteChar != ';' || id == 0) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *firstCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar != ';') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *secondCity = getCity2(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = newRoute(map, id, firstCity, secondCity);

    if (isRight == false) {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void extendRouteCommand(Map *map, int *whiteChar, int *line) {
    int id = getRouteId(whiteChar);

    if (*whiteChar == EOF || *whiteChar != ';' || id == 0) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *extendTo = getCity2(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n') {
        free(extendTo);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = extendRoute(map, id, extendTo);

    if (isRight == false) {
        free(extendTo);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void removeRoadCommand(Map *map, int *whiteChar, int *line) {
    char *firstCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    char *secondCity = getCity1(whiteChar);

    if (*whiteChar == EOF || *whiteChar == '\n') {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = removeRoad(map, firstCity, secondCity);

    if (isRight == false) {
        free(firstCity);
        free(secondCity);
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void removeRouteCommand(Map *map, int *whiteChar, int *line) {
    int id = getRouteId(whiteChar);

    if (*whiteChar == EOF || *whiteChar != '\n' || id == 0) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }

    bool isRight = removeRoute(map, id);

    if (isRight == false) {
        fprintf(stderr, "ERROR %d\n", *line);
        return;
    }
}

void doOperation(Map *map, int index, int *whiteChar, int *line) {
    switch (index) {
        case ADD:
            addRoadCommand(map, whiteChar, line);
            break;
        case REPAIR:
            repairRoadCommand(map, whiteChar, line);
            break;
        case GETROUTE:
            getRouteDescrCommand(map, whiteChar, line);
            break;
        case NEWROUTE:
            newRouteCommand(map, whiteChar, line);
            break;
        case EXTEND:
            extendRouteCommand(map, whiteChar, line);
            break;
        case REMOVE:
            removeRoadCommand(map, whiteChar, line);
            break;
        case REMOVEROUTE:
            removeRouteCommand(map, whiteChar, line);
            break;
        default: {
            fprintf(stderr, "ERROR %d\n", *line);
            break;
        }
    }
}

void execCommand(Map *map) {
    char *commands[COMMANDS_SIZE] = {"addRoad", "repairRoad", "getRouteDescription", "newRoute", "extendRoute",
                                     "removeRoad", "removeRoute"};
    int input;
    int line = 1;

    while ((input = getchar()) != EOF) {
        if (input == '#') {
            do {
                // Get whole line and ignore
                input = getchar();
                if (input == EOF) {
                    fprintf(stderr, "ERROR %d\n", line);
                    return;
                }
            } while (input != '\n');

            line++;
            continue;
        }

        if (input != '\n') {
            int index = -1;
            int whiteChar;

            ungetc(input, stdin);
            char *command = getCommand(&whiteChar);

            if (command == NULL && whiteChar == '\n') {
                fprintf(stderr, "ERROR %d\n", line);
                line++;
            }

            if (command == NULL && whiteChar == EOF) {
                fprintf(stderr, "ERROR %d\n", line);
                return;
            }

            int routeNumber = 0;

            if (command != NULL) {
                routeNumber = strtol(command, NULL, 10);
            }

            if (routeNumber > 0 && routeNumber < 1000) {
                addRouteManual(map, routeNumber, &whiteChar, &line);
                line++;
            } else if (command != NULL) {
                for (int i = 0; i < COMMANDS_SIZE; i++) {
                    if (strcmp(command, commands[i]) == 0) {
                        index = i;
                        break;
                    }
                }

                doOperation(map, index, &whiteChar, &line);
                line++;
            }

            if (whiteChar != '\n') {
                do {
                    input = getchar();

                    if (input == EOF) {
                        free(command);
                        return;
                    }
                } while (input != '\n');
            }

            free(command);
        }
    }
}
