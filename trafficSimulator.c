// edited, turn this file in

#include "trafficSimulator.h"

void printRoadData(int length, int from, int to, int greenStartTime, int greenEndTime, int lightCycleLength);

int max(int a, int b);



void printNames() { // DONE

    printf("\nThis solution was completed by:\n");
    printf("Matthew Beck (vax202)\n");
    printf("Joshua Freeman (uyv353)\n\n");
}



// scanner mainly created by Matthew Beck, with troubleshooting from Joshua Freeman, 11/15/23 to 11/22/23
TrafficData* createTrafficData(char* filename) { // DONE

    // dependencies for opening and looking through the .txt file
    FILE *pFile = fopen(filename, "r");
    char lineData[20];

    // dependency for creating data structure to analyze
    TrafficData* trafficData = malloc(sizeof(*trafficData));

    // dependencies for collecting road data and then storing it
    int totalIntersections, totalRoads, i = 0, j, k, incomingRoads, fromIntersection, toIntersection, roadLength, greenOn,
    greenOff, resetLight, intCount, prevIntCount, incomingRoadsTracker = 0, currentIntersection = -1, allInts[90],
    currentRoad = 0, totalNums = 0;

    // dependencies for organizing events and putting them into a priority queue
    int totalEvents, currentEvent = -1, eventPart = 0, eventFrom, eventTo, eventStep, carsToAdd;
    trafficData->eventQueue = createPQ(); // PriorityQueue* type eventQueue to queue events based on importance
    Event* event; // Event* type event to hold event data as it is found
    Car* car; // Car* type car to hold car data as it is found
    trafficData->totalCars = 0; // pre-initialize value for counting total cars in the system

    // dependencies for printing road events
    int printPart = 0;



    /********** READ FILE **********/

    // loop though the file until it terminates
    while (fgets(lineData, 20, pFile) != NULL) {

        // declare int counter
        intCount = sscanf(lineData, "%d %d %d %d %d", &allInts[0], &allInts[1], &allInts[2], &allInts[3], &allInts[4]);



        /********** CREATE TRAFFIC DATA **********/

        // if 2 ints detected and i = 0...
        if (intCount == 2 && i == 0 && currentEvent == -1 && printPart == 0) {

            sscanf(lineData, "%d %d", &totalIntersections, &totalRoads);

            // increment total numbers for the fscanf collection
            totalNums = totalNums + 2;

            printf("Add the roads:\n");

            // store the amount of roads in the system within the data structure holding it
            trafficData->totalRoads = totalRoads;

            // create Graph* type roadMap to size of all intersections (vertexes)
            trafficData->roadMap = createGraph(totalIntersections);

            // allocate memory for array of roadData roadsArray within trafficData
            trafficData->roadsArray = (RoadData**)malloc(sizeof(RoadData)*totalRoads);
        }



        /********** CREATE ROAD DATA **********/

        // if one integer is detected and not all roads have been created...
        else if (intCount == 1 && currentRoad < totalRoads && currentEvent == -1 && printPart == 0) {

            // incomingRoadsTracker detects how many times incomingRoads is displayed,
            // and by extension, also how many times currentIntersection is incremented
            incomingRoadsTracker++;

            // currentIntersection keeps track of which intersection all incoming roads lead to,
            // and should never exceed totalRoads
            currentIntersection++;

            sscanf(lineData, "%d", &incomingRoads);

            // increment total numbers for the fscanf collection
            totalNums++;
        }

        // if five integers are detected hinting at road data and not all roads have been created...
        else if (intCount == 5 && currentRoad < totalRoads && currentEvent == -1 && printPart == 0) {

            // sets tracker back to 0
            incomingRoadsTracker--;

            // currentRoad counts how many times a road is inputted into trafficData->roadsArray,
            // and should never exceed totalRoads
            currentRoad++;

            // if 'intCount == 1 && currentRoad < totalRoads' conditional called more...
            if (incomingRoadsTracker > 1) {

                // update currentIntersection if more intersections made than roads
                currentIntersection = currentIntersection - (incomingRoadsTracker - 1);

                // reset the road tracker
                incomingRoadsTracker = 0;
            }

            // set toIntersection variable as current intersection
            toIntersection = currentIntersection;

            sscanf(lineData, "%d %d %d %d %d", &fromIntersection, &roadLength, &greenOn, &greenOff, &resetLight);

            // increment total numbers for the fscanf collection
            totalNums = totalNums + 5;

            // call createRoad function from road.c in order to take data scraped from file and store it into
            // a RoadData* type struct to be used later
            trafficData->roadsArray[currentRoad - 1] = createRoad(
                    fromIntersection,
                    toIntersection,
                    roadLength,
                    greenOn,
                    greenOff,
                    resetLight
            );

            // loop through the length of the road to add empty positions to the road's carsArray
            for (j = 0; j < roadLength; j++) {

                trafficData->roadsArray[currentRoad - 1]->carsArray[j] = NULL;
            }

            // call setEdge to actually create a road represented as an edge
            setEdge(
                    trafficData->roadMap,
                    fromIntersection,
                    toIntersection,
                    roadLength
            );

            // call setEdgeData to set edge data with data found in roadsArray at the index of the current road within
            // the graph to be used later when working with cars
            setEdgeData(
                    trafficData->roadMap,
                    fromIntersection,
                    toIntersection,
                    trafficData->roadsArray[currentRoad - 1]
            );

            // call printRoadData to print out roads, their intersections, and those intersections' light states
            printRoadData(
                    trafficData->roadsArray[currentRoad - 1]->roadLength,
                    trafficData->roadsArray[currentRoad - 1]->intersectionsArray[0],
                    trafficData->roadsArray[currentRoad - 1]->intersectionsArray[1],
                    trafficData->roadsArray[currentRoad - 1]->greenOn,
                    trafficData->roadsArray[currentRoad - 1]->greenOff,
                    trafficData->roadsArray[currentRoad - 1]->resetLight
            );
        }



        /********** CREATE EVENTS **********/

        // if roadCounter that checks for how many roads have been inputted into trafficData->roadsArray equals
        // totalRoads...
        if (currentRoad == totalRoads) {

            // if 1 int found, and event processing has not started...
            if (intCount == 1 && currentEvent == -1 && printPart == 0) {

                printf("Add the cars events:");

                // currentEvent is used to store which event is being loaded into the queue, and is incremented
                // when an unwanted section of the .txt file needs to be passed or the next event must be moved into
                // the queue
                currentEvent++;

                sscanf(lineData, "%d", &totalEvents);

                // increment total numbers for the fscanf collection
                totalNums++;

                // track how many events are in each process (kinda useful, may delete later)
                trafficData->totalEvents = totalEvents;
            }

            // if the currentEvent is -1...
            else if (currentEvent > -1) {

                // if first part of event, 3 ints found, and current event set...
                if (intCount == 3 && currentEvent >= 0 && eventPart == 0 && printPart == 0) {

                    // increment eventPart counter first time
                    eventPart++;

                    sscanf(lineData, "%d %d %d", &eventFrom, &eventTo, &eventStep);

                    // increment total numbers for the fscanf collection
                    totalNums = totalNums + 3;
                }

                // if second part of event, 1 int found, and current event set...
                else if (intCount == 1 && currentEvent < totalEvents && eventPart == 1 && printPart == 0) {

                    // increment eventPart counter final time
                    eventPart++;

                    sscanf(lineData, "%d", &carsToAdd);

                    // increment total numbers for the fscanf collection
                    totalNums++;
                }

                // if third part of event and current event...
                else if (currentEvent < totalEvents && eventPart == 2 && printPart == 0) {

                    // reset eventPart counter
                    eventPart = 0;

                    // Add cars and null spaces to trafficData->roadArray[j]->carsArray that is of size roadLength
                    for (j = 0; j < totalRoads; j++) {

                        // if correct road (same eventTo, toIntersection, eventFrom, and fromIntersection)...
                        if (
                                trafficData->roadsArray[j]->intersectionsArray[0] == eventFrom &&
                                trafficData->roadsArray[j]->intersectionsArray[1] == eventTo
                                ) {

                            // fill Event* event
                            event = createAddCarEvent(
                                    eventStep,
                                    trafficData->roadsArray[j]
                            );

                            printf(
                                    "\nCreated event for time step %d on road from %d to %d.\n",
                                    eventStep,
                                    eventFrom,
                                    eventTo
                            );

                            printf("Destinations of added cars:");

                            // make new file
                            FILE *pFile2 = fopen(filename, "r");

                            // set temporary array to hold all numbers up until now
                            int carDestinations[totalNums + carsToAdd];

                            // loop through all numbers so far
                            for (k = 0; k < (totalNums + carsToAdd); k++) {

                                fscanf(pFile2, "%d", &carDestinations[k]);
                            }

                            // loop through all destinations of cars to add
                            for (k = 0; k < carsToAdd; k++) {

                                // create a car and set it to the Car* type car
                                car = createCar(
                                        trafficData->totalCars,
                                        eventStep,
                                        eventFrom,
                                        eventTo,
                                        carDestinations[(k + totalNums)]
                                );

                                // add the Car* type car above to the event car queue
                                enqueue(
                                        event->pCarQueue,
                                        car
                                );

                                // if last car to be added...
                                if (carsToAdd - k == 1) {

                                    // print car without comma
                                    printf(" %d\n", carDestinations[(k + totalNums)]);
                                }

                                // if a car is being added...
                                else {

                                    // print car with a comma
                                    printf(" %d,", carDestinations[(k + totalNums)]);
                                }

                                // increase the total amount of cars within the system after creating a car successfully
                                trafficData->totalCars++; // increase total count of cars within the system
                            }

                            // increment total numbers for the fscanf collection
                            totalNums = totalNums + carsToAdd;

                            // close the file
                            fclose(pFile2);

                            // loop through all cars to add to print their data
                            /*for (k = 0; k < carsToAdd; k++) {

                                // create a Car* type car to be stored within a road
                                car = createCar(
                                        trafficData->totalCars,
                                        eventStep,
                                        eventFrom,
                                        eventTo,
                                        allInts[k]
                                );

                                totalNums++;

                                // add the Car* type car above to the event car queue
                                enqueue(
                                        event->pCarQueue,
                                        car
                                );

                                // if last car to be added...
                                if (carsToAdd - k == 1) {

                                    // print car without comma
                                    printf(" %d\n", allInts[k]);
                                }

                                // if a car is being added...
                                else {

                                    // print car with a comma
                                    printf(" %d,", allInts[k]);
                                }

                                // increase the total amount of cars within the system after creating a car successfully
                                trafficData->totalCars++; // increase total count of cars within the system
                            }*/

                            // if correct road found, create event and input eventStep and the correct road found by matching
                            // eventFrom and fromIntersection as well as eventTo and toIntersection, then add it to the
                            // priority queue with enqueueByPriority
                            enqueueByPriority(
                                    trafficData->eventQueue,
                                    event,
                                    eventStep
                            );

                            // increase the current event count to continue to the next event in the file
                            currentEvent++;
                        }
                    }
                }



                /********** CREATE TIME STEPS **********/

                // if all events successfully loaded into queue and thus currentEvent has accounted for all events...
                if (currentEvent == totalEvents) {

                    // if only one integer is found in the line...
                    if (intCount == 1 && printPart == 0 && prevIntCount == -1) {

                        // increase the printPart to make sure the lineData is being read in the correct way
                        printPart++;

                        sscanf(lineData, "%d", &trafficData->printStepsCount);
                        //printf("(1 time) STEP COUNT, +1\n");

                        // increment total numbers for the fscanf collection
                        totalNums++;

                        // set the total number of steps to print from numprints
                        //trafficData->printStepsCount = numPrints;

                        // allocate memory to for many road events are to be printed later to the size of numPrints,
                        // which is a number that varies from file to file
                        trafficData->printSteps = (int*)malloc((trafficData->printStepsCount) * sizeof(int));

                        printf("\nAdd the roads events:\nTime steps: ");
                    }

                    // if second part of printing and there are integers present...
                    else if (printPart == 1 && intCount != -1) {

                        // crate a temporary array to collect all numbers, then get rid of the ones that aren't needed
                        int testSteps[(totalNums + trafficData->printStepsCount)];

                        // record all the print steps
                        /*for (j = 0; j < trafficData->printStepsCount; j++) {

                            // let the roadEvents equal the integers that have been detected
                            trafficData->printSteps[j] = allInts[j];

                            //fscanf(pFile, "%d", &testSteps[j]);

                            // if last step to print...
                            if (j == trafficData->printStepsCount - 1) {

                                // print data with a newline and without a comma
                                printf("%d\n", trafficData->printSteps[j]);
                            }

                            // if not last step to print...
                            else {

                                // print data with a comma
                                printf("%d, ", trafficData->printSteps[j]);
                            }
                        }*/

                        // call rewind
                        rewind(pFile);

                        // loop through every number in the file
                        for (j = 0; j < (totalNums + trafficData->printStepsCount); j++) {

                            fscanf(pFile, "%d", &testSteps[j]);
                        }

                        // loop through the amount of time steps and collect those as the time steps
                        for (j = 0; j < trafficData->printStepsCount; j++) {

                            // save the required integers to memory
                            trafficData->printSteps[j] = testSteps[(j + totalNums)];

                            // if last step to print...
                            if (j == trafficData->printStepsCount - 1) {

                                // print data with a newline and without a comma
                                printf("%d\n", trafficData->printSteps[j]);
                            }

                            // if not last step to print...
                            else {

                                // print data with a comma
                                printf("%d, ", trafficData->printSteps[j]);
                            }
                        }

                        //break the process to immediately continue to the simulation
                        break;
                    }
                }
            }
        }

        // keep track of last integer for pesky case where if there is only 1 car in an event, the process gets confused
        prevIntCount = intCount;

        // increment i to read remaining lines in the file differently from the first line
        i++;
    }

    // close the file
    fclose(pFile);

    // return data to be used in different functions
    return trafficData;
}





