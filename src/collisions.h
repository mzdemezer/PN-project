#ifndef _INCLUDE_COLLISIONS_H
#define _INCLUDE_COLLISIONS_H

#define MAX_COLLISIONS_PER_TURN 1600

#define PLAYER_TO_PLAYER_RESTITUTION 1
#define PLAYER_TO_PARTICLE_RESTITUTION 1
#define PARTICLE_TO_PARTICLE_RESTITUTION 1
#define PLAYER_TO_WALL_RESTITUTION 0.8
#define PARTICLE_TO_WALL_RESTITUTION 0.8

#define PLAYER_DAMAGE_MULTIPLIER 0.0005

#include "structures.h"

/**
    Administration
    */
void collide(level_data *level, short int who, short int with, bool with_movable, double dt);
void coll_clear_trash(level_data *level, coll_node *node, coll_node *nil);
void move_objects(level_data *level, double t);
void collision_min_for_object(level_data *level, short int who);
void find_next_collision(level_data *level, short int index,
                         fast_read_set *primitive_done, fast_read_set *movable_done, double time_passed);
collision_data get_collision_with_primitive(movable_object *who, primitive_object *with_what);
collision_data get_collision_with_movable(movable_object *who, movable_object *with_whom);

void get_and_check_mov_coll_if_valid(level_data *level, short int who, short int with, double time_passed);
void for_each_higher_check_collision(level_data *level, fast_read_set *movable_done,
                                     short int who, RB_node *node, RB_node *nil, double time_passed);
void in_order_check_collision(level_data *level, fast_read_set *movable_done,
                              short int who, RB_node *node, RB_node *nil, double time_passed);

/**
    Detecting
    */
double check_collision_between_two_balls(double x, double y, double dx, double dy, double d);
double check_collision_between_ball_and_segment(double x, double y, double dx, double dy, double r, segment *seg);
double check_exit(double dx, double dy, fixed_exit *ex);

/**
    Colliding
    */
void get_velocities_after_two_balls_collision(double *v1x, double *v1y, double *v2x, double *v2y,
                                              double dx, double dy, double m1, double m2, double restitution);
void get_velocity_after_ball_to_fixed_ball_collision(double *vx, double *vy, double dx, double dy, double restitution);

void get_velocity_after_ball_to_wall_collision(double *vx, double *vy, segment *seg, double restitution);

/**
    Separating
    */
void separate_two_balls(double *x1, double *y1, double m1, double *x2, double *y2, double m2, double d);
void separate_ball_from_fixed_ball(double *x1, double *y1, double x2, double y2, double d);
void separate_ball_from_segment(double *x, double *y, double d, segment *seg);

#endif
