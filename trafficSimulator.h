// edited, turn this file in

#ifndef _trafficSimulator_h
#define _trafficSimulator_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "graph.h"
#include "queue.h"
#include "priorityQueue.h"
#include "event.h"
#include "road.h"



//Data structure created by Joshua Freeman on 11/13/23
typedef struct TrafficData { // DONE

    // mapping of all positions in every road set as a Graph* type
    Graph* roadMap;

    // array of the roads of the graph in the sequence they were added to the graph
    RoadData** roadsArray;

    // priority queue of events where the priority represents the time the event will occur
    PriorityQueue* eventQueue;

    // track the number of cars still in the simulator
    int totalCars;

    // MADE BY ME track the amount of roads that are present within the trafficData because I'm tired of spending hours
    // on this same issue
    int totalRoads;

    // MADE BY ME track the amount of events that are present within the trafficData because I'm tired of spending hours
    // on this same issue
    int totalEvents;

    // MADE BY ME store the amount of times a time step is to be printed
    int printStepsCount;

    // MADE BY ME store the time steps that are going to be printed
    int* printSteps;

} TrafficData;



void printNames();

TrafficData* createTrafficData(char* name);

void trafficSimulator(TrafficData* pTrafficData);

void freeTrafficData(TrafficData* pTrafficData);

#endif
