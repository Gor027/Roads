#ifndef GRAPHS_CITYROAD_H
#define GRAPHS_CITYROAD_H

#include <stdint.h>

/**
 * Structure storing a city with edges to
 * adjacent cities.
 */
typedef struct City City;

/**
 * Structure storing a road with parameters
 * length, built year and pointer to a city.
 */
typedef struct Road Road;

/**
 * Structure storing a route between two cities
 * with a given id.
 */
typedef struct Route Route;

/**
 * Structure storing a routNode which is used
 * in route to store city, leangh and builtYear
 * of the road to the next city.
 */
typedef struct RouteNode RouteNode;

/**
 * Structure storing a linked list with
 * head and tail.
 */
typedef struct LinkedList List;

struct LinkedList {
    void *head;
    void *tail; // To avoid linear complexity
};

struct Road {
    City *adjCity;
    unsigned length;
    int builtYear; /* In Range between 1 and 999 */
    Road *nextRoadOfCity; /* Next in adjacency list for a city */
};

struct City {
    const char *cityName;
    uint64_t id;
    bool visited;
    List *roadsList;
};

struct Route {
    List *routeNodeList; // For list of roads in route
};

struct RouteNode {
    City *city;
    unsigned length;
    int age;
    struct RouteNode *next;
};

/**
 * @brief Creates a linked list. The list type may be any specified struct.
 * @return a pointer on a created linked list, or NULL if allocation failed.
 */
List *List_create(void);

/**
 * @brief Creates road between city1 and city2.
 * Adds city2 in the adjacency list of city1 and vice versa.
 * Assume that both cities exist.
 * @param[in] city1     - pointer to city1;
 * @param[in] city2     - pointer to city2;
 * @param[in] length    - length of the road between city1 and city2;
 * @param[in] builtYear - construction year or the year of the last repair of the road.
 * @return pointer on the created road, or NULL if memory allocation failed.
 */
Road *Road_create(City *adjCity, unsigned length, int builtYear);

/**
 * @brief Creates a city with a given name without any roads.
 * @param[in] cityName - name of the city to be created.
 * @return pointer on the created city, or NULL if memory allocation failed.
 */
City *City_create(const char *cityName, uint64_t id);

/**
 * @brief Creates new route .
 * @return pointer on a created route, or NULL if malloc failed.
 */
Route *Route_create(void);

/**
 * @brief Creates new node for in route.
 * @param[in] city   - pointer on a city;
 * @param[in] length - length of the road between city and the next route node city;
 * @param[in] age    - age of the road between city and the next route node city.
 * @return pointer on the created route node, or NULL if malloc failed.
 */
RouteNode *RouteNode_create(City *city, unsigned length, int age);

/**
 * @brief Enough to check whether city2 is adjacent to city1.
 * @param[in] city1 - pointer on the city1;
 * @param[in] city2 - pointer on the city2.
 * @return pointer on the road if city2 is in city1's adjacency list, otherwise NULL.
 */
Road *areConnected(City *city1, City *city2);

#endif //GRAPHS_CITYROAD_H
