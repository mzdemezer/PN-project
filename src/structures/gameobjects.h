#ifndef _INCLUDE_GAMEOBJECTS_H
#define _INCLUDE_GAMEOBJECTS_H

#include <allegro5/allegro.h>
#include "collstructures.h"

/**
    These structures enable objectivization
    by casting on different objects in
    relation to type. They also enable
    creating lists of objects that are
    logically connected with each other
    */

/**
    Order of first few fields in objects is
    deliberetly the same. This order simplifies
    getting access to them. This order
    SHOULD NOT BE CHANGED UNDER NO CIRCUMSTANCES!!!
    */

/**
    Primitive
    */
typedef enum primitive_object_type{
    potSEGMENT,
    potPOINT,
    potCIRCLE,
    potEXIT
}primitive_object_type;

typedef struct primitive_object_structure{
    primitive_object_type type;
    void *object_data;
}primitive_object;

typedef struct point{
    double x, y;
}point;

typedef struct line{
    double A, B, C,
          sqrtAB; //cache
}line;

typedef struct segment{
    point A, B;
    double ang; // direction of AB vector; to make collisions faster
    line line_equation; //for faster separation
}segment;

typedef struct circle{
    point center;
    double r;
}circle;

/**
    Movables
    */
typedef enum movable_object_type{
    motPLAYER,
    motSWITCH,
    motDOOR,
    motPARTICLE
}movable_object_type;

typedef struct movable_object{
    movable_object_type type;
    void* object_data;
    void (*draw)(void*, double dx, double dy);
    double (*r)(void*, double);
    double dx, dy;
    short int zones[4];
    collision_data *next_collision,
                    coll_with_fixed;
    coll_tree colls_with_mov;
}movable_object;

#define MAX_ACCELERATE 40
#define MAX_DECELERATE -20
#define THROTTLE 3
#define dANG 0.06981 //4 degrees
#define PLAYER_MASS 600
#define PLAYER_RADIUS 20
#define PLAYER_HP 1000
typedef struct movable_player{
    point center;
    double r, vx, vy;
    double ang;
    int engine_state;
    double mass,
          charge,
          r0;
    double HP;
}movable_player;

typedef enum switch_type{
    stSLIDE

}switch_type;

typedef struct connected_objects{
    int number_of_doors,
        number_of_switches,
        *doors,
        *switches;
}connected_objects;

typedef struct movable_switch{
    point center;
    double r, a, b, ang, fi0, fi02, wsp1, wsp2;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;

    int pos;
    double mass;
    switch_type type;
    connected_objects connected;

    double vx, vy;
}movable_switch;

typedef enum door_type{
    dt1WINGSLIDE
}door_type;

typedef struct movable_door{
    point center;
    double r, a, b, ang, fi0, fi02, wsp1, wsp2;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;

    int pos;
    double opening_time, mass;
    door_type type;

    double vx, vy;
}movable_door;

typedef struct movable_particle{
    point center;
    double r, mass, charge;
    ALLEGRO_COLOR color;

    double vx, vy;
    double surface_field;
}movable_particle;


/**
    Fixed
    */
typedef enum fixed_object_type{
    fotRECTANGLE,
    fotCIRCLE,
    fotSQUARE,
    fotENTRANCE,
    fotEXIT
}fixed_object_type;

typedef struct fixed_object_structure{
    fixed_object_type type;
    void* object_data;
    void (*draw)(void*);
    double (*r)(void*, double);
    short int zones[4];
}fixed_object;

typedef struct fixed_rectangle{
    point center;
    double r, a, b, ang, fi0, fi02, wsp1, wsp2;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
}fixed_rectangle;

typedef struct fixed_circle{
    point center;
    double r, ang;
    ALLEGRO_COLOR color;
}fixed_circle;

typedef struct fixed_square{
    point center;
    double r, bok, ang;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
}fixed_square;

typedef struct fixed_entrance{
    point center;
    double r, a, b, ang, fi0, fi02, wsp1, wsp2;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
}fixed_entrance;

typedef struct fixed_exit{
    point center;
    double r, a, b, ang, fi0, fi02, wsp1, wsp2;
    point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
}fixed_exit;

typedef struct prim_exit{
    fixed_exit *fixed_data;
    bool done;
}prim_exit;

typedef struct object_workshop{
    movable_player *new_player;
    fixed_rectangle *new_rectangle;
    fixed_circle *new_circle;
    fixed_square *new_square;
    fixed_entrance *new_entrance;
    fixed_exit *new_exit;
    movable_switch *new_switch;
    movable_door *new_door;
    movable_particle *new_particle;
}object_workshop;

/**
    Constructing
    */
point* construct_point();
segment* construct_segment(const point *A, const point *B);
prim_exit* construct_prim_exit(fixed_exit *);

void construct_circle(fixed_object *);
void construct_square(fixed_object *);
void construct_rectangle(fixed_object *);

void construct_player(movable_object *);
void construct_particle(movable_object *);
void construct_door(movable_object *);
void construct_switch(movable_object *);
void construct_movable(movable_object *);

double radius_square(void *object_data, double fi);
double radius_circle(void *object_data, double fi);
double radius_rectangle(void *object_data, double fi);
double radius_player(void *object_data, double fi);

#endif
