#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "map.h"
#include "HashMap.h"
#include "Heap.h"
#include "CityRoad.h"

#define HASH_MAP_SIZE 1000
#define BUFF_SIZE 10000
#define ROUTES_SIZE 1000
#define MAP_CITIES_SIZE 1000

struct Map {
    uint64_t nCities;
    uint64_t citiesSize;
    City **cities;
    Route *routes[ROUTES_SIZE];
    hmap *nameToCity;
};

Map *newMap(void) {
    Map *map = (Map *) malloc(sizeof(Map));

    if (map == NULL)
        return NULL; /* In case when allocation failed */

    map->nCities = 0;
    map->citiesSize = MAP_CITIES_SIZE;
    map->cities = (City **) malloc(MAP_CITIES_SIZE * sizeof(City *));
    for (uint64_t i = 0; i < ROUTES_SIZE; i++) {
        map->routes[i] = NULL;
    }

    map->nameToCity = create_hmap(HASH_MAP_SIZE);

    return map;
}

void deleteMap(Map *map) {
    for (uint64_t i = 0; i < map->nCities; i++) {
        Road *road = map->cities[i]->roadsList->head;
        while (road != NULL) {
            Road *nextRoad = road->nextRoadOfCity;
            free(road);
            road = nextRoad;
        }

        free(map->cities[i]->roadsList);
        free(map->cities[i]);
    }

    for (uint64_t i = 0; i < ROUTES_SIZE; i++) {
        if (map->routes[i] != NULL) {
            RouteNode *routeNode = map->routes[i]->routeNodeList->head;
            while (routeNode != NULL) {
                RouteNode *next = routeNode->next;
                free(routeNode);
                routeNode = next;
            }
            free(map->routes[i]->routeNodeList);
            free(map->routes[i]);
        }
    }

    // Free hash map
    free_hmap(map->nameToCity);
    free(map->cities);
    free(map);
}

static inline void addCityOnMap(Map *map, City *city) {
    // 1. Add in cities
    map->cities[map->nCities] = city;
    // Count of the cities in the array map->cities
    map->nCities++;

    // Resize the cities
    if (map->nCities == map->citiesSize - 1) {
        map->citiesSize = 2 * map->citiesSize;
        map->cities = (City **) realloc(map->cities, map->citiesSize * sizeof(City *));
    }

    // 2. Add in hashMap
    set_hmap(map->nameToCity, (void *) city->cityName, city);
}

static inline City *getFromHashMap(Map *map, const char *city) {
    City *firstCity = (City *) search_hmap(map->nameToCity, (void *) city);

    // Create the city if it does not exist
    if (firstCity == NULL) {
        firstCity = City_create(city, map->nCities);

        // If allocation succeeds
        if (firstCity != NULL)
            addCityOnMap(map, firstCity);
        else
            return NULL;
    }

    return firstCity;
}

static inline bool checkCityName(const char *name) {
    uint64_t i = 0;
    while (name[i] == '\0') {
        if (name[i] >= 0 && name[i] <= 31)
            return false;
        i++;
    }

    return true;
}

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear) {
    // If the city names are the same or the builtYear is wrong
    if (!checkCityName(city1) || !checkCityName(city2) || strcmp(city1, city2) == 0 || builtYear == 0 || length <= 0) {
        return false;
    }

    // Get city1 and city2 from hashMap
    // If city does not exist, add it on the map
    City *firstCity = getFromHashMap(map, city1);
    City *secondCity = getFromHashMap(map, city2);

    // When road between cities already exist
    if (areConnected(firstCity, secondCity) != NULL)
        return false;

    // Create road for city1 with adjCity city2
    if (firstCity->roadsList->head == NULL) {
        Road *road = Road_create(secondCity, length, builtYear);
        firstCity->roadsList->head = road;
        firstCity->roadsList->tail = road;
    } else {
        Road *tail = (Road *) firstCity->roadsList->tail;
        tail->nextRoadOfCity = Road_create(secondCity, length, builtYear);
        firstCity->roadsList->tail = tail->nextRoadOfCity;
    }

    // Create road for city2 with adjCity city1
    if (secondCity->roadsList->head == NULL) {
        Road *road = Road_create(firstCity, length, builtYear);
        secondCity->roadsList->head = road;
        secondCity->roadsList->tail = road;
    } else {
        Road *tail = (Road *) secondCity->roadsList->tail;
        tail->nextRoadOfCity = Road_create(firstCity, length, builtYear);
        secondCity->roadsList->tail = tail->nextRoadOfCity;
    }

    return true;
}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear) {
    if (!checkCityName(city1) || !checkCityName(city2))
        return false;

    City *firstCity = search_hmap(map->nameToCity, (void *) city1);
    City *secondCity = search_hmap(map->nameToCity, (void *) city2);
    Road *road1 = areConnected(firstCity, secondCity);
    Road *road2 = areConnected(secondCity, firstCity);

    if (!firstCity || !secondCity || road1 == NULL || road2 == NULL || repairYear == 0 || road1->builtYear > repairYear)
        return false;

    road1->builtYear = repairYear;
    road2->builtYear = repairYear;

    return true;
}

