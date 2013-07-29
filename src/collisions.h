#ifndef _INCLUDE_COLLISIONS_H
#define _INCLUDE_COLLISIONS_H

#define MAX_COLLISIONS_PER_TURN 1600

#define PLAYER_TO_PLAYER_RESTITUTION 1
#define PLAYER_TO_PARTICLE_RESTITUTION 1
#define PARTICLE_TO_PARTICLE_RESTITUTION 1
#define PLAYER_TO_WALL_RESTITUTION 0.8
#define PARTICLE_TO_WALL_RESTITUTION 0.8
#define SHIELD_TO_WALL_RESTITUTION 0.5

#define PLAYER_DAMAGE_MULTIPLIER 0.001
#define PLAYER_WITH_PARTICLE_DAMAGE_MULTIPLIER 12
#define PLAYER_WITH_PLAYER_DAMAGE_MULTIPLIER 2
#define PLAYER_WITH_WALL_DAMAGE_MULTIPLIER 0.7
#define PLAYER_SHIELD_DAMAGE_MULITIPLIER 0.1

#include "structures.h"

/**
    Administration
    */
void collide(level_data *level, short int who, short int with, bool with_movable, long double dt);
void coll_clear_trash(level_data *level, coll_node *node, coll_node *nil);
void move_objects(level_data *level, long double t);
void collision_min_for_object(level_data *level, short int who);
void find_next_collision(level_data *level, short int index,
                         fast_read_set *primitive_done, fast_read_set *movable_done, long double time_passed);
collision_data get_collision_with_primitive(movable_object *who, primitive_object *with_what);
collision_data get_collision_with_movable(movable_object *who, movable_object *with_whom);

void get_and_check_mov_coll_if_valid(level_data *level, short int who, short int with, long double time_passed);
void for_each_higher_check_collision(level_data *level, fast_read_set *movable_done,
                                     short int who, RB_node *node, RB_node *nil, long double time_passed);
void in_order_check_collision(level_data *level, fast_read_set *movable_done,
                              short int who, RB_node *node, RB_node *nil, long double time_passed);

/**
    Detecting
    */
long double check_collision_between_two_balls(long double x, long double y, long double dx, long double dy,
                                         long double d, long double dr);
long double check_collision_between_ball_and_segment(long double x, long double y, long double dx, long double dy, long double r, long double dr, segment *seg);
long double check_exit(long double dx, long double dy, fixed_exit *ex);

/**
    Colliding
    */
void get_velocities_after_two_balls_collision(long double *v1x, long double *v1y, long double *v2x, long double *v2y,
                                              long double dx, long double dy, long double m1, long double m2, long double dr, long double restitution);
void get_velocity_after_ball_to_fixed_ball_collision(long double *vx, long double *vy, long double dx, long double dy, long double dr, long double restitution);

void get_velocity_after_ball_to_wall_collision(long double *vx, long double *vy, segment *seg, long double dr, long double restitution);

/**
    Separating
    */
bool separate_two_balls(long double *x1, long double *y1, long double m1, long double *x2, long double *y2, long double m2, long double d);
bool separate_ball_from_fixed_ball(long double *x1, long double *y1, long double x2, long double y2, long double d);
bool separate_ball_from_segment(long double *x, long double *y, long double d, segment *seg);


void check_dx_dy(long double *dx, long double *dy);

#endif
