#include <stdio.h>
#include <stdlib.h>
#include "zone.h"
#include "../mathematics.h"

void construct_zone(struct zone *zn){
    RB_construct_tree(&zn->movable);
    zn->number_of_primitives = 0;
    zn->allocated = INITIAL_PRIMITIVES_PER_ZONE;
    zn->primitives = (short int*)malloc(sizeof(short int) * INITIAL_PRIMITIVES_PER_ZONE);
}

void clear_zone(struct zone *zn){
    zn->number_of_primitives = 0;
    clear_tree(&zn->movable);
}

void destroy_zone(struct zone *zn){
    free(zn->primitives);
    RB_destroy_tree(&zn->movable);
}

void get_zone(double x, double y, short int *zone){
    /**
        Closure: [0 ; SCREEN_BUFFER_HEIGHT) ---> [0 ; SCREEN_BUFFER_HEIGHT]
        */
    if(double_abs(x - SCREEN_BUFFER_HEIGHT) < eps){
        zone[0] = ZONE_FACTOR - 1;
    }else{
        zone[0] = (short int)((int)x / ZONE_SIZE);
        if(x < 0){
            zone[0] -= 1;
        }
    }
    if(double_abs(y - SCREEN_BUFFER_HEIGHT) < eps){
        zone[1] = ZONE_FACTOR - 1;
    }else{
        zone[1] = (short int)((int)y / ZONE_SIZE);
        if(y < 0){
            zone[1] -= 1;
        }
    }
}

void get_zone_for_object(double x, double y, double dx, double dy, double r0, short int *zone){
    dx = double_abs(dx) + r0;
    dy = double_abs(dy) + r0;
    get_zone(x - dx, y - dy, zone);
    get_zone(x + dx, y + dy, zone + 2);
    if(zone[0] < 0){
        zone[0] = 0;
    }
    if(zone[1] < 0){
        zone[1] = 0;
    }
    if(zone[2] >= ZONE_FACTOR){
        zone[2] = ZONE_FACTOR - 1;
    }
    if(zone[3] >= ZONE_FACTOR){
        zone[3] = ZONE_FACTOR - 1;
    }
}

void add_primitive_to_zone(struct zone* zone, short int key){
    if(zone->number_of_primitives >= zone->allocated){
        zone->allocated *= 2;
        zone->primitives = (short int*)realloc(zone->primitives, sizeof(short int) * zone->allocated);
    }

    zone->primitives[zone->number_of_primitives] = key;
    zone->number_of_primitives += 1;
}
