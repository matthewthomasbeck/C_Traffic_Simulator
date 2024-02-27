// edited, turn this file in

#include "trafficSimulator.h"

void printRoadData(int length, int from, int to, int greenStartTime, int greenEndTime, int lightCycleLength);

int max(int a, int b);



void printNames() { // DONE

  printf("\nThis solution was completed by:\n");
  printf("Matthew Beck (vax202)\n");
  printf("Joshua Freeman\n\n");
}



TrafficData* createTrafficData(char* filename) { // DONE

    // dependencies for opening and looking through the .txt file
    FILE *pFile = fopen(filename, "r");
    char lineData[200];

    // dependency for creating data structure to analyze
    TrafficData* trafficData = malloc(sizeof(*trafficData));

    // dependencies for collecting road data and then storing it
    int totalIntersections, totalRoads, i = 0, j, k, incomingRoads, fromIntersection, toIntersection, roadLength, greenOn,
    greenOff, resetLight, intCount, prevIntCount, incomingRoadsTracker = 0, currentIntersection = -1, allInts[90], currentRoad = 0;

    // dependencies for organizing events and putting them into a priority queue
    int totalEvents, currentEvent = -1, eventPart = 0, eventFrom, eventTo, eventStep, carsToAdd;
    trafficData->eventQueue = createPQ(); // PriorityQueue* type eventQueue to queue events based on importance
    Event* event; // Event* type event to hold event data as it is found
    Car* car; // Car* type car to hold car data as it is found
    trafficData->totalCars = 0; // pre-initialize value for counting total cars in the system

    // dependencies for printing road events
    int printPart = 0, numPrints;

    // loop through each line in pFile
    while (fgets(lineData, 200, pFile) != NULL) {

        // intCount is used as a detector in which how many integers it detects within a line helps determine which
        // conditional statement should be executed, and by extension, how said line of data is processed
        intCount = sscanf(lineData, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
                                    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
                                    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d", &allInts[0], &allInts[1], &allInts[2], &allInts[3], &allInts[4],
                                    &allInts[5], &allInts[6], &allInts[7], &allInts[8], &allInts[9], &allInts[10], &allInts[11], &allInts[12],
                                    &allInts[13], &allInts[14], &allInts[15], &allInts[16], &allInts[17], &allInts[18], &allInts[19], &allInts[20],
                                    &allInts[21], &allInts[22], &allInts[23], &allInts[24], &allInts[25], &allInts[26], &allInts[27], &allInts[28],
                                    &allInts[29], &allInts[30], &allInts[31], &allInts[32], &allInts[33], &allInts[34], &allInts[35], &allInts[36],
                                    &allInts[37], &allInts[38], &allInts[39], &allInts[40], &allInts[41], &allInts[42], &allInts[43], &allInts[44],
                                    &allInts[45], &allInts[46], &allInts[47], &allInts[48], &allInts[49], &allInts[50], &allInts[51], &allInts[52],
                                    &allInts[53], &allInts[54], &allInts[55], &allInts[56], &allInts[57], &allInts[58], &allInts[59], &allInts[60],
                                    &allInts[61], &allInts[62], &allInts[63], &allInts[64], &allInts[65], &allInts[66], &allInts[67], &allInts[68],
                                    &allInts[69], &allInts[70], &allInts[71], &allInts[72], &allInts[73], &allInts[74], &allInts[75], &allInts[76],
                                    &allInts[77], &allInts[78], &allInts[79], &allInts[80], &allInts[81], &allInts[82], &allInts[83], &allInts[84],
                                    &allInts[85], &allInts[86], &allInts[87], &allInts[88], &allInts[89]);

        // if 2 ints detected and i = 0, first line data processing
        if (intCount == 2 && i == 0) {

            sscanf(lineData, "%d %d", &totalIntersections, &totalRoads);

            printf("Add the roads:\n");

            // store the amount of roads in the system within the data structure holding it
            trafficData->totalRoads = totalRoads;

            // create Graph* type roadMap to size of all intersections (vertexes)
            trafficData->roadMap = createGraph(totalIntersections);

            // allocate memory for array of roadData roadsArray within trafficData
            trafficData->roadsArray = (RoadData**)malloc(sizeof(RoadData)*totalRoads);
        }

        // if one integer is detected and not all roads have been created, increment tracker variables to track position
        // within the file and calculate the amount of incomingRoads lead to the road to be created
        else if (intCount == 1 && currentRoad < totalRoads) {

            // incomingRoadsTracker detects how many times incomingRoads is displayed,
            // and by extension, also how many times currentIntersection is incremented
            incomingRoadsTracker++;

            // currentIntersection keeps track of which intersection all incoming roads lead to,
            // and should never exceed totalRoads
            currentIntersection++;

            sscanf(lineData, "%d", &incomingRoads); // collect incomingRoads
        }

        // if five integers are detected hinting at road data and not all roads have been created, decrement
        // incomingRoadsTracker that corresponds to number of intersections, increment currentRoad to show which road
        // is being created, and create a road to store in trafficData->roadsArray
        else if (intCount == 5 && currentRoad < totalRoads) {

            // sets tracker back to 0
            incomingRoadsTracker--;

            // currentRoad counts how many times a road is inputted into trafficData->roadsArray,
            // and should never exceed totalRoads
            currentRoad++;

            // if 'intCount == 1 && currentRoad < totalRoads' conditional called more,
            // bump down currentIntersection and reset incomingRoadsTracker
            if (incomingRoadsTracker > 1) {

                // update currentIntersection if more intersections made than roads
                currentIntersection = currentIntersection - (incomingRoadsTracker - 1);

                // reset the road tracker
                incomingRoadsTracker = 0;
            }

            // set toIntersection variable as current intersection
            toIntersection = currentIntersection;

            sscanf(lineData, "%d %d %d %d %d", &fromIntersection, &roadLength, &greenOn, &greenOff, &resetLight);

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

        // if roadCounter that checks for how many roads have been inputted into trafficData->roadsArray equals
        // totalRoads, maximum roads have been added and thus abandon logic above for new processing
        if (currentRoad == totalRoads) {

            // if 1 int found, and event processing has not started, store totalEvents processing
            if (intCount == 1 && currentEvent == -1) {

                printf("Add the cars events:");

                // currentEvent is used to store which event is being loaded into the queue, and is incremented
                // when an unwanted section of the .txt file needs to be passed or the next event must be moved into
                // the queue
                currentEvent++;

                sscanf(lineData, "%d", &totalEvents);

                // track how many events are in each process (kinda useful, may delete later)
                trafficData->totalEvents = totalEvents;
            }

            // if the currentEvent is -1, begin pre-process before handling events
            else if (currentEvent > -1) {

                // if first part of event, 3 ints found, and current event set, eventFrom, eventTo, begin eventPart counter
                // and start eventStep processing
                if (intCount == 3 && currentEvent >= 0 && eventPart == 0) {

                    eventPart++; // increment eventPart counter first time

                    sscanf(lineData, "%d %d %d", &eventFrom, &eventTo, &eventStep);
                }

                // if second part of event, 1 int found, and current event set, increment to next step and start
                // carsToAdd processing
                else if (intCount == 1 && currentEvent < totalEvents && eventPart == 1) {

                    // increment eventPart counter final time
                    eventPart++;

                    sscanf(lineData, "%d", &carsToAdd);
                }

                // if third part of event and current event set, eventFrom, eventTo, reset eventPart counter
                // and start eventStep processing
                else if (currentEvent < totalEvents && eventPart == 2) {

                    // reset eventPart counter
                    eventPart = 0;

                    // Add cars and null spaces to trafficData->roadArray[j]->carsArray that is of size roadLength
                    for (j = 0; j < totalRoads; j++) {

                        // if correct road (same eventTo, toIntersection, eventFrom, and fromIntersection), create carsToAdd
                        // cars with corresponding destination from allInts[k] to then store them within a
                        // trafficData->roadsArray[j]->carsQueue
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

                            for (k = 0; k < carsToAdd; k++) {

                                // create a Car* type car to be stored within a road
                                car = createCar(
                                        trafficData->totalCars,
                                        eventStep,
                                        eventFrom,
                                        eventTo,
                                        allInts[k]
                                        );

                                // add the Car* type car above to the event car queue
                                enqueue(
                                        event->pCarQueue,
                                        car
                                        );

                                // if last car to be added, print the car without the comma
                                if (carsToAdd - k == 1) {

                                    printf(" %d\n", allInts[k]);
                                }

                                // if a car is being added, print the car with a comma
                                else {

                                    printf(" %d,", allInts[k]);
                                }

                                // increase the total amount of cars within the system after creating a car successfully
                                trafficData->totalCars++; // increase total count of cars within the system
                            }

                            // if correct road found, create event and input eventStep and the correct road found by matching
                            // eventFrom and fromIntersection as well as eventTo and toIntersection, then add it to the
                            // priority queue with enqueueByPriority
                            enqueueByPriority(
                                    trafficData->eventQueue,
                                    event,
                                    eventStep
                            );

                            //printf("\nenqueued\n");

                            // increase the current event count to continue to the next event in the file
                            currentEvent++;
                        }
                    }
                }

                // if all events successfully loaded into queue and thus currentEvent has accounted for all events,
                // maximum events have been reached and thus abandon logic above for final processing
                if (currentEvent == totalEvents) {

                    // if only one integer is found in the line, the format dictates that this is the number of items to print
                    if (intCount == 1 && printPart == 0 && prevIntCount == -1) {

                        // increase the printPart to make sure the lineData is being read in the correct way
                        printPart++;

                        sscanf(lineData, "%d", &numPrints);

                        trafficData->printStepsCount = numPrints;

                        // allocate memory to for many road events are to be printed later to the size of numPrints,
                        // which is a number that varies from file to file
                        trafficData->printSteps = (int*)malloc((numPrints) * sizeof(int));

                        printf("\nAdd the roads events:\n");
                    }

                    // if second part of printing and there are integers present, continue to final processing
                    else if (printPart == 1 && intCount != -1) {

                        for (j = 0; j < trafficData->printStepsCount; j++) {

                            // let the roadEvents equal the integers that have been detected
                            trafficData->printSteps[j] = allInts[j];

                            printf("%d ", trafficData->printSteps[j]);
                        }

                        // if file read in perfectly, break the process to immediately continue to the simulation
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

    fclose(pFile);

    /* HINTs:
     *
     * You'll add edges to a graph to represent each road using `setEdge` (see `graph.c`). The vertices of the graph
     * represent intersections. The graph will do the work of finding shortest paths (see `getNextOnShortestPath` in graph.c
     * and the longest shortest path problem on Blackboard).
     *
     * It will also help to add your `RoadData` structs to your graph using `setEdgeData` (see `graph.c`). You can then
     * retrieve that `RoadData` struct by calling `getEdgeData`. This will be helpful for finding roads when cars pass
     * through intersections.
     *
     * It's recommended to store all of the `Event` structs in a single priority queue (using time step as the priority). This
     * allows you to quickly find the next event to execute by time step (see `enqueueByPriority`, `getFrontPriority`, and
     * `dequeuePQ`).
     */

    // return data to be used in different functions
    return trafficData;
}



#define PRINT_COLOR true



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

/* trafficSimulator
 * input: char* name of file to read
 * output: N/A
 *
 * Simulate the road network in the given TrafficData variable
 */
void trafficSimulator(TrafficData* pTrafficData) { // TODO

    // dependencies for simulating the traffic data gathered above
    int timeStep = 0, i, j, k, totalCars, totalEvents, nextIntersection, firstPosition, carCount = 0;

    // boolean reset for every road to determine if it's respective intersection is passable or not
    bool isGreen, canDequeue, gridLockFlag = false;

    // Event* type event to hold event data as it is dequeued
    Event* event;

    // static variable that will not change as pTrafficData->totalCars is slowly decremented
    totalCars = pTrafficData->totalCars;

    // if there are cars within the system, keep looping to run the system until they are all gone
    while (totalCars > -1 && gridLockFlag == false) {

        // static variable that will not change as pTrafficData->totalEvents is slowly decremented
        totalEvents = pTrafficData->totalEvents;

        // loop through all Event* items in the priority queue while the queue is not empty
        for (i = 0; i < totalEvents; i++) {

            // if an event's time step is the current time step, dequeue the event and process it
            if (pTrafficData->eventQueue->qFront->qt->eventTimeStep == timeStep) {

                // dequeue the front event in the priority queue to be used to add cars
                event = dequeuePQ(pTrafficData->eventQueue);

                // loop through each road to see if said road matches the road in the event
                for (j = 0; j < pTrafficData->totalRoads; j++) {

                    // if a road matches the road in the event, merge its queues
                    if (pTrafficData->roadsArray[j]->intersectionsArray[0] == event->pRoadData->intersectionsArray[0] &&
                        pTrafficData->roadsArray[j]->intersectionsArray[1] == event->pRoadData->intersectionsArray[1]) {

                        // free the roadsArray[i] carQueue because it for some reason is filled
                        freeQueue(pTrafficData->roadsArray[j]->carsQueue);

                        // create a new empty roadsArray[i] carsQueue to use
                        pTrafficData->roadsArray[j]->carsQueue = createQueue();

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

        printf("\nSTEP %d - PRINT_ROADS_EVENT - Current contents of the roads:\n", timeStep);

        // loop through each road to print its data
        for (i = 0; i < pTrafficData->totalRoads; i++) {

            // calculate if the traffic light is green or not
            if (pTrafficData->roadsArray[i]->greenOn - 1 < timeStep % pTrafficData->roadsArray[i]->resetLight &&
                timeStep % pTrafficData->roadsArray[i]->resetLight < pTrafficData->roadsArray[i]->greenOff) {

                isGreen = true;
            }

            else {

                isGreen = false;
            }

            printf(
                    "Cars on the road from %d to %d\n",
                    pTrafficData->roadsArray[i]->intersectionsArray[0],
                    pTrafficData->roadsArray[i]->intersectionsArray[1]
            );

            // loop through every position in the road to print it
            for (j = 0; j < (pTrafficData->roadsArray[i]->roadLength); j++) {

                if (pTrafficData->roadsArray[i]->carsArray[j] == NULL) {

                    printf("- ");
                }

                    // if a position contains a car, print the car
                else {

                    printf("%d ", pTrafficData->roadsArray[i]->carsArray[j]->destination);
                }
            }

            // if the light is currently green, print green
            if (isGreen == true) {

                printf("(GREEN light)\n");
            }

                // if the light is currently red, print red
            else {

                printf("(RED light)\n");
            }
        }

        // add a newline between every print
        printf("\n");

        // loop through each road for its turn to add a car
        for (i = 0; i < pTrafficData->totalRoads; i++) {

            // calculate if the traffic light is green or not
            if (pTrafficData->roadsArray[i]->greenOn - 1 < timeStep % pTrafficData->roadsArray[i]->resetLight &&
                timeStep % pTrafficData->roadsArray[i]->resetLight < pTrafficData->roadsArray[i]->greenOff) {

                isGreen = true;
            }

            else {

                isGreen = false;
            }

            firstPosition = pTrafficData->roadsArray[i]->roadLength - 1;

            //printf("LENGTH OF ROAD: %d\n", pTrafficData->roadsArray[i]->roadLength);
            //printf("FIRST POSITION: %d\n", firstPosition);

            // loop through the length of a road to print the positions within said road
            for (j = 0; j < pTrafficData->roadsArray[i]->roadLength; j++) {
            //for (j = (pTrafficData->roadsArray[i]->roadLength - 1); 0 <= j; j--) {

                // FINAL POSITION IN ROAD
                if (j == 0) {

                    // if a car in final position
                    if (pTrafficData->roadsArray[i]->carsArray[0] != NULL) {

                        // if the car can move
                        //if (pTrafficData->roadsArray[i]->carsArray[0]->moved == false) {

                            // if the light is green
                            if (isGreen == true) {

                                // if car at its destination
                                if (pTrafficData->roadsArray[i]->carsArray[0]->destination ==
                                    pTrafficData->roadsArray[i]->intersectionsArray[1]) {

                                    printf(
                                            "\nSTEP %d - Car %d successfully traveled from %d to %d in %d time steps.\n",
                                            timeStep,
                                            carCount,
                                            pTrafficData->roadsArray[i]->carsArray[0]->origin,
                                            pTrafficData->roadsArray[i]->carsArray[0]->destination,
                                            ((timeStep + 1) - pTrafficData->roadsArray[i]->carsArray[0]->stepAdded)
                                    );

                                    carCount++;

                                    // remove the car from the road by setting its position to NULL
                                    pTrafficData->roadsArray[i]->carsArray[0] = NULL;

                                    // decrement total cars from the system by 1 every time a car successfully makes it to its destination
                                    totalCars--;
                                }

                                // if car must continue to travel
                                else {

                                    // call getNextShortestPath to determine which road to go to next
                                    getNextOnShortestPath(
                                            pTrafficData->roadMap,
                                            pTrafficData->roadsArray[i]->intersectionsArray[1],
                                            pTrafficData->roadsArray[i]->carsArray[0]->destination,
                                            &nextIntersection
                                    );

                                    //printf("NEXT INTERSECTION IS: %d\n", nextIntersection);

                                    // if path ahead possible
                                    //if (nextIntersection != false) {

                                        // loop to find the next road to travel on
                                        for (k = 0; k < pTrafficData->totalRoads; k++) {

                                            // if terminating intersection of road k next intersection found above,
                                            // move car to the first position of its cars array
                                            if (pTrafficData->roadsArray[k]->intersectionsArray[1] == nextIntersection &&
                                                pTrafficData->roadsArray[k]->intersectionsArray[0] == pTrafficData->roadsArray[i]->intersectionsArray[1]) {

                                                // if first position in next road vacant
                                                if (pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)] == NULL) {

                                                    // if second position in next road not taken
                                                    if (pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 2)] == NULL) {

                                                        //printf("SECOND POSITION AVAILABLE, MOVING\n");

                                                        // move the car from the road it is on to the next position of the
                                                        // next road it must travel on
                                                        pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)] = pTrafficData->roadsArray[i]->carsArray[0];

                                                        // since the car has been moved, set moved to true
                                                        pTrafficData->roadsArray[k]->carsArray[(pTrafficData->roadsArray[k]->roadLength - 1)]->moved = true;

                                                        // set the final position of the previous road to NULL
                                                        pTrafficData->roadsArray[i]->carsArray[0] = NULL;

                                                        // break to save time
                                                        break;
                                                    }

                                                    // if car not able to move because next spot not available, update moved to false
                                                    else {

                                                        pTrafficData->roadsArray[i]->carsArray[0]->moved = false;

                                                        // break to save time
                                                        break;
                                                    }
                                                }

                                                // if car not able to move because next spot not available, update moved to false
                                                else {

                                                    pTrafficData->roadsArray[i]->carsArray[0]->moved = false;
                                                }
                                            }
                                        }
                                    //}

                                    // if path ahead impossible
                                    /*else {

                                        printf("PATH AHEAD IMPOSSIBLE, BREAKING\n");

                                        break;
                                    }*/
                                }
                            }
                        //}

                        // if the car cannot move
                        /*else {

                            pTrafficData->roadsArray[i]->carsArray[0]->moved = false;
                        }*/
                    }
                }

                // MIDDLE POSITION IN ROAD
                else if (j > 0 && j < firstPosition) {

                    // if car in middle position
                    if (pTrafficData->roadsArray[i]->carsArray[j] != NULL) {

                        // if car can move
                        //if (pTrafficData->roadsArray[i]->carsArray[j]->moved == false) {

                            // if spot ahead available
                            if (pTrafficData->roadsArray[i]->carsArray[(j - 1)] == NULL) {

                                //printf("%d SPOT AHEAD AVAILABLE\n", pTrafficData->roadsArray[i]->carsArray[j]->destination);

                                // move car from current position to next position
                                pTrafficData->roadsArray[i]->carsArray[j - 1] = pTrafficData->roadsArray[i]->carsArray[j];

                                // since the car has been moved, set the moved boolean to true to prevent moving it again
                                //pTrafficData->roadsArray[i]->carsArray[j - 1]->moved = true;

                                // set the current position to NULL
                                pTrafficData->roadsArray[i]->carsArray[j] = NULL;
                            }

                            // if spot ahead not available
                            else {

                                //printf("%d NO SPOT AHEAD AVAILABLE\n", pTrafficData->roadsArray[i]->carsArray[j]->destination);

                                // car was not able to move, let it move next round
                                //pTrafficData->roadsArray[i]->carsArray[j]->moved = false;
                            }
                        //}

                        // if car can't move
                        /*else {

                            pTrafficData->roadsArray[i]->carsArray[j]->moved = false;
                        }*/
                    }
                }

                // FIRST POSITION IN ROAD
                else {

                    // if car in first position
                    if (pTrafficData->roadsArray[i]->carsArray[firstPosition] != NULL) {

                        // if car can move
                        //if (pTrafficData->roadsArray[i]->carsArray[firstPosition]->moved == false) {

                            // if spot after first position is available
                            if (pTrafficData->roadsArray[i]->carsArray[(firstPosition - 1)] == NULL ) {

                                // move car from the first position on the road to the next position
                                pTrafficData->roadsArray[i]->carsArray[firstPosition - 1] = pTrafficData->roadsArray[i]->carsArray[firstPosition];

                                // since the car has been moved, set the moved boolean to true to prevent moving it again
                                pTrafficData->roadsArray[i]->carsArray[firstPosition - 1]->moved = true;

                                // set the first position to NULL
                                pTrafficData->roadsArray[i]->carsArray[firstPosition] = NULL;
                            }

                            // if car cannot move because space ahead not available
                            else {

                                // car was not able to move, update moved to false
                                pTrafficData->roadsArray[i]->carsArray[firstPosition]->moved = false;
                            }
                        //}

                        // if car can't move
                        /*else {

                            pTrafficData->roadsArray[i]->carsArray[firstPosition]->moved = false;
                        }*/
                    }

                    // if no car in first position
                    else {

                        // set canDequeue to true by default
                        canDequeue = true;

                        // loop through every road to find roads that terminate at current intersection
                        for (k = 0; k < pTrafficData->totalRoads; k++) {

                            // if there is a road terminates at current intersection
                            if (pTrafficData->roadsArray[k]->intersectionsArray[1] == pTrafficData->roadsArray[i]->intersectionsArray[0]) {

                                // if the light is green
                                if (pTrafficData->roadsArray[k]->greenOn - 1 < timeStep % pTrafficData->roadsArray[k]->resetLight &&
                                    timeStep % pTrafficData->roadsArray[k]->resetLight < pTrafficData->roadsArray[k]->greenOff) {

                                    // if there is a car in the final position of road
                                    if (pTrafficData->roadsArray[k]->carsArray[0] != NULL) {

                                        // if the car can move, set flag
                                        //if (pTrafficData->roadsArray[k]->carsArray[0]->moved == false) {

                                            // prevent dequeueing of element as there is a car that must move onto road
                                            // first
                                            canDequeue = false;

                                            // break process to save time;
                                            break;
                                        //}
                                    }
                                }
                            }
                        }

                        // if car in queue and dequeue flag set to true
                        if (getNumElements(pTrafficData->roadsArray[i]->carsQueue) > 0 && canDequeue == true) {

                            // dequeue first car in the roadsArray[i] car queue
                            pTrafficData->roadsArray[i]->carsArray[firstPosition] = dequeue(pTrafficData->roadsArray[i]->carsQueue);

                            // since the car has been moved, set the moved boolean to true to prevent moving it again
                            pTrafficData->roadsArray[i]->carsArray[firstPosition]->moved = true;
                        }
                    }
                }
            }
        }


        //gridlock, test with 2 instead of boolean
        /*if (timeStep > 0) {

            gridLockFlag = true;

            for (i = 0; i < pTrafficData->totalRoads; i++) {

                // calculate if the traffic light is green or not
                if (pTrafficData->roadsArray[i]->greenOn - 1 < timeStep % pTrafficData->roadsArray[i]->resetLight &&
                    timeStep % pTrafficData->roadsArray[i]->resetLight < pTrafficData->roadsArray[i]->greenOff) {

                    isGreen = true;
                }

                else {

                    isGreen = false;
                }

                for (j = 0; j < pTrafficData->roadsArray[i]->roadLength; j++) {

                    if (pTrafficData->roadsArray[i]->carsArray[j] != NULL) {

                        if (pTrafficData->roadsArray[i]->carsArray[j]->moved == true && isGreen == true) {

                            gridLockFlag = false;

                            break;
                        }
                    }

                    else {

                        gridLockFlag = false;

                        break;
                    }
                }
            }
        }*/

        //printf("CARS LEFT: %d\n\n", totalCars);

        if (timeStep == 1000) {

            break;
        }

        //printf("\n");

        // every time the process loops, iterate timeStep to keep track of the current time until the process terminates
        timeStep++;
    }

    /* TODO: complete this function */

    /* Loop until all events processed and either all cars reached destination or gridlock has occurred */

    /* Update the state of every traffic light */

    /* Loop on events associated with this time step */

    /* First try to move cars through the next intersection */

    /* Second move waiting cars onto the end of each road if possible */

    /* Third move cars forward on every road (only those that haven't moved yet this time step) */
}



/* freeTrafficData
 * input: TrafficData* pTrafficData
 * output: N/A
 *
 * Free the data in the TrafficData variable
 */
void freeTrafficData(TrafficData* pTrafficData) { // TODO

    int i;

    free(pTrafficData->printSteps);

    // loop through all roads in pTrafficData and call freeRoad to free them of their memory
    for (i = 0; i < pTrafficData->totalRoads; i++) {

        freeRoad(pTrafficData->roadsArray[i]); // TODO free every Car* struct upon dequeue-ing
    }

    // free the 2D array of roadData** that holds all road data
    free(pTrafficData->roadsArray);

    for (i = 0; i < pTrafficData->totalEvents; i++) {

        freeEvent(pTrafficData->eventQueue->qFront->qt);
    }

    // free the priorityQueue* holding the list of elements // TODO free each Event* struct
    freePQ(pTrafficData->eventQueue);

    // free the Graph* holding everything
    freeGraph(pTrafficData->roadMap);

    // free the int** 2D array type holding traffic light data
    //free(pTrafficData->trafficLightTimes);

    // free the TrafficData* struct holding all the information
    free(pTrafficData);
}



int max(int a, int b) {

  if (a > b) {
  
    return a;
  }
  
  return b;
}

