#include <stdlib.h>
#include "zone.h"
#include "../structures.h"
#include "../mathematics.h"

void construct_zone(zone *zn){
    RB_construct_tree(&zn->movable);
    zn->number_of_primitives = 0;
    zn->allocated = INITIAL_PRIMITIVES_PER_ZONE;
    zn->primitives = (short int*)malloc(sizeof(short int) * INITIAL_PRIMITIVES_PER_ZONE);
}

void clear_zone(zone *zn){
    zn->number_of_primitives = 0;
    clear_tree(&zn->movable);
}

void destroy_zone(zone *zn){
    free(zn->primitives);
    RB_destroy_tree(&zn->movable);
}

void get_zone(long double x, long double y, short int *zn){
    /**
        Closure: [0 ; SCREEN_BUFFER_HEIGHT) ---> [0 ; SCREEN_BUFFER_HEIGHT]
        */
    if(double_abs(x - SCREEN_BUFFER_HEIGHT) < eps){
        zn[0] = ZONE_FACTOR - 1;
    }else{
        zn[0] = (short int)((int)x / ZONE_SIZE);
        if(x < 0){
            zn[0] -= 1;
        }
    }
    if(double_abs(y - SCREEN_BUFFER_HEIGHT) < eps){
        zn[1] = ZONE_FACTOR - 1;
    }else{
        zn[1] = (short int)((int)y / ZONE_SIZE);
        if(y < 0){
            zn[1] -= 1;
        }
    }
}

void get_zone_for_object(long double x, long double y, long double dx, long double dy, long double r0, short int *zn){
    dx = double_abs(dx) + r0;
    dy = double_abs(dy) + r0;
    get_zone(x - dx, y - dy, zn);
    get_zone(x + dx, y + dy, zn + 2);
    if(zn[0] < 0){
        zn[0] = 0;
    }
    if(zn[1] < 0){
        zn[1] = 0;
    }
    if(zn[2] >= ZONE_FACTOR){
        zn[2] = ZONE_FACTOR - 1;
    }
    if(zn[3] >= ZONE_FACTOR){
        zn[3] = ZONE_FACTOR - 1;
    }
}

void add_primitive_to_zone(zone* zn, short int key){
    if(zn->number_of_primitives >= zn->allocated){
        zn->allocated *= 2;
        zn->primitives = (short int*)realloc(zn->primitives, sizeof(short int) * zn->allocated);
    }

    zn->primitives[zn->number_of_primitives] = key;
    zn->number_of_primitives += 1;
}
