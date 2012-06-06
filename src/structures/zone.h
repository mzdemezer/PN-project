#ifndef _INCLUDE_ZONE_H
#define _INCLUDE_ZONE_H

#include "../structures.h"

/**
    Plane of a level is chopped into
    ZONES, so that collision algorithm
    for given object wouldn't have to run
    tests with every other, but only with
    ones that share zones with it.
    */
struct zone{
    RB_tree movable;
    int number_of_primitives, allocated;
    short int *primitives;
};

#define INITIAL_PRIMITIVES_PER_ZONE 64

void construct_zone(struct zone *zn);
void clear_zone(struct zone *zn);
void destroy_zone(struct zone *zn);
void get_zone(double x, double y, short int *zone);
void get_zone_for_object(double x, double y, double dx, double dy, double r0, short int *zone);
void add_primitive_to_zone(struct zone* zone, short int key);

#endif
