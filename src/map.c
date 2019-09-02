#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "map.h"
#include "HashMap.h"
#include "Heap.h"
#include "CityRoad.h"

#define HASH_MAP_SIZE 1000
#define BUFF_SIZE 10000
#define MAP_CITIES_SIZE 1000

Map *newMap(void) {
    Map *map = (Map *) malloc(sizeof(Map));

    if (map == NULL)
        return NULL; /* In case when allocation failed */

    map->nCities = 0;
    map->citiesSize = MAP_CITIES_SIZE;
    map->cities = (City **) malloc(MAP_CITIES_SIZE * sizeof(City *));

    if (map->cities == NULL)
        return NULL;

    for (uint64_t i = 0; i < ROUTES_SIZE; i++) {
        map->routes[i] = NULL;
    }

    map->nameToCity = create_hmap(HASH_MAP_SIZE);

    return map;
}

void UnusedRoute_free(Route *route) {
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

static inline void AllRoutes_free(Map *map) {
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
}

void deleteMap(Map *map) {
    for (uint64_t i = 0; i < map->nCities; i++) {
        Road *road = (Road *) map->cities[i]->roadsList->head;

        while (road != NULL) {
            Road *nextRoad = road->nextRoadOfCity;
            free(road);
            road = nextRoad;
        }

        free(map->cities[i]->roadsList);
        free(map->cities[i]->cityName);
        free(map->cities[i]);
    }

    AllRoutes_free(map);

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
        if (firstCity != NULL) {
            addCityOnMap(map, firstCity);
            firstCity->visited = false;
        } else
            return NULL;
    }

    return firstCity;
}

static inline bool checkCityName(const char *name) {
    if (strcmp(name, "") == 0)
        return false;

    uint64_t i = 0;
    while (name[i] != '\0') {
        if ((name[i] >= 0 && name[i] <= 31) || name[i] == 59)
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

    if (firstCity == NULL || secondCity == NULL)
        return false;

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
        tail->nextRoadOfCity->adjCity->visited = false;
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
        tail->nextRoadOfCity->adjCity->visited = false;
        secondCity->roadsList->tail = tail->nextRoadOfCity;
    }

    return true;
}

void repairInRoute(Route *route, City *city1, City *city2, int repairYear) {
    RouteNode *current = route->routeNodeList->head;

    while (current != NULL) {
        if (current->next != NULL && ((current->city == city1 && current->next->city == city2) ||
                                      (current->city == city2 && current->next->city == city1)))
            current->age = repairYear;

        current = current->next;
    }
}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear) {
    if (!checkCityName(city1) || !checkCityName(city2))
        return false;

    City *firstCity = search_hmap(map->nameToCity, (void *) city1);
    City *secondCity = search_hmap(map->nameToCity, (void *) city2);
    Road *road1 = areConnected(firstCity, secondCity);
    Road *road2 = areConnected(secondCity, firstCity);

    if (!firstCity || !secondCity || road1 == NULL || road2 == NULL || repairYear == 0 ||
        road1->builtYear > repairYear || road2->builtYear > repairYear)
        return false;

    road1->builtYear = repairYear;
    road2->builtYear = repairYear;

    for (int i = 1; i < ROUTES_SIZE; ++i) {
        if (map->routes[i] != NULL) {
            repairInRoute(map->routes[i], firstCity, secondCity, repairYear);
        }
    }

    return true;
}

static inline void Route_getUtil(City **parent, City *destination, Route *route) {
    if (parent[destination->id] == NULL)
        return;

    Route_getUtil(parent, parent[destination->id], route);

    RouteNode *lastRouteNode = route->routeNodeList->tail;
    Road *road = areConnected(lastRouteNode->city, destination);
    lastRouteNode->length = road->length;
    lastRouteNode->age = road->builtYear;
    lastRouteNode->next = RouteNode_create(destination, 0, 0);
    route->routeNodeList->tail = lastRouteNode->next;
}

