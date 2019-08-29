/** @file
 * Class interface storing the following structures:
 * City, Road, Route.
 *
 * @author Gor Stepanyan <gs404865@mimuw.edu.pl>
 * @copyright Gor Stepanyan
 * @date 29.08.2019
 */

#ifndef GRAPHS_CITYROAD_H
#define GRAPHS_CITYROAD_H

#include <stdint.h>

/**
 * Structure storing a city with edges to
 * adjacent cities.
 */
typedef struct City City;

/**
 * Structure storing a road.
 */
typedef struct Road Road;

/**
 * Structure storing a route between two cities.
 */
typedef struct Route Route;

/**
 * Structure storing a routNode.
 */
typedef struct RouteNode RouteNode;

/**
 * Structure storing a linked list for intermediary uses.
 */
typedef struct LinkedList List;

struct LinkedList {
    void *head; /**< Head of the linked list. */
    void *tail; /**< Tail keeps the reference on the last node of the list. */
};

struct Road {
    City *adjCity; /**< Reference on the adjacent city. */
    unsigned length; /**< Length of the road. */
    int builtYear; /**< Built year or the year of the last repair of the road. */
    Road *nextRoadOfCity; /**< Keeps reference on the next Road. */
};

struct City {
    char *cityName; /**< Name of the city. */
    uint64_t id; /**< Id of the city, to be used in dijkstra algorithm. */
    bool visited; /**< Whether city is visited, to be used in dijkstra algorithm. */
    List *roadsList; /**< List of the roads to adjacent cities. */
};

struct Route {
    List *routeNodeList; /**< List of the RouteNodes. */
};

struct RouteNode {
    City *city; /**< Reference on a city in the route. */
    unsigned length; /**< Length of a road between two cities in a route. */
    int age; /**< Built year of a road between two cities in a route. */
    struct RouteNode *next; /**< Reference on the next RouteNode in the route. */
};

/**
 * @brief Creates a linked list.
 * The list type may be any specified structure. To be used to keep the head and tail of
 * any list to prevent each time finding the last node of the list.
 * @return a pointer on a created linked list, or NULL if allocation failed.
 */
List *List_create(void);

/**
 * @brief Creates road to adjacent city.
 * @param[in] adjCity   - pointer to adjacent city;
 * @param[in] length    - length of the road to adjacent city;
 * @param[in] builtYear - built year or the year of the last repair of the road.
 * @return pointer on the created road, or NULL if memory allocation failed.
 */
Road *Road_create(City *adjCity, unsigned length, int builtYear);

/**
 * @brief Creates a city with a given name without any roads.
 * @param[in] cityName - name of the city to be created;
 * @param[in] id       - id of the city.
 * @return pointer on the created city, or NULL if memory allocation failed.
 */
City *City_create(const char *cityName, uint64_t id);

/**
 * @brief Creates new route.
 * Route has a linked list of RouteNodes.
 * @return pointer on a created route, or NULL if memory allocation failed.
 */
Route *Route_create(void);

/**
 * @brief Creates new RouteNode.
 * RouteNode contains reference to a city. Length and age are the parameters
 * of the road between city of the RouteNode and the city of the next RouteNode.
 * @param[in] city   - pointer on a city;
 * @param[in] length - length of the road between city and the next RouteNode city;
 * @param[in] age    - built year of the road between city and the next RouteNode city.
 * @return pointer on the created route node, or NULL if memory allocation failed.
 */
RouteNode *RouteNode_create(City *city, unsigned length, int age);

/**
 * @brief Checks if city1 and city2 are connected.
 * Enough to check for only one pair.
 * @param[in] city1 - pointer on the city1;
 * @param[in] city2 - pointer on the city2.
 * @return pointer on the road if city2 is in city1's adjacency list, otherwise NULL.
 */
Road *areConnected(City *city1, City *city2);

#endif //GRAPHS_CITYROAD_H
