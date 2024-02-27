// edited, turn this file in

#ifndef _road_h
#define _road_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "car.h"
#include "queue.h"



// Data structure created by Joshua Freeman on 11/13/23
typedef struct RoadData {

    // length of the road
    int roadLength;

    // steps where green is on
    int greenOn;

    // steps where green is off
    int greenOff;

    // rate at which the light changes
    int resetLight;

    // 2 intersection where the road starts and terminates
    int intersectionsArray[2];

    /* Each road should have an array to track cars currently driving on them. The type of this array can be either `Car**` 
     * (array of car pointers) or `Car*` (array of cars) but the former may be easier to use since locations on the road can 
     * be set to `NULL` if no car is present.
     */
    Car** carsArray; // array of car positions, including empty spaces
    
    /* For each road it is recommended to have a `queue` of cars waiting to enter the simulated road system (see `queue.h` for 
     * the queues functions which have already been implemented for you). Cars will be put into this queue only when an add 
     * car event occurs for this road.
     */
    Queue* carsQueue; // queue of cars for a particular road

} RoadData;

RoadData* createRoad(int from, int to, int length, int on, int off, int step);

void freeRoad(RoadData* road);

bool updateTrafficLight(int timeStep, RoadData* road);


#endif

