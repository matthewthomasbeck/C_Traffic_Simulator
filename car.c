// edited, turn this file in

#include "car.h"



Car* createCar(int carID, int stepAdded, int origin, int next, int destination) { // DONE

    Car* c = (Car*)malloc(sizeof(Car));

    c->carID = carID;
    c->stepAdded = stepAdded;
    c->origin = origin;
    c->next = next;
    c->destination = destination;

    c->moved = true;			/* Car moves on the time step it enters the simulation on */

    return c;
}

void freeCar( Car* c )
{
    free( c );
}
