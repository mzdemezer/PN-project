#include <stdlib.h>
#include <math.h>
#include "gameobjects.h"
#include "../structures.h"
#include "../mathematics.h"
#include "../draw.h"

/**
    Constructors
    Primitives if needed must be added
    explicitly outside constructors
    */

void construct_circle(fixed_object *object){
    #define Data ((fixed_circle*)object_data)
    object->draw = draw_circle;
    object->r = radius_circle;
    #undef Data
}

void construct_square(fixed_object *object){
    #define Data ((fixed_square*)object->object_data)
    object->draw = draw_square;
    object->r = radius_square;
    Data->r = Data->bok * SQRT2 / 2;
    double fi = PI4 + Data->ang;
    Data->v1 = (point*)malloc(sizeof(point));
    Data->v1->x = Data->center.x + Data->r * cos(fi);
    Data->v1->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v2 = (point*)malloc(sizeof(point));
    Data->v2->x = Data->center.x + Data->r * cos(fi);
    Data->v2->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v3 = (point*)malloc(sizeof(point));
    Data->v3->x = Data->center.x + Data->r * cos(fi);
    Data->v3->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v4 = (point*)malloc(sizeof(point));
    Data->v4->x = Data->center.x + Data->r * cos(fi);
    Data->v4->y = Data->center.y + Data->r * sin(fi);
    #undef Data
}

/**
    This sets the values for a rectangle,
    but due to order of variables it works also for
    entrances, exits, switches and doors
    (which of course need further initialization, btw);
    SO DON'T ANYONE DARE CHANGE THE ORDER!!!
    */
void construct_rectangle(fixed_object *object){
    #define Data ((fixed_rectangle*)object->object_data)
    double fi;
    object->draw = draw_rectangle;
    object->r = radius_rectangle;

    if(Data->b > Data->a){
        fi = Data->a;
        Data->a = Data->b;
        Data->b = fi;
        Data->ang = norm(Data->ang + PIpol);
    }

    Data->fi0 = double_abs(2 * atan(Data->b / Data->a));
    Data->fi02 = Data->fi0 * 0.5;
    Data->wsp1 = tan(PIpol + Data->fi02);
    Data->wsp2 = tan(Data->fi02);

    Data->r = Data->b / (sin(Data->fi02) * 2);

    fi = PIpol - Data->ang + Data->fi02;
    Data->v1 = (point*)malloc(sizeof(point));
    Data->v1->x = Data->center.x + Data->r * cos(fi);
    Data->v1->y = Data->center.y + Data->r * sin(fi);
    fi += PI;
    Data->v3 = (point*)malloc(sizeof(point));
    Data->v3->x = Data->center.x + Data->r * cos(fi);
    Data->v3->y = Data->center.y + Data->r * sin(fi);
    fi = PIpol - Data->ang - Data->fi02;
    Data->v2 = (point*)malloc(sizeof(point));
    Data->v2->x = Data->center.x + Data->r * cos(fi);
    Data->v2->y = Data->center.y + Data->r * sin(fi);
    fi += PI;
    Data->v4 = (point*)malloc(sizeof(point));
    Data->v4->x = Data->center.x + Data->r * cos(fi);
    Data->v4->y = Data->center.y + Data->r * sin(fi);
    #undef Data
}

void construct_door(movable_object *object){
    #define Data ((movable_door*)(object->object_data))
    construct_rectangle((fixed_object*)object);

    object->draw = draw_door;
    Data->vx = 0;
    Data->vy = 0;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, object->zones);

    #undef Data
}

void construct_switch(movable_object *object){
    #define Data ((movable_switch*)(object->object_data))
    construct_rectangle((fixed_object*)object);

    object->draw = draw_switch;
    Data->vx = 0;
    Data->vy = 0;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, object->zones);

    #undef Data
}

void construct_player(movable_object *object){
    #define Data ((movable_player*)(object->object_data))

    object->draw = draw_player;
    object->r = radius_player;
    Data->vx = 0;
    Data->vy = 0;
    Data->engine_state = 0;
    Data->mass = PLAYER_MASS;
    Data->charge = 0;
    Data->r = PLAYER_RADIUS;
    Data->r0 = PLAYER_RADIUS;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, object->zones);
    #undef Data
}

void construct_particle(movable_object *object){
    #define Data ((movable_particle*)(object->object_data))
    object->draw = draw_particle;
    object->r = radius_circle;
    Data->vx = 0;
    Data->vy = 0;
    Data->surface_field = Data->r * Data->r * PI;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, object->zones);
    #undef Data
}

void construct_movable(movable_object *object){
    object->dx = 0;
    object->dy = 0;
    coll_construct_tree(&object->colls_with_mov);
    object->next_collision = &object->colls_with_mov.nil->key;

    switch(object->type){
        case motPLAYER:
            construct_player(object);
            break;
        case motPARTICLE:
            construct_particle(object);
            break;
        case motDOOR:
            construct_door(object);
            break;
        case motSWITCH:
            construct_switch(object);
            break;
    }
}


/**
    Radius functions
    */
double radius_square(void *object_data, double fi){
    #define Data ((fixed_square*)object_data)
    return square_equation(Data->r, fi - Data->ang);
    #undef Data
}

double radius_circle(void *object_data, double fi){
    return ((fixed_circle*)object_data)->r;
}

double radius_player(void *object_data, double fi){
    return PLAYER_RADIUS;
}

double radius_rectangle(void *object_data, double fi){
    #define Data ((fixed_rectangle*)object_data)
    return rectangle_equation(Data->r, fi - Data->ang, Data->fi0, Data->fi02, Data->wsp1, Data->wsp2);
    #undef Data
}