static inline Route *Route_get(City **parent, City *destination, City *srcCity) {
    Route *route = Route_create();
    RouteNode *routeNode = RouteNode_create(srcCity, 0, 0);
    route->routeNodeList->head = routeNode;
    route->routeNodeList->tail = routeNode;
    Route_getUtil(parent, destination, route);

    return route;
}

static inline int compareMin(int year1, int year2) {
    return year1 < year2 ? year1 : year2;
}

bool isUnique(Route *optimalPath, uint64_t const *dist, int const *years) {
    RouteNode *prevInOptimal = (RouteNode *) optimalPath->routeNodeList->head;
    RouteNode *nextInOptimal = prevInOptimal->next;

    while (nextInOptimal != NULL) {
        Road *nextCityNeighbour = (Road *) nextInOptimal->city->roadsList->head;
        uint64_t nextCityId = nextInOptimal->city->id;

        while (nextCityNeighbour != NULL) {
            if (nextCityNeighbour->adjCity != prevInOptimal->city) {
                uint64_t neighbourId = nextCityNeighbour->adjCity->id;
                uint64_t length = nextCityNeighbour->length;
                int year = nextCityNeighbour->builtYear;

                if ((dist[neighbourId] + length == dist[nextCityId]) &&
                    compareMin(years[neighbourId], year) <= years[nextCityId])
                    return false;
            }

            nextCityNeighbour = nextCityNeighbour->nextRoadOfCity;
        }

        prevInOptimal = nextInOptimal;
        nextInOptimal = nextInOptimal->next;
    }

    return true;
}

