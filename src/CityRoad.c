#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "CityRoad.h"

List *List_create(void) {
    List *list = (List *) malloc(sizeof(List));
    if (list == NULL)
        return NULL;

    list->head = NULL;
    list->tail = NULL;

    return list;
}

Road *Road_create(City *adjCity, unsigned length, int builtYear) {
    Road *road = (Road *) malloc(sizeof(Road));

    if (road == NULL)
        return NULL;

    road->adjCity = adjCity;
    road->length = length;
    road->builtYear = builtYear;
    road->nextRoadOfCity = NULL;

    return road;
}

City *City_create(const char *cityName, uint64_t id) {
    City *city = (City *) malloc(sizeof(City));

    if (city == NULL)
        return NULL; /* In case when allocation failed */

    char *str_new = (char *) malloc(sizeof(char) * (strlen(cityName)+1));

    if (str_new == NULL)
        return NULL;

    strcpy(str_new, cityName);
    city->cityName = str_new;
    city->visited = false;
    city->id = id;
    city->roadsList = List_create();

    return city;
}

Route *Route_create(void) {
    Route *route = (Route *) malloc(sizeof(Route));

    if (route == NULL)
        return NULL;

    route->routeNodeList = List_create();

    return route;
}

RouteNode *RouteNode_create(City *city, unsigned length, int age) {
    RouteNode *routeNode = (RouteNode *) malloc(sizeof(RouteNode));

    if (routeNode == NULL)
        return NULL;

    routeNode->city = city;
    routeNode->length = length;
    routeNode->age = age;
    routeNode->next = NULL;

    return routeNode;
}

Road *areConnected(City *city1, City *city2) {
    if (city1 == NULL || city2 == NULL)
        return NULL;

    Road *currentRoad = city1->roadsList->head;

    while (currentRoad != NULL) {
        if (currentRoad->adjCity == city2)
            return currentRoad;

        currentRoad = currentRoad->nextRoadOfCity;
    }

    return NULL;
}