static inline void Route_getUtil(City **parent, City *destination, Route **route) {
    if (parent[destination->id] == NULL)
        return;

    Route_getUtil(parent, parent[destination->id], route);

    RouteNode *lastRouteNode = (*route)->routeNodeList->tail;
    Road *road = areConnected(lastRouteNode->city, destination);
    lastRouteNode->length = road->length;
    lastRouteNode->age = road->builtYear;
    lastRouteNode->next = RouteNode_create(destination, 0, 0);
    (*route)->routeNodeList->tail = lastRouteNode->next;
}

static inline Route *Route_get(City **parent, City *destination, City *srcCity) {
    Route *route = Route_create();
    RouteNode *routeNode = RouteNode_create(srcCity, 0, 0);
    route->routeNodeList->head = routeNode;
    route->routeNodeList->tail = routeNode;
    Route_getUtil(parent, destination, &route);

    return route;
}

static inline int compareMin(int year1, int year2) {
    if (year1 < 0 && year2 < 0)
        return year1 >= year2 ? year1 : year2;
    else
        return year1 <= year2 ? year1 : year2;
}

Route *dijkstra(Map *map, City *src, City *destination) {
    uint64_t vertices = map->nCities;// Get the number of vertices in graph
    uint64_t dist[vertices];      // dist values used to pick minimum weight edge in cut

    City *parent[vertices];

    Heap *heap = Heap_create(vertices);

    for (uint64_t v = 0; v < vertices; ++v) {
        parent[v] = NULL;
        dist[v] = UINT64_MAX;
        heap->array[v] = HeapNode_create(v, dist[v]);
        heap->pos[v] = v;
    }

    heap->array[src->id]->id = src->id;
    heap->array[src->id]->distance = dist[src->id];

    heap->pos[src->id] = src->id;
    dist[src->id] = 0;
    heap->array[src->id]->year = INT32_MAX;
    src->visited = true;
    decreaseKey(heap, src->id, dist[src->id], 0);

    heap->size = vertices;

    while (!isEmpty(heap)) {
        // Extract the vertex with minimum distance value
        HeapNode *heapNode = extract_Min(heap);
        uint64_t u = heapNode->id;

        Road *pCrawl = map->cities[u]->roadsList->head;
        while (pCrawl != NULL) {
            uint64_t v = pCrawl->adjCity->id;

            if (isInHeap(heap, v) && pCrawl->adjCity->visited == false && dist[u] != UINT64_MAX &&
                pCrawl->length + dist[u] < dist[v]) {
                dist[v] = dist[u] + pCrawl->length;
                parent[v] = map->cities[u];

                decreaseKey(heap, v, dist[v], compareMin(heapNode->year, pCrawl->builtYear));
            }
            pCrawl = pCrawl->nextRoadOfCity;
        }
        free(heapNode);
    }

    free_Heap(heap);
    return Route_get(parent, destination, src);
}

bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2) {
    if (!checkCityName(city1) || !checkCityName(city2))
        return false;

    City *srcCity = search_hmap(map->nameToCity, (void *) city1);
    City *destCity = search_hmap(map->nameToCity, (void *) city2);

    if (!srcCity || !destCity || strcmp(city1, city2) == 0 || routeId < 1 || routeId > 999 ||
        map->routes[routeId] != NULL)
        return false;

    Route *shortestPath = dijkstra(map, srcCity, destCity);

    RouteNode *lastRouteNode = shortestPath->routeNodeList->tail;
    if (lastRouteNode->city != destCity) {
        return false;
    }

    map->routes[routeId] = shortestPath;

    return true;
}

static inline bool isInRoute(Route *route, City *city) {
    RouteNode *routeNode = route->routeNodeList->head;
    while (routeNode != NULL) {
        if (routeNode->city == city)
            return true;
        routeNode = routeNode->next;
    }

    return false;
}

static inline int getOldest(int year1, int year2) {
    if (year1 < 0 && year2 < 0)
        return year1 >= year2 ? year2 : year1;
    else
        return year1 <= year2 ? year2 : year1;
}

static inline void get_LengthSum_OldestYear(Route *route, uint64_t *lengthSum, int *year) {
    RouteNode *routeNode = route->routeNodeList->head;
    uint64_t sum = 0;
    int oldestYear = 0;

    while (routeNode != NULL) {
        if (oldestYear == 0) {
            oldestYear = routeNode->age;
        } else {
            oldestYear = getOldest(oldestYear, routeNode->age);
        }
        sum += routeNode->length;
        routeNode = routeNode->next;
    }

    *lengthSum = sum;
    *year = oldestYear;
}

static inline void markVisited(Route *route) {
    RouteNode *routeNode = route->routeNodeList->head;
    while (routeNode != NULL) {
        routeNode->city->visited = true;
        routeNode = routeNode->next;
    }
}

static inline void markUnvisited(Route *route) {
    RouteNode *routeNode = route->routeNodeList->head;
    while (routeNode != NULL) {
        routeNode->city->visited = false;
        routeNode = routeNode->next;
    }
}

bool extendRoute(Map *map, unsigned routeId, const char *city) {
    if (!checkCityName(city))
        return false;

    City *extendTo = search_hmap(map->nameToCity, (void *) city);
    if (routeId < 1 || routeId > 999 || map->routes[routeId] == NULL || extendTo == NULL)
        return false;

    Route *route = map->routes[routeId];
    if (isInRoute(route, extendTo))
        return false;

    // Mark all cities of the route visited
    markVisited(route);

    RouteNode *lastRouteNode = route->routeNodeList->tail;
    Route *utilRoute = dijkstra(map, lastRouteNode->city, extendTo);

    // Unmark all cities of the route to be unvisited
    markUnvisited(route);

    RouteNode *start = route->routeNodeList->head;
    while (start->next->next != NULL) {
        start = start->next;
    }

    free(start->next);
    start->next = utilRoute->routeNodeList->head;
    route->routeNodeList->tail = utilRoute->routeNodeList->tail;

    uint64_t minLengthTail;
    int oldestYearTail;
    get_LengthSum_OldestYear(route, &minLengthTail, &oldestYearTail);

    // Extend from start
    markVisited(route);

    RouteNode *routeHead = route->routeNodeList->head;
    Route *routeFromStart = dijkstra(map, routeHead->city, extendTo);
    uint64_t minLengthHead;
    int oldestYearHead;
    get_LengthSum_OldestYear(routeFromStart, &minLengthHead, &oldestYearHead);

    oldestYearHead = getOldest(oldestYearHead, oldestYearTail);
    if ((minLengthHead != 0 && minLengthHead < minLengthTail) ||
        (minLengthHead == minLengthTail && oldestYearHead != oldestYearTail)) {
        map->routes[routeId] = routeFromStart;
        markUnvisited(route);

        RouteNode *currentRouteNode = route->routeNodeList->head;
        while (currentRouteNode != NULL) {
            RouteNode *next = currentRouteNode->next;
            free(currentRouteNode);
            currentRouteNode = next;
        }
        free(route->routeNodeList);
        free(route);
    } else {
        RouteNode *currentRouteNode = routeFromStart->routeNodeList->head;
        while (currentRouteNode != NULL) {
            RouteNode *next = currentRouteNode->next;
            free(currentRouteNode);
            currentRouteNode = next;
        }
        free(routeFromStart->routeNodeList);
        free(routeFromStart);
    }

    free(utilRoute->routeNodeList);
    free(utilRoute);

    return true;
}

