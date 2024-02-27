// edited, turn this file in

#include "road.h"



RoadData* createRoad(int fromIntersection, int toIntersection, int roadLength, int greenOn, int greenOff, int resetLight) { // DONE

  RoadData* road = malloc(sizeof(*road)); // initialize a road

  road->roadLength = roadLength;
  road->greenOn = greenOn;
  road->greenOff = greenOff;
  road->resetLight = resetLight;
  road->intersectionsArray[0] = fromIntersection; // first intersection (vertex) in the road (edge)
  road->intersectionsArray[1] = toIntersection; // second intersection (vertex) in the road (edge)

  // array that is equal to the length of the road roadLength that is filled with cars and null spaces
  road->carsArray = (Car**)malloc(sizeof(Car)*roadLength);

  road->carsQueue = createQueue(); // creates a queue for cars to wait in and inputs a car should an event happen

  return road; // return the built road
}



void freeRoad(RoadData* road) { // DONE

    int i;

    // loop length of carsArray to free all the individual cars
    for (i = 0; i < road->roadLength; i++) {

        freeCar(road->carsArray[i]);
    }

    // free the Car** 2D array that holds the Car* structs
    free(road->carsArray);

    /*for (i = 0; i < getNumElements(road->carsQueue); i++) {

        freeCar(road->carsQueue->qFront->qt);
    }*/

    // free the Queue* struct holding the queue
    freeQueue(road->carsQueue);

    // free the roadData* struct holding the road
    free(road);
}



bool updateTrafficLight(int timeStep, RoadData* road) { // DONE

    // if the mod timeStep and resetLight is in between the last red (greenOn - 1) and the next red...
    if (road->greenOn - 1 < timeStep % road->resetLight &&
        timeStep % road->resetLight < road->greenOff) {

        // light is green, set to true
        return true;
    }

    // if the mod timeStep and resetLight is not in between the last red (greenOn - 1) and the next red...
    else {

        // light is red, set to false
        return false;
    }
}