Route *dijkstra(Map *map, City *src, City *destination) {
    uint64_t vertices = map->nCities; // Get the number of vertices in graph
    uint64_t dist[vertices];      // Dist values used to pick minimum weight edge in cut
    int years[vertices];          // Years array is used to keep the oldest year from source
    City *parent[vertices];       // Parent array to get the optimalPath by recursion

    Heap *heap = Heap_create(vertices);
    if (heap == NULL)
        return NULL;

    for (uint64_t v = 0; v < vertices; ++v) {
        parent[v] = NULL;
        dist[v] = UINT64_MAX;
        years[v] = INT32_MAX;
        heap->array[v] = HeapNode_create(v, dist[v], years[v]);
        if (heap->array[v] == NULL)
            return NULL;

        heap->pos[v] = v;
    }

    heap->array[src->id]->id = src->id;
    heap->array[src->id]->distance = dist[src->id];
    heap->array[src->id]->year = years[src->id];

    heap->pos[src->id] = src->id;
    dist[src->id] = 0;
    years[src->id] = INT32_MAX;
    heap->size = vertices;
    decreaseKey(heap, src->id, dist[src->id], years[src->id]);

    while (!isEmpty(heap)) {
        // Extract the vertex with minimum distance value
        HeapNode *heapNode = extract_Min(heap);
        uint64_t u = heapNode->id;
        Road *pCrawl = (Road *) map->cities[u]->roadsList->head;

        while (pCrawl != NULL) {
            uint64_t v = pCrawl->adjCity->id;

            if (isInHeap(heap, v) && pCrawl->adjCity->visited == false && dist[u] != UINT64_MAX &&
                (pCrawl->length + dist[u] < dist[v] ||
                 (pCrawl->length + dist[u] == dist[v] && compareMin(pCrawl->builtYear, years[u]) > years[v]))) {
                dist[v] = dist[u] + pCrawl->length;
                years[v] = compareMin( pCrawl->builtYear, years[u]);
                parent[v] = map->cities[u];

                decreaseKey(heap, v, dist[v], years[v]);
            }

            pCrawl = pCrawl->nextRoadOfCity;
        }

        free(heapNode);
    }

    free_Heap(heap);

    Route *optimalPath = Route_get(parent, destination, src);
    RouteNode *tail = optimalPath->routeNodeList->tail;

    if (isUnique(optimalPath, dist, years) && tail->city == destination) {
        return optimalPath;
    }

    UnusedRoute_free(optimalPath);
    return NULL;
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

    if (shortestPath == NULL)
        return false;

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

void get_LengthSum_OldestYear(Route *route, uint64_t *lengthSum, int *year) {
    if (route == NULL)
        return;

    RouteNode *routeNode = route->routeNodeList->head;
    uint64_t sum = 0;
    int oldestYear = 0;

    while (routeNode != NULL) {
        if (oldestYear == 0) {
            oldestYear = routeNode->age;
        } else {
            oldestYear = compareMin(oldestYear, routeNode->age);
        }

        sum += routeNode->length;
        routeNode = routeNode->next;
    }

    *lengthSum = sum;
    *year = oldestYear;
}

static inline void markVisitedWithout(Route *route, City *city1, City *city2) {
    if (route == NULL)
        return;

    RouteNode *routeNode = route->routeNodeList->head;

    while (routeNode != NULL) {
        if (routeNode->city != city1 && routeNode->city != city2) {
            routeNode->city->visited = true;
            routeNode = routeNode->next;
        } else {
            routeNode->city->visited = false;
            routeNode = routeNode->next;
        }
    }
}

static inline void markVisited(Route *route) {
    if (route == NULL)
        return;

    RouteNode *routeNode = route->routeNodeList->head;

    while (routeNode != NULL) {
        routeNode->city->visited = true;
        routeNode = routeNode->next;
    }
}

static inline void markUnvisited(Route *route) {
    if (route == NULL)
        return;

    RouteNode *routeNode = route->routeNodeList->head;

    while (routeNode != NULL) {
        routeNode->city->visited = false;
        routeNode = routeNode->next;
    }
}

void extendFromEnd(Map *map, unsigned routeId, Route *route, Route *routeFromEnd) {
    RouteNode *nodeBeforeTail = route->routeNodeList->head;

    while (nodeBeforeTail->next != route->routeNodeList->tail) {
        nodeBeforeTail = nodeBeforeTail->next;
    }

    free(nodeBeforeTail->next);
    nodeBeforeTail->next = NULL;
    nodeBeforeTail->next = routeFromEnd->routeNodeList->head;
    route->routeNodeList->tail = routeFromEnd->routeNodeList->tail;

    free(routeFromEnd->routeNodeList);
    free(routeFromEnd);
    map->routes[routeId] = route;

}

void extendFromStart(Map *map, unsigned routeId, Route *route, Route *routeToStart) {
    RouteNode *nodeBeforeTail = routeToStart->routeNodeList->head;

    while (nodeBeforeTail->next != routeToStart->routeNodeList->tail) {
        nodeBeforeTail = nodeBeforeTail->next;
    }

    free(nodeBeforeTail->next);
    nodeBeforeTail->next = NULL;
    nodeBeforeTail->next = route->routeNodeList->head;
    route->routeNodeList->head = routeToStart->routeNodeList->head;

    free(routeToStart->routeNodeList);
    free(routeToStart);
    map->routes[routeId] = route;
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

    RouteNode *routeTail = route->routeNodeList->tail;
    RouteNode *routeHead = route->routeNodeList->head;
    markVisited(route);
    routeTail->city->visited = false;
    Route *routeFromEnd = dijkstra(map, routeTail->city, extendTo);
    routeHead->city->visited = false;
    routeTail->city->visited = true;
    Route *routeToStart = dijkstra(map, extendTo, routeHead->city);
    routeHead->city->visited = false;
    routeTail->city->visited = false;
    uint64_t fromEndLength = 0, fromStartLength = 0;
    int fromEndYear = 0, fromStartYear = 0;

    if (routeFromEnd == NULL && routeToStart == NULL) {
        markUnvisited(route);
        return false;
    }

    get_LengthSum_OldestYear(routeFromEnd, &fromEndLength, &fromEndYear);
    get_LengthSum_OldestYear(routeToStart, &fromStartLength, &fromStartYear);

    if (routeFromEnd != NULL && routeToStart != NULL && fromEndLength == fromStartLength &&
        fromEndYear == fromStartYear) {
        UnusedRoute_free(routeToStart);
        UnusedRoute_free(routeFromEnd);
        markUnvisited(route);
        return false;
    }

    if (routeFromEnd != NULL &&
        (routeToStart == NULL ||
         (fromEndLength < fromStartLength || (fromEndLength == fromStartLength && fromEndYear > fromStartYear)))) {
        extendFromEnd(map, routeId, route, routeFromEnd);
        UnusedRoute_free(routeToStart);
    } else if (routeToStart != NULL &&
               (routeFromEnd == NULL || (fromStartLength < fromEndLength ||
                                         (fromEndLength == fromStartLength && fromStartYear > fromEndYear)))) {
        extendFromStart(map, routeId, route, routeToStart);
        UnusedRoute_free(routeFromEnd);
    }

    markUnvisited(route);
    return true;
}

void removeRoadInAdjList(City *city1, City *city2) {
    Road *fakeHead = Road_create(NULL, 0, 0);
    fakeHead->nextRoadOfCity = (Road *) city1->roadsList->head;
    Road *road = fakeHead;

    while (road->nextRoadOfCity != NULL) {
        if (road->nextRoadOfCity->adjCity == city2) {
            Road *next = road->nextRoadOfCity->nextRoadOfCity;
            free(road->nextRoadOfCity);
            road->nextRoadOfCity = NULL;
            road->nextRoadOfCity = next;
            city1->roadsList->head = fakeHead->nextRoadOfCity;

            if (city1->roadsList->head == NULL)
                city1->roadsList->tail = NULL;
            else if (road->nextRoadOfCity == NULL)
                city1->roadsList->tail = road;

            free(fakeHead);
            return;
        }

        road = road->nextRoadOfCity;
    }

    free(fakeHead);
}

bool isRoadInRoute(Route *route, City *city1, City *city2) {
    RouteNode *routeNode = route->routeNodeList->head;

    while (routeNode != NULL && routeNode->next != NULL) {
        if (routeNode->city == city1 && routeNode->next->city == city2)
            return true;
        else if (routeNode->city == city2 && routeNode->next->city == city1)
            return true;

        routeNode = routeNode->next;
    }

    return false;
}

bool dijkstraDirection(Route *route, City *city1, City *city2) {
    RouteNode *current = (RouteNode *) route->routeNodeList->head;

    while (current != NULL && current->next != NULL) {
        if (current->city == city1 && current->next->city == city2)
            return false;
        else if (current->city == city2 && current->next->city == city1) {
            return true;
        }

        current = current->next;
    }

    return false;
}

bool checkRemoveInRoutes(Map *map, City *city1, City *city2) {
    for (int i = 1; i < ROUTES_SIZE; ++i) {
        if (map->routes[i] != NULL && isRoadInRoute(map->routes[i], city1, city2)) {
            markVisitedWithout(map->routes[i], city1, city2);
            bool oppDir = dijkstraDirection(map->routes[i], city1, city2);

            if (oppDir == true) {
                City *temp = city1;
                city1 = city2;
                city2 = temp;
            }

            Route *newRoute = dijkstra(map, city1, city2);

            if (newRoute == NULL) {
                markUnvisited(map->routes[i]);
                return false;
            }

            markUnvisited(map->routes[i]);
            UnusedRoute_free(newRoute);
        }
    }

    return true;
}

void removeInRoute(Map *map, unsigned routeId, Route *route, City *city1, City *city2) {
    markVisitedWithout(route, city1, city2);
    bool oppDir = dijkstraDirection(route, city1, city2);

    if (oppDir == true) {
        City *temp = city1;
        city1 = city2;
        city2 = temp;
    }

    Route *newRoute = dijkstra(map, city1, city2);
    markUnvisited(newRoute);

    if (newRoute == NULL) {
        markUnvisited(route);
        return;
    }

    RouteNode *fakeNode = RouteNode_create(NULL, 0, 0);
    fakeNode->next = (RouteNode *) route->routeNodeList->head;
    RouteNode *current = fakeNode;

    while (current->next->next != NULL) {
        if ((current->next->city == city1 && current->next->next->city == city2) ||
            (current->next->city == city2 && current->next->next->city == city1)) {
            RouteNode *next = current->next->next;
            int builtYear = current->age;
            uint64_t length = current->length;
            free(current->next);
            current->next = NULL;
            current->next = (RouteNode *) newRoute->routeNodeList->head;
            current->age = builtYear;
            current->length = length;
            RouteNode *newRouteTail = (RouteNode *) newRoute->routeNodeList->tail;
            newRouteTail->next = next->next;
            newRouteTail->length = next->length;
            newRouteTail->age = next->age;
            free(next);
            route->routeNodeList->head = fakeNode->next;

            if (newRouteTail->next == NULL)
                route->routeNodeList->tail = newRouteTail;

            free(newRoute->routeNodeList);
            free(newRoute);
            free(fakeNode);
            map->routes[routeId] = route;
            markUnvisited(route);
            return;
        }

        current = current->next;
    }

    markUnvisited(route);
    UnusedRoute_free(newRoute);
    free(fakeNode);
}

bool removeRoad(Map *map, const char *city1, const char *city2) {
    if (!checkCityName(city1) || !checkCityName(city2))
        return false;

    City *firstCity = (City *) search_hmap(map->nameToCity, (void *) city1);
    City *secondCity = (City *) search_hmap(map->nameToCity, (void *) city2);
    Road *road1 = areConnected(firstCity, secondCity);
    Road *road2 = areConnected(secondCity, firstCity);

    if (!firstCity || !secondCity || !road1 || !road2 || strcmp(city1, city2) == 0)
        return false;

    uint64_t length = road1->length;
    int year = road1->builtYear;
    removeRoadInAdjList(firstCity, secondCity);
    removeRoadInAdjList(secondCity, firstCity);

    if (!checkRemoveInRoutes(map, firstCity, secondCity)) {
        addRoad(map, firstCity->cityName, secondCity->cityName, length, year);
        return false;
    }

    for (int i = 1; i < ROUTES_SIZE; ++i) {
        if (map->routes[i] != NULL && isRoadInRoute(map->routes[i], firstCity, secondCity)) {
            removeInRoute(map, i, map->routes[i], firstCity, secondCity);
        }
    }

    return true;
}

bool removeRoute(Map *map, unsigned routeId) {
    if (routeId < 1 || routeId > 999 || map->routes[routeId] == NULL)
        return false;

    UnusedRoute_free(map->routes[routeId]);
    map->routes[routeId] = NULL;

    return true;
}

char const *getRouteDescription(Map *map, unsigned routeId) {
    if (routeId < 1 || routeId > 999 || map->routes[routeId] == NULL) {
        char *c = malloc(1);

        if (c == NULL)
            return NULL;

        c[0] = '\0';
        return c;
    }

    Route *route = map->routes[routeId];
    RouteNode *routeNode = route->routeNodeList->head;
    size_t totalSize = BUFF_SIZE;
    char *b = (char *) malloc(totalSize * sizeof(char));

    if (b == NULL)
        return NULL;

    if (routeNode == NULL) {
        free(b);
        char *c = malloc(1);
        c[0] = '\0';
        return c;
    }

    sprintf(b, "%u", routeId);
    size_t b_size = strlen(b);

    while (routeNode->next != NULL) {
        sprintf(b_size + b, ";%s;%u;%d", routeNode->city->cityName, routeNode->length, routeNode->age);
        b_size = strlen(b);

        if (b_size > (totalSize * 0.75)) {
            totalSize = 2 * totalSize;
            b = (char *) realloc(b, totalSize * sizeof(char));
        }

        routeNode = routeNode->next;
    }

    sprintf(b_size + b, ";%s", routeNode->city->cityName);

    return b;
}