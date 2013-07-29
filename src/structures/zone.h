#ifndef _INCLUDE_ZONE_H
#define _INCLUDE_ZONE_H

#include "RBtree.h"

/**
    Plane of a level is chopped into
    ZONES, so that collision algorithm
    for given object wouldn't have to run
    tests with every other, but only with
    ones that share zones with it.
    */
typedef struct zone{
    RB_tree movable;
    int number_of_primitives, allocated;
    short int *primitives;
}zone;

#define INITIAL_PRIMITIVES_PER_ZONE 64

void construct_zone(zone *zn);
void clear_zone(zone *zn);
void destroy_zone(zone *zn);
void get_zone(long double x, long double y, short int *zn);
void get_zone_for_object(long double x, long double y, long double dx, long double dy, long double r0, short int *zn);
void add_primitive_to_zone(zone* zn, short int key);

#endif