#define PRINT_COLOR true




// original light algorithm created by Joshua Freeman, 11/22/23
void printRoadData(int length, int from, int to, int greenStartTime, int greenEndTime, int lightCycleLength) {

    int i;
    char state='R';

    printf("Created road from %d to %d with length %d (green=%d; red=%d; reset=%d).\n", from, to, length, greenStartTime, greenEndTime, lightCycleLength);
    printf("Cycle number: ");

    for (i = 0; i < 25; i++) {

        printf("%2d ", i);
    }

    printf("...\nLight state : ");

    if (PRINT_COLOR) {

        printf("\033[0;31m"); //Color = RED
    }

    for (i = 0; i < 25; i++) { // print example light cycle:

        if (greenStartTime - 1 < i % lightCycleLength && i % lightCycleLength < greenEndTime) {

            if (PRINT_COLOR) {

                printf("\033[0;32m"); //Color = GREEN
            }

            state = 'G';
        }

        else {

            if (PRINT_COLOR) {

                printf("\033[0;31m"); //Color = RED
            }

            state = 'R';
        }

        printf("%2c ", state);
    }

    if (PRINT_COLOR) {

        printf("\033[0m"); //Reset color
    }

    printf("...\n\n");
}




// traffic simulator made by Matthew Beck and Joshua Freeman, 11/23/23 to 11/28/23
void trafficSimulator(TrafficData* pTrafficData) { // DONE

    // dependencies for simulating the traffic data gathered above
    int timeStep = 0, i, j, k, totalCars, carsLeft, totalEvents, nextIntersection, firstPosition, lastMoved = 0,
    longestLight = 0, maxSteps = 0;

    // dependency to track average steps taken for a car to travel
    double averageSteps = 0;

    // dependency for whether data should be printed or not
    bool printStep;

    // Event* type event to hold event data as it is dequeued
    Event* event;

    // static variable that will not change as pTrafficData->totalCars is slowly decremented
    totalCars = pTrafficData->totalCars;

    // number of cars that will slowly decrement as more cars leave the system
    carsLeft = totalCars;

    for (i = 0; i < pTrafficData->totalRoads; i++) {

        if (longestLight < pTrafficData->roadsArray[i]->resetLight) {

            longestLight = pTrafficData->roadsArray[i]->resetLight;
        }
    }

    // if there are cars within the system...
    while (carsLeft > -1) {

        /********** CHECK PRINT ABILITY **********/

        // loop through all print steps
        for (i = 0; i < pTrafficData->printStepsCount; i++) {

            // if the current time step is in list of steps to print...
            if (timeStep == pTrafficData->printSteps[i]) {

                // set printing to true
                printStep = true;

                // break to save time
                break;
            }

            // if the current time step not in list of steps to print...
            else {

                // set printing to true
                printStep = false;
            }
        }

        /********** LOAD EVENTS **********/

        // static variable that will not change as pTrafficData->totalEvents is slowly decremented
        totalEvents = pTrafficData->totalEvents;

        // loop through all Event* items in the priority queue while the queue is not empty
        for (i = 0; i < totalEvents; i++) {

            // if an event's time step is the current time step...
            if (pTrafficData->eventQueue->qFront->qt->eventTimeStep == timeStep) {

                // dequeue the front event in the priority queue to be used to add cars
                event = dequeuePQ(pTrafficData->eventQueue);

                // loop through each road to see if said road matches the road in the event
                for (j = 0; j < pTrafficData->totalRoads; j++) {

                    // if a road matches the road in the event...
                    if (pTrafficData->roadsArray[j]->intersectionsArray[0] == event->pRoadData->intersectionsArray[0] &&
                        pTrafficData->roadsArray[j]->intersectionsArray[1] == event->pRoadData->intersectionsArray[1]) {

                        // send the cars from the event to the roadsArray[i] car queue to be added to the road
                        mergeQueues(pTrafficData->roadsArray[j]->carsQueue, event->pCarQueue);

                        printf(
                                "STEP %d - ADD_CAR_EVENT - Cars enqueued on road from %d to %d\n",
                                timeStep,
                                event->pRoadData->intersectionsArray[0],
                                event->pRoadData->intersectionsArray[1]
                        );

                        // once road has been found and queues have been merged, break the process to save time
                        break;
                    }
                }

                // free temporary Event* event struct upon entering its data
                freeEvent(event);

                // decrement totalEvents as one event has successfully been processed
                pTrafficData->totalEvents--;
            }
        }



        /********** PRINT ROAD EVENTS **********/

        // if data from current time step can be printed...
        if (printStep == true) {

            // print the time step and the contents of the system
            printf("\nSTEP %d - PRINT_ROADS_EVENT - Current contents of the roads:\n", timeStep);

            // loop through each road to print its data
            for (i = 0; i < pTrafficData->totalRoads; i++) {

                printf(
                        "Cars on the road from %d to %d\n",
                        pTrafficData->roadsArray[i]->intersectionsArray[0],
                        pTrafficData->roadsArray[i]->intersectionsArray[1]
                );

                // loop through every position in the road to print it
                for (j = 0; j < (pTrafficData->roadsArray[i]->roadLength); j++) {

                    // if position on road empty...
                    if (pTrafficData->roadsArray[i]->carsArray[j] == NULL) {

                        // print a hyphen
                        printf("- ");
                    }

                        // if position on road contains a car...
                    else {

                        // print car represented as its destination
                        printf("%d ", pTrafficData->roadsArray[i]->carsArray[j]->destination);
                    }
                }

                // if the light is currently green...
                if (updateTrafficLight(timeStep, pTrafficData->roadsArray[i]) == true) {

                    // print green
                    printf("(GREEN light)\n");
                }

                    // if the light is currently red...
                else {

                    // print red
                    printf("(RED light)\n");
                }
            }

            // add a newline between every print
            printf("\n");
        }



        /********** MOVE CARS **********/

        // loop through each road to try and pass a car through its intersection
        for (i = 0; i < pTrafficData->totalRoads; i++) {



            /********** UPDATE EACH ROAD INTERSECTION **********/

            // if a car in final position...
            if (pTrafficData->roadsArray[i]->carsArray[0] != NULL) {

                // if the light is green...
                if (updateTrafficLight(timeStep, pTrafficData->roadsArray[i]) == true) {

                    // if car at its destination...
                    if (pTrafficData->roadsArray[i]->carsArray[0]->destination ==
                        pTrafficData->roadsArray[i]->intersectionsArray[1]) {

                        printf(
                                "STEP %d - Car successfully traveled from %d to %d in %d time steps.\n",
                                timeStep,
                                pTrafficData->roadsArray[i]->carsArray[0]->origin,
                                pTrafficData->roadsArray[i]->carsArray[0]->destination,
                                ((timeStep + 1) - pTrafficData->roadsArray[i]->carsArray[0]->stepAdded)
                        );

                        // add all steps to find the average steps in the end
                        averageSteps = averageSteps + ((timeStep + 1) - pTrafficData->roadsArray[i]->carsArray[0]->stepAdded);

                        // if time taken to travel is greater than previous record...
                        if (maxSteps < ((timeStep + 1) - pTrafficData->roadsArray[i]->carsArray[0]->stepAdded)) {

                            // change maximum steps taken to travel record
                            maxSteps = ((timeStep + 1) - pTrafficData->roadsArray[i]->carsArray[0]->stepAdded);
                        }

                        freeCar(pTrafficData->roadsArray[i]->carsArray[0]);

                        // remove the car from the road by setting its position to NULL
                        pTrafficData->roadsArray[i]->carsArray[0] = NULL;

                        // decrement carsLeft from the system by 1 every time a car successfully makes it to its destination
                        carsLeft--;

                        // keep track of time step where a car has moved as lastMoved for gridlock calculator
                        lastMoved = timeStep;
                    }

                    // if car must continue to travel...
                    else {

                        // call getNextShortestPath to determine which road to go to next
                        getNextOnShortestPath(
                                pTrafficData->roadMap,
                                pTrafficData->roadsArray[i]->intersectionsArray[1],
                                pTrafficData->roadsArray[i]->carsArray[0]->destination,
                                &nextIntersection
                        );

                        // loop to find the next road to travel on
                        for (k = 0; k < pTrafficData->totalRoads; k++) {

                            // if terminating intersection of road k next intersection found above...
                            if (pTrafficData->roadsArray[k]->intersectionsArray[1] == nextIntersection &&
                                pTrafficData->roadsArray[k]->intersectionsArray[0] == pTrafficData->roadsArray[i]->intersectionsArray[1]) {

                                // if first position in next road vacant...
                                if (pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)] == NULL) {

                                    //printf("SECOND POSITION AVAILABLE, MOVING\n");

                                    // move the car from the road it is on to the next position of the next road it
                                    // must travel on
                                    pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)] = pTrafficData->roadsArray[i]->carsArray[0];

                                    // since the car has been moved, set moved to true
                                    pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)]->moved = true;

                                    // set the final position of the previous road to NULL
                                    pTrafficData->roadsArray[i]->carsArray[0] = NULL;

                                    // keep track of time step where a car has moved as lastMoved for gridlock calculator
                                    lastMoved = timeStep;

                                    // break to save time
                                    break;
                                }

                                // if car not able to move because next spot not available...
                                else {

                                    // update moved to false
                                    pTrafficData->roadsArray[i]->carsArray[0]->moved = false;
                                }
                            }
                        }
                    }
                }

                // if car cannot move because the light is not green...
                else {

                    // update moved to false
                    pTrafficData->roadsArray[i]->carsArray[0]->moved = false;
                }
            }
        }



        /********* DEQUEUE FROM ROAD **********/

        // loop through each road to try to add a car from its queue
        for (i = 0; i < pTrafficData->totalRoads; i++) {

            // declare first position of road as the roads length minus 1 as 0 is a position in the index
            firstPosition = pTrafficData->roadsArray[i]->roadLength - 1;

            // if the first position of the road is null, check if there is a car in
            if (pTrafficData->roadsArray[i]->carsArray[firstPosition] == NULL) {

                // if a car is in the road's queue, add a car to the first position of the road
                if (getNumElements(pTrafficData->roadsArray[i]->carsQueue) > 0) {

                    // dequeue first car from the car queue
                    pTrafficData->roadsArray[i]->carsArray[firstPosition] = dequeue(pTrafficData->roadsArray[i]->carsQueue);

                    // set moved to true to prevent the car from moving again
                    pTrafficData->roadsArray[i]->carsArray[firstPosition]->moved = true;

                    // keep track of time step where a car has moved as lastMoved for gridlock calculator
                    lastMoved = timeStep;
                }
            }
        }



        /********* MOVE A CAR NORMALLY **********/

        // loop through each road to try and move a car
        for (i = 0; i < pTrafficData->totalRoads; i++) {

            // loop through each position in a road that is not the end to try and move a car
            for (j = (pTrafficData->roadsArray[i]->roadLength - 1); 0 < j; j--) {

                // if a position on the road has a car on it...
                if (pTrafficData->roadsArray[i]->carsArray[j] != NULL) {

                    // if there is a spot ahead available...
                    if (pTrafficData->roadsArray[i]->carsArray[(j - 1)] == NULL) {

                        // if car can move...
                        if (pTrafficData->roadsArray[i]->carsArray[j]->moved == false) {

                            // move car from current position to next position
                            pTrafficData->roadsArray[i]->carsArray[j - 1] = pTrafficData->roadsArray[i]->carsArray[j];

                            // set the moved boolean to true to prevent moving it again
                            pTrafficData->roadsArray[i]->carsArray[j - 1]->moved = true;

                            // set the current position to NULL
                            pTrafficData->roadsArray[i]->carsArray[j] = NULL;

                            // keep track of time step where a car has moved as lastMoved for gridlock calculator
                            lastMoved = timeStep;
                        }

                        // if car can't move...
                        else {

                            // change moved to false
                            pTrafficData->roadsArray[i]->carsArray[j]->moved = false;
                        }
                    }

                    // if no spot available...
                    else {

                        // change moved to false
                        pTrafficData->roadsArray[i]->carsArray[j]->moved = false;
                    }
                }
            }
        }



        /********** DETECT GRIDLOCK **********/

        //maybe explicitly make sure there are cars on road to begin with

        // if the amount of steps since the last car movement is greater than the longest light cycle...
        if (timeStep - lastMoved > longestLight && totalEvents == 0) {

            printf("STEP %d - Gridlock detected.\n", timeStep);

            // break the process
            break;
        }



        /********** FINAL PRINT **********/

        // if all cars freed from the system...
        if (carsLeft == 0) {

            // loop through all print steps
            for (i = 0; i < pTrafficData->printStepsCount; i++) {

                // if the current time step is in list of steps to print...
                if (timeStep + 1 == pTrafficData->printSteps[i]) {

                    // set printing to true
                    printStep = true;

                    // break to save time
                    break;
                }

                // if the current time step not in list of steps to print...
                else {

                    // set printing to true
                    printStep = false;
                }
            }

            // if data from current time step can be printed...
            if (printStep == true) {

                // print the time step and the contents of the system
                printf("\nSTEP %d - PRINT_ROADS_EVENT - Current contents of the roads:\n", (timeStep + 1));

                // loop through each road to print its beginning and destination
                for (i = 0; i < pTrafficData->totalRoads; i++) {

                    printf(
                            "Cars on the road from %d to %d\n",
                            pTrafficData->roadsArray[i]->intersectionsArray[0],
                            pTrafficData->roadsArray[i]->intersectionsArray[1]
                    );

                    // loop through every position in a road
                    for (j = 0; j < (pTrafficData->roadsArray[i]->roadLength); j++) {

                        // if a position is empty...
                        if (pTrafficData->roadsArray[i]->carsArray[j] == NULL) {

                            // print a hyphen
                            printf("- ");
                        }

                            // if a position contains a car...
                        else {

                            // print the car represented by its destination
                            printf("%d ", pTrafficData->roadsArray[i]->carsArray[j]->destination);
                        }
                    }

                    // if the light is currently green...
                    if (updateTrafficLight(timeStep, pTrafficData->roadsArray[i]) == true) {

                        // print green
                        printf("(GREEN light)\n");
                    }

                    // if the light is currently red
                    else {

                        // print red
                        printf("(RED light)\n");
                    }
                }
            }
        }



        /********** FINAL ANALYTICS **********/

        // if all cars have left the system...
        if (carsLeft == 0) {

            // calculate average steps taken by dividing sum of all steps by total cars
            averageSteps = averageSteps / totalCars;

            printf("\nAverage number of time steps to the reach their destination is %.2f.\n", averageSteps);

            printf("Maximum number of time steps to the reach their destination is %d.\n", maxSteps);

            // decrement carsLeft one last time to stop the process
            carsLeft--;
        }

        // every time the process loops, iterate timeStep to keep track of the current time until the process terminates
        timeStep++;
    }
}





void freeTrafficData(TrafficData* pTrafficData) { // DONE

    int i;

    free(pTrafficData->printSteps);

    // loop through all roads in pTrafficData and call freeRoad to free them of their memory
    for (i = 0; i < pTrafficData->totalRoads; i++) {

        freeRoad(pTrafficData->roadsArray[i]);
    }

    // free the 2D array of roadData** that holds all road data
    free(pTrafficData->roadsArray);

    /*for (i = 0; i < pTrafficData->totalEvents; i++) {

        freeEvent(pTrafficData->eventQueue->qFront->qt);
    }*/

    // free the priorityQueue* holding the list of elements
    freePQ(pTrafficData->eventQueue);

    // free the Graph* holding everything
    freeGraph(pTrafficData->roadMap);

    // free the TrafficData* struct holding all the information
    free(pTrafficData);
}





int max(int a, int b) {

    if (a > b) {

        return a;
    }

    return b;
}

