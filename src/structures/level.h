#ifndef _INCLUDE_LEVEL_H
#define _INCLUDE_LEVEL_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "zone.h"
#include "collstructures.h"
#include "gameobjects.h"
#include "../structures.h"

#define DEFAULT_FLUID_DENSITY 0.001

#define ACC_2nd_DIM NumOfThreads + 2
typedef struct move_arrays{
    double x, y,
          vx, vy,
          ax[ACC_2nd_DIM],
          ay[ACC_2nd_DIM];
}move_arrays;

typedef struct score_struct{
    int score,
        total_score,
        level_number;
}score_struct;


/**
    level structure
    */
#define INITIAL_ALLOCATED_MOVABLE 256
#define INITIAL_ALLOCATED_FIXED 1024
#define INITIAL_ALLOCATED_PRIMITIVE 8192
#define INITIAL_OBJECT_COLLISION_QUEUE_SIZE 1024
typedef struct level_data{
    score_struct score;
    int number_of_movable_objects,
        number_of_fixed_objects,
        number_of_primitive_objects,
        allocated_movable,
        allocated_fixed,
        allocated_primitive;
    primitive_object *primitive_objects;
    fixed_object *fixed_objects;
    movable_object *movable_objects;

    movable_player *player;

    zone zones[ZONE_FACTOR][ZONE_FACTOR];
    collision_heap collision_queue;
    coll_tree dirty_tree;

    ALLEGRO_BITMAP *background;
    char filename[256];
    double start_time, sum_time,
           last_time, dt;
    move_arrays *acc;
    double dens, wind_vx, wind_vy;

    bool at_exit,
         victory;
}level_data;

void construct_level(level_data *level);
void initialize_level(level_data *level);
void destroy_level(level_data *level);
void clear_level(level_data *level);

/**
    Scores
    */
void new_scores(score_struct *score);
score_struct clear_score(score_struct *score);

/**
    Dynamic arrays for game objects
    */

void add_primitive_object(level_data *level, primitive_object_type, void* new_object_data);
void delete_primitive_object(primitive_object *);
void clear_primitive_object_list(level_data *level);

void add_fixed_object(level_data *level, fixed_object_type, void* new_object_data);
void delete_fixed_object(fixed_object *);
void clear_fixed_object_list(level_data *level);

void add_movable_object(level_data *levelData, movable_object_type NewObjectType, void* new_object_data);
void delete_movable_object(movable_object *);
void clear_movable_object_list(level_data *level);

/**
    Zones matrix
    */
void construct_zones(level_data *level);
void destroy_zones(level_data *level);
void clear_zones(level_data *level);

void initialize_zones_with_movable(level_data *level, short int *zones, short int index);
void change_zones_for_movable(level_data *level, short int index, double t);
bool get_outer_zones_of_segment(const point *A, const point *B, short int *zones);
void normalize_segment_zones(short int *zones);

/**
    Adding
    */
void add_segment(level_data *level, const point *A, const point *B);
void add_borders(level_data *level);
void add_point(level_data *level, point *A);
void add_circle(level_data *level, double r, point center);

void add_square(level_data *level, fixed_square *square);
void add_rectangle(level_data *level, fixed_rectangle *rectangle);
void add_exit(level_data *level, fixed_exit *ex);

#endif