static inline void removeRoadInAdjList(City *city1, City *city2) {
    Road *fakeHead = Road_create(NULL, 0, 0);
    fakeHead->nextRoadOfCity = city1->roadsList->head;
    Road *road = fakeHead;
    while (road->nextRoadOfCity != NULL) {
        if (road->nextRoadOfCity->adjCity == city2) {
            Road *next = road->nextRoadOfCity->nextRoadOfCity;
            free(road->nextRoadOfCity);
            road->nextRoadOfCity = next;
            return;
        }
        road = road->nextRoadOfCity;
    }
}

static inline bool removeInRoute(Map *map, unsigned routeId, Route *route, City *city1, City *city2) {
    RouteNode *fakeNode = RouteNode_create(NULL, 0, 0);
    fakeNode->next = route->routeNodeList->head;
    RouteNode *routeNode = fakeNode;
    while (routeNode->next != NULL) {
        if (routeNode->next->city == city1 && routeNode->next->next->city == city2) {
            RouteNode *tail = routeNode->next->next;
            Road *road = areConnected(city1, city2);

            removeRoadInAdjList(city1, city2);
            removeRoadInAdjList(city2, city2);
            Route *newRoute = dijkstra(map, city1, city2);

            RouteNode *lastRouteNode = newRoute->routeNodeList->tail;
            if (lastRouteNode->city == city2) {
                free(routeNode->next);
                routeNode->next = newRoute->routeNodeList->head;
                RouteNode *lastNodeNewRoute = newRoute->routeNodeList->tail;
                lastNodeNewRoute->next = tail->next;
                free(tail);
                route->routeNodeList->head = fakeNode->next;
                route->routeNodeList->tail = newRoute->routeNodeList->tail;
                map->routes[routeId] = route;
                free(fakeNode);

                return true;
            } else {
                // Free allocated memory.
                RouteNode *current = newRoute->routeNodeList->head;
                while (current != NULL) {
                    RouteNode *next = current->next;
                    free(current);
                    current = next;
                }
                free(newRoute->routeNodeList);
                free(newRoute);
                free(fakeNode);

                // Add roads between cities
                addRoad(map, city1->cityName, city2->cityName, road->length, road->builtYear);
                return false;
            }
        }
        routeNode = routeNode->next;
    }

    free(fakeNode);
    return true;
}

bool removeRoad(Map *map, const char *city1, const char *city2) {
    if (!checkCityName(city1) || !checkCityName(city2))
        return false;

    City *firstCity = search_hmap(map->nameToCity, (void *) city1);
    City *secondCity = search_hmap(map->nameToCity, (void *) city2);
    Road *road1 = areConnected(firstCity, secondCity);

    if (!firstCity || !secondCity || !road1 || strcmp(city1, city2) == 0)
        return false;

    for (int i = 1; i < ROUTES_SIZE; ++i) {
        if (map->routes[i] != NULL) {
            if (!removeInRoute(map, i, map->routes[i], firstCity, secondCity))
                return false;
        }
    }

    removeRoadInAdjList(firstCity, secondCity);
    removeRoadInAdjList(secondCity, firstCity);

    return true;
}

char const *getRouteDescription(Map *map, unsigned routeId) {
    Route *route = map->routes[routeId];

    if (route == NULL) {
        char *c = malloc(1);
        c[0] = '\0';
        return c;
    }

    RouteNode *routeNode = route->routeNodeList->head;
    char *b = (char *) malloc(BUFF_SIZE * sizeof(char));

    if (b == NULL)
        return NULL;

    sprintf(b, "%u", routeId);
    int b_size = strlen(b);

    while (routeNode->next != NULL) {
        sprintf(b_size + b, ";%s;%u;%d", routeNode->city->cityName, routeNode->length, routeNode->age);
        b_size = strlen(b);
        routeNode = routeNode->next;
    }

    sprintf(b_size + b, ";%s", routeNode->city->cityName);

    return b;
}