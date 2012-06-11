#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "collisions.h"
#include "structures.h"
#include "mathematics.h"
#include "game.h"

/**
    Private methods
    */
void player_get_dx_dy(movable_object *obj, double dt);
void particle_get_dx_dy(movable_object *obj, double dt);
double count_hp_loss(double d_vx, double d_vy);
void count_hp_and_shield(movable_player *player, double d_vx, double d_vy, float multiplier);

/**
    Code
    */
void collide(level_data *level, short int who, short int with, bool with_movable, double dt){
    #define WHO_PLAYER ((movable_player*)level->movable_objects[who].object_data)
    #define WHO_PARTICLE ((movable_particle*)level->movable_objects[who].object_data)
    #define WITH_PLAYER ((movable_player*)level->movable_objects[with].object_data)
    #define WITH_PARTICLE ((movable_particle*)level->movable_objects[with].object_data)
    #define WITH_POINT ((point*)level->primitive_objects[with].object_data)
    #define WITH_SEGMENT ((segment*)level->primitive_objects[with].object_data)
    #define WITH_CIRCLE ((circle*)level->primitive_objects[with].object_data)
    #define WITH_EXIT ((prim_exit*)level->primitive_objects[with].object_data)
    double player_d_vx,
           player_d_vy,
           with_d_vx,
           with_d_vy,
           dr,
           mult;
    if(with_movable){
        switch(level->movable_objects[who].type){
            case motPLAYER:
                if(WHO_PLAYER->shield + WHO_PLAYER->shield_push > WHO_PLAYER->r0){
                    dr = WHO_PLAYER->shield_push;
                }else{
                    dr = 0;
                }
                player_d_vx = WHO_PLAYER->vx;
                player_d_vy = WHO_PLAYER->vy;
                switch(level->movable_objects[with].type){
                    case motPLAYER:
                        mult = PLAYER_WITH_PLAYER_DAMAGE_MULTIPLIER;
                        with_d_vx = WITH_PLAYER->vx;
                        with_d_vy = WITH_PLAYER->vy;
                        if(WITH_PLAYER->shield + WITH_PLAYER->shield_push > WITH_PLAYER->r0){
                            dr += WHO_PLAYER->shield_push;
                        }
                        dr *= dt;
                        separate_two_balls(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y, WHO_PLAYER->mass,
                                           &WITH_PLAYER->center.x, &WITH_PLAYER->center.y, WITH_PLAYER->mass,
                                           WHO_PLAYER->r + WITH_PLAYER->r + dr);
                        get_velocities_after_two_balls_collision(&(WHO_PLAYER->vx), &(WHO_PLAYER->vy),
                                                                 &(WITH_PLAYER->vx), &(WITH_PLAYER->vy),
                                                                 WITH_PLAYER->center.x - WHO_PLAYER->center.x,
                                                                 WITH_PLAYER->center.y - WHO_PLAYER->center.y,
                                                                 WHO_PLAYER->mass, WITH_PLAYER->mass, dr,
                                                                 PLAYER_TO_PLAYER_RESTITUTION);
                        player_get_dx_dy(&level->movable_objects[who], dt);
                        player_get_dx_dy(&level->movable_objects[with], dt);
                        with_d_vx = WITH_PLAYER->vx - with_d_vx;
                        with_d_vy = WITH_PLAYER->vy - with_d_vy;
                        count_hp_and_shield(WITH_PLAYER, with_d_vx, with_d_vy, mult);
                        break;
                    case motPARTICLE:
                        mult = PLAYER_WITH_PARTICLE_DAMAGE_MULTIPLIER;
                        dr *= dt;
                        separate_two_balls(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y, WHO_PLAYER->mass,
                                           &WITH_PARTICLE->center.x, &WITH_PARTICLE->center.y, WITH_PARTICLE->mass,
                                           WHO_PLAYER->r + WITH_PARTICLE->r + dr);
                        get_velocities_after_two_balls_collision(&(WHO_PLAYER->vx), &(WHO_PLAYER->vy),
                                                                 &(WITH_PARTICLE->vx), &(WITH_PARTICLE->vy),
                                                                 WITH_PARTICLE->center.x - WHO_PLAYER->center.x,
                                                                 WITH_PARTICLE->center.y - WHO_PLAYER->center.y,
                                                                 WHO_PLAYER->mass, WITH_PARTICLE->mass, dr,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        player_get_dx_dy(&level->movable_objects[who], dt);
                        particle_get_dx_dy(&level->movable_objects[with], dt);
                        break;
                    default:
                        break;
                }
                player_d_vx = WHO_PLAYER->vx - player_d_vx;
                player_d_vy = WHO_PLAYER->vy - player_d_vy;
                count_hp_and_shield(WHO_PLAYER, player_d_vx, player_d_vy, mult);
                break;
            case motPARTICLE:
                switch(level->movable_objects[with].type){
                    case motPLAYER:
                        with_d_vx = WITH_PLAYER->vx;
                        with_d_vy = WITH_PLAYER->vy;
                        if(WITH_PLAYER->shield + WITH_PLAYER->shield_push > WITH_PLAYER->r0){
                            dr = WITH_PLAYER->shield_push;
                        }else{
                            dr = 0;
                        }
                        dr *= dt;
                        separate_two_balls(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y, WHO_PARTICLE->mass,
                                           &WITH_PLAYER->center.x, &WITH_PLAYER->center.y, WITH_PLAYER->mass,
                                           WHO_PARTICLE->r + WITH_PLAYER->r + dr);
                        get_velocities_after_two_balls_collision(&(WHO_PARTICLE->vx), &(WHO_PARTICLE->vy),
                                                                 &(WITH_PLAYER->vx), &(WITH_PLAYER->vy),
                                                                 WITH_PLAYER->center.x - WHO_PARTICLE->center.x,
                                                                 WITH_PLAYER->center.y - WHO_PARTICLE->center.y,
                                                                 WHO_PARTICLE->mass, WITH_PLAYER->mass, dr,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        particle_get_dx_dy(&level->movable_objects[who], dt);
                        player_get_dx_dy(&level->movable_objects[with], dt);
                        with_d_vx = WITH_PLAYER->vx - with_d_vx;
                        with_d_vy = WITH_PLAYER->vy - with_d_vy;
                        count_hp_and_shield(WITH_PLAYER, with_d_vx, with_d_vy, PLAYER_WITH_PARTICLE_DAMAGE_MULTIPLIER);
                        break;
                    case motPARTICLE:
                        separate_two_balls(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y, WHO_PARTICLE->mass,
                                           &WITH_PARTICLE->center.x, &WITH_PARTICLE->center.y, WITH_PARTICLE->mass,
                                           WHO_PARTICLE->r + WITH_PARTICLE->r);
                        get_velocities_after_two_balls_collision(&(WHO_PARTICLE->vx), &(WHO_PARTICLE->vy),
                                                                 &(WITH_PARTICLE->vx), &(WITH_PARTICLE->vy),
                                                                 WITH_PARTICLE->center.x - WHO_PARTICLE->center.x,
                                                                 WITH_PARTICLE->center.y - WHO_PARTICLE->center.y,
                                                                 WHO_PARTICLE->mass, WITH_PARTICLE->mass, 0,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        particle_get_dx_dy(&level->movable_objects[who], dt);
                        particle_get_dx_dy(&level->movable_objects[with], dt);
                        break;
                    default:
                        break;
                }

                break;
            default:
                break;
        }
    }else{
        switch(level->movable_objects[who].type){
            case motPLAYER:
                player_d_vx = WHO_PLAYER->vx;
                player_d_vy = WHO_PLAYER->vy;
                if(WHO_PLAYER->shield + WHO_PLAYER->shield_push > WHO_PLAYER->r0){
                    dr = WHO_PLAYER->shield_push;
                }else{
                    dr = 0;
                }
                dr *= dt;
                switch(level->primitive_objects[with].type){
                    case potPOINT:
                        separate_ball_from_fixed_ball(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y,
                                                      WITH_POINT->x, WITH_POINT->y, WHO_PLAYER->r + dr);
                        if(WHO_PLAYER->shield <= WHO_PLAYER->r0){
                            get_velocity_after_ball_to_fixed_ball_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                            WITH_POINT->x - WHO_PLAYER->center.x,
                                                                            WITH_POINT->y - WHO_PLAYER->center.y,
                                                                            0, PLAYER_TO_WALL_RESTITUTION);
                        }else{
                            get_velocity_after_ball_to_fixed_ball_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                            WITH_POINT->x - WHO_PLAYER->center.x,
                                                                            WITH_POINT->y - WHO_PLAYER->center.y,
                                                                            dr, SHIELD_TO_WALL_RESTITUTION);
                        }
                        break;
                    case potSEGMENT:
                        separate_ball_from_segment(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y, WHO_PLAYER->r + dr * dt, WITH_SEGMENT);
                        if(WHO_PLAYER->shield <= WHO_PLAYER->r0){
                            get_velocity_after_ball_to_wall_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                      WITH_SEGMENT, dr, PLAYER_TO_WALL_RESTITUTION);
                        }else{
                            get_velocity_after_ball_to_wall_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                      WITH_SEGMENT, dr, SHIELD_TO_WALL_RESTITUTION);
                        }
                        break;
                    case potCIRCLE:
                        separate_ball_from_fixed_ball(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y,
                                                      WITH_CIRCLE->center.x, WITH_CIRCLE->center.y,
                                                      WHO_PLAYER->r + WITH_CIRCLE->r + dr * dt);
                        if(WHO_PLAYER->shield <= WHO_PLAYER->r0){
                            get_velocity_after_ball_to_fixed_ball_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                            WITH_CIRCLE->center.x - WHO_PLAYER->center.x,
                                                                            WITH_CIRCLE->center.y - WHO_PLAYER->center.y,
                                                                            dr, PLAYER_TO_WALL_RESTITUTION);
                        }else{
                            get_velocity_after_ball_to_fixed_ball_collision(&WHO_PLAYER->vx, &WHO_PLAYER->vy,
                                                                            WITH_CIRCLE->center.x - WHO_PLAYER->center.x,
                                                                            WITH_CIRCLE->center.y - WHO_PLAYER->center.y,
                                                                            dr, SHIELD_TO_WALL_RESTITUTION);
                        }
                        break;
                    case potEXIT:
                        level->at_exit = true;
                        WITH_EXIT->done = true;
                        break;
                }
                player_get_dx_dy(&level->movable_objects[who], dt);
                player_d_vx = WHO_PLAYER->vx - player_d_vx;
                player_d_vy = WHO_PLAYER->vy - player_d_vy;
                count_hp_and_shield(WHO_PLAYER, player_d_vx, player_d_vy, PLAYER_WITH_WALL_DAMAGE_MULTIPLIER);
                break;
            case motPARTICLE:
                switch(level->primitive_objects[with].type){
                    case potPOINT:
                        separate_ball_from_fixed_ball(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y,
                                                      WITH_POINT->x, WITH_POINT->y, WHO_PARTICLE->r);
                        get_velocity_after_ball_to_fixed_ball_collision(&WHO_PARTICLE->vx, &WHO_PARTICLE->vy,
                                                                        WITH_POINT->x - WHO_PARTICLE->center.x,
                                                                        WITH_POINT->y - WHO_PARTICLE->center.y,
                                                                        0, PARTICLE_TO_WALL_RESTITUTION);
                        break;
                    case potSEGMENT:
                        separate_ball_from_segment(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y, WHO_PARTICLE->r, WITH_SEGMENT);
                        get_velocity_after_ball_to_wall_collision(&WHO_PARTICLE->vx, &WHO_PARTICLE->vy,
                                                                  WITH_SEGMENT, 0, PARTICLE_TO_WALL_RESTITUTION);
                        break;
                    case potCIRCLE:
                        separate_ball_from_fixed_ball(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y,
                                                      WITH_CIRCLE->center.x, WITH_CIRCLE->center.y,
                                                      WHO_PARTICLE->r + WITH_CIRCLE->r);
                        get_velocity_after_ball_to_fixed_ball_collision(&WHO_PARTICLE->vx, &WHO_PARTICLE->vy,
                                                                        WITH_CIRCLE->center.x - WHO_PARTICLE->center.x,
                                                                        WITH_CIRCLE->center.y - WHO_PARTICLE->center.y,
                                                                        0, PARTICLE_TO_WALL_RESTITUTION);
                        break;
                    default:
                        ;
                }
                particle_get_dx_dy(&level->movable_objects[who], dt);
                break;
            default:
                break;
        }
    }
    #undef WHO_PLAYER
    #undef WHO_PARTICLE
    #undef WITH_PLAYER
    #undef WITH_PARTICLE
    #undef WITH_POINT
    #undef WITH_SEGMENT
    #undef WITH_CIRCLE
    #undef WITH_EXIT
}

double count_hp_loss(double d_vx, double d_vy){
    d_vx = d_vx * d_vx + d_vy * d_vy;
    if( !(double_abs(d_vx) < eps) ){
        return d_vx * PLAYER_DAMAGE_MULTIPLIER;
    }else{
        return 0;
    }
}

void count_hp_and_shield(movable_player *player, double d_vx, double d_vy, float multiplier){
    d_vx = count_hp_loss(d_vx, d_vy) * multiplier;
    if(player->shield > player->r0){
        player->shield -= d_vx * PLAYER_SHIELD_DAMAGE_MULITIPLIER;
        if(player->shield <= eps){
            player->shield = 0;
        }
        player->shield_push = 0;
        get_player_radius(player);
    }else{
        player->HP -= d_vx;
    }
}

/**
    After collision the ould collision tree
    with movables for given object still exists,
    as well as information about collisin with
    the object, that are no longer valid.
    This function clears the tree with post-order
    deleting info.
    */
void coll_clear_trash(level_data *level, coll_node *node, coll_node *nil){
    if(node != nil){
        coll_clear_trash(level, node->left, nil);
        coll_clear_trash(level, node->right, nil);

        short int temp = node->key.with;
        node->key.with = node->key.who;
        node->key.who = temp;

        temp = coll_comp(&node->key, level->movable_objects[node->key.who].next_collision);
        coll_delete_node(&level->movable_objects[node->key.who].colls_with_mov, &node->key);
        if(temp == EQUAL){
            //mark dirty
            temp = node->key.who;
            if(node->key.with < node->key.who){
                node->key.who = node->key.with;
                node->key.with = temp;
            }
            coll_insert_node(&level->dirty_tree, &node->key);
            //find new min
            collision_min_for_object(level, temp);
            //push on queue //how to push so that one collision gets on queue only once?
            if(level->movable_objects[node->key.who].next_collision->time >= 0 &&
               level->movable_objects[node->key.who].next_collision->time <= 1){
                heap_insert(&level->collision_queue, level->movable_objects[node->key.who].next_collision);
            }
        }
        free(node);
    }
}

void move_objects(level_data *level, double t){
    int i;
    for(i = 0; i < level->number_of_movable_objects; ++i){
        ((point*)level->movable_objects[i].object_data)->x += level->movable_objects[i].dx * t;
        ((point*)level->movable_objects[i].object_data)->y += level->movable_objects[i].dy * t;
    }
}

void collision_min_for_object(level_data *level, short int who){
    if(level->movable_objects[who].colls_with_mov.root == level->movable_objects[who].colls_with_mov.nil){
        level->movable_objects[who].next_collision = &level->movable_objects[who].coll_with_fixed;
    }else{
        level->movable_objects[who].next_collision = &(coll_get_minimum(level->movable_objects[who].colls_with_mov.root,
                                                                              level->movable_objects[who].colls_with_mov.nil)->key);
    }
}

void find_next_collision(level_data *level, short int index,
                         fast_read_set *primitive_done, fast_read_set *movable_done, double time_passed){
    collision_data new_coll;
    int i, j, k;
    level->movable_objects[index].coll_with_fixed.time = EMPTY_COLLISION_TIME;
    for(i = level->movable_objects[index].zones[0]; i <= level->movable_objects[index].zones[2]; ++i){
        for(j = level->movable_objects[index].zones[1]; j <= level->movable_objects[index].zones[3]; ++j){
            for(k = 0; k < level->zones[i][j].number_of_primitives; ++k){
                if(!is_marked(primitive_done, level->zones[i][j].primitives[k])){
                    set_mark(primitive_done, level->zones[i][j].primitives[k]);
                    new_coll = get_collision_with_primitive(&level->movable_objects[index],
                                                            &level->primitive_objects[level->zones[i][j].primitives[k]]);
                    new_coll.time += time_passed;
                    if(new_coll.time >= 0 && new_coll.time <= 1){
                        if(new_coll.time < level->movable_objects[index].coll_with_fixed.time){
                            new_coll.who = index;
                            new_coll.with = level->zones[i][j].primitives[k];
                            level->movable_objects[index].coll_with_fixed = new_coll;
                        }
                    }
                }
            }
        }
    }
    reset_marks(primitive_done);

    set_mark(movable_done, index);
    for(i = level->movable_objects[index].zones[0]; i <= level->movable_objects[index].zones[2]; ++i){
        for(j = level->movable_objects[index].zones[1]; j <= level->movable_objects[index].zones[3]; ++j){
            in_order_check_collision(level, movable_done,
                                            index,
                                            level->zones[i][j].movable.root,
                                            level->zones[i][j].movable.nil,
                                            time_passed);
        }
    }
    reset_marks(movable_done);

    collision_min_for_object(level, index);

    //push on queue  //heap checks if who < with and does necessary exchanges
    if(level->movable_objects[index].next_collision->time <= 1 &&
       level->movable_objects[index].next_collision->time >= 0){
        heap_insert(&level->collision_queue, level->movable_objects[index].next_collision);
    }
}

collision_data get_collision_with_primitive(movable_object *who, primitive_object *with_what){
    #define WHO_PLAYER ((movable_player*)who->object_data)
    #define WHO_PARTICLE ((movable_particle*)who->object_data)
    #define WITH_POINT ((point*)with_what->object_data)
    #define WITH_SEGMENT ((segment*)(with_what->object_data))
    #define WITH_CIRCLE ((circle*)with_what->object_data)
    #define WITH_EXIT ((fixed_exit*)(((prim_exit*)with_what->object_data)->fixed_data))
    collision_data new_coll;
    double dr;
    new_coll.time = EMPTY_COLLISION_TIME;
    new_coll.with_movable = false;
    switch(who->type){
        case motPLAYER:
            if(WHO_PLAYER->shield + WHO_PLAYER->shield_push > WHO_PLAYER->r0){
                dr = WHO_PLAYER->shield_push;
            }else{
                dr = 0;
            }
            switch(with_what->type){
                case potPOINT:
                    new_coll.time = check_collision_between_two_balls(WITH_POINT->x - WHO_PLAYER->center.x,
                                                                      WITH_POINT->y - WHO_PLAYER->center.y,
                                                                      who->dx, who->dy, WHO_PLAYER->r, dr);
                    new_coll.time = double_min(new_coll.time, check_collision_between_two_balls(WITH_POINT->x - WHO_PLAYER->center.x,
                                                                      WITH_POINT->y - WHO_PLAYER->center.y,
                                                                      who->dx, who->dy, WHO_PLAYER->r0, 0));
                    break;
                case potSEGMENT:
                    new_coll.time = check_collision_between_ball_and_segment(WHO_PLAYER->center.x, WHO_PLAYER->center.y,
                                                                             who->dx, who->dy, WHO_PLAYER->r, dr, WITH_SEGMENT);
                    new_coll.time = double_min(new_coll.time, check_collision_between_ball_and_segment(WHO_PLAYER->center.x, WHO_PLAYER->center.y,
                                                                             who->dx, who->dy, WHO_PLAYER->r0, 0, WITH_SEGMENT));
                    break;
                case potCIRCLE:
                    new_coll.time = check_collision_between_two_balls(WITH_CIRCLE->center.x - WHO_PLAYER->center.x,
                                                                      WITH_CIRCLE->center.y - WHO_PLAYER->center.y,
                                                                      who->dx, who->dy, WHO_PLAYER->r + WITH_CIRCLE->r, dr);
                    new_coll.time = double_min(new_coll.time, check_collision_between_two_balls(WITH_CIRCLE->center.x - WHO_PLAYER->center.x,
                                                                      WITH_CIRCLE->center.y - WHO_PLAYER->center.y,
                                                                      who->dx, who->dy, WHO_PLAYER->r0 + WITH_CIRCLE->r, 0));
                    break;
                case potEXIT:
                    if(!((prim_exit*)with_what->object_data)->done){
                        new_coll.time = check_exit(WITH_EXIT->center.x - WHO_PLAYER->center.x,
                                                   WITH_EXIT->center.y - WHO_PLAYER->center.y,
                                                   WITH_EXIT);
                    }
                    break;
            }
            break;
        case motPARTICLE:
            switch(with_what->type){
                case potPOINT:
                    new_coll.time = check_collision_between_two_balls(WITH_POINT->x - WHO_PARTICLE->center.x,
                                                                      WITH_POINT->y - WHO_PARTICLE->center.y,
                                                                      who->dx, who->dy, WHO_PARTICLE->r, 0);
                    break;
                case potSEGMENT:
                    new_coll.time = check_collision_between_ball_and_segment(WHO_PARTICLE->center.x, WHO_PARTICLE->center.y,
                                                                             who->dx, who->dy, WHO_PARTICLE->r, 0, WITH_SEGMENT);
                    break;
                case potCIRCLE:
                    new_coll.time = check_collision_between_two_balls(WITH_CIRCLE->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_CIRCLE->center.y - WHO_PARTICLE->center.y,
                                                                      who->dx, who->dy, WHO_PARTICLE->r + WITH_CIRCLE->r, 0);
                    break;
                default:
                    ;
            }
            break;
        default:
            break;
    }
    #undef WHO_PLAYER
    #undef WHO_PARTICLE
    #undef WITH_POINT
    #undef WITH_SEGMENT
    #undef WITH_CIRCLE
    #undef WITH_EXIT
    return new_coll;
}

collision_data get_collision_with_movable(movable_object *who, movable_object *with_whom){
    #define WHO_PLAYER ((movable_player*)who->object_data)
    #define WITH_PLAYER ((movable_player*)with_whom->object_data)
    #define WHO_PARTICLE ((movable_particle*)who->object_data)
    #define WITH_PARTICLE ((movable_particle*)with_whom->object_data)
    collision_data new_coll;
    new_coll.time = EMPTY_COLLISION_TIME;
    double dr;
    switch(who->type){
        case motPLAYER:
            if(WHO_PLAYER->shield + WHO_PLAYER->shield_push > WHO_PLAYER->r0){
                dr = WHO_PLAYER->shield_push;
            }else{
                dr = 0;
            }
            switch(with_whom->type){
                case motPLAYER:
                    if(WITH_PLAYER->shield + WITH_PLAYER->shield_push > WITH_PLAYER->r0){
                        dr += WITH_PLAYER->shield_push;
                    }
                    new_coll.time = check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WHO_PLAYER->r + WITH_PLAYER->r, dr);
                    new_coll.time = double_min(new_coll.time, check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WHO_PLAYER->r0 + WITH_PLAYER->r0, 0));
                    break;
                case motPARTICLE:
                    new_coll.time = check_collision_between_two_balls(WITH_PARTICLE->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PARTICLE->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PARTICLE->r + WHO_PLAYER->r, dr);
                    new_coll.time = double_min(new_coll.time, check_collision_between_two_balls(WITH_PARTICLE->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PARTICLE->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PARTICLE->r + WHO_PLAYER->r0, 0));
                    break;
                default:
                    break;
            }
            break;
        case motPARTICLE:
            switch(with_whom->type){
                case motPLAYER:
                    if(WITH_PLAYER->shield + WITH_PLAYER->shield_push > WITH_PLAYER->r0){
                        dr = WITH_PLAYER->shield_push;
                    }else{
                        dr = 0;
                    }
                    new_coll.time = check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PARTICLE->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PLAYER->r + WHO_PARTICLE->r, dr);
                    new_coll.time = double_min(new_coll.time, check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PARTICLE->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PLAYER->r0 + WHO_PARTICLE->r, 0));
                    break;
                case motPARTICLE:
                    new_coll.time = check_collision_between_two_balls(WITH_PARTICLE->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_PARTICLE->center.y - WHO_PARTICLE->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PARTICLE->r + WHO_PARTICLE->r, 0);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    #undef WHO_PLAYER
    #undef WITH_PLAYER
    #undef WHO_PARTICLE
    #undef WITH_PARTICLE
    return new_coll;
}

/**
    First it's important to get fresh collision Data
    with fixed objects - that's the limit if the object
    doesn't collide with other movable one.
    */
void get_and_check_mov_coll_if_valid(level_data *level, short int who, short int with, double time_passed){
    collision_data coll = get_collision_with_movable(&level->movable_objects[who], &level->movable_objects[with]);
    coll.time += time_passed;
    if(coll.time >= 0 && coll.time <= 1){
        if(coll.time < level->movable_objects[who].coll_with_fixed.time &&
           coll.time < level->movable_objects[with].coll_with_fixed.time){//otherwise it's pointless to store such information
            coll.with_movable = true;
            coll.who = who;
            coll.with = with;
            coll_insert_node(&level->movable_objects[who].colls_with_mov, &coll);
            coll.who = with;
            coll.with = who;
            coll_insert_node(&level->movable_objects[with].colls_with_mov, &coll);
            if(time_passed > 0){
                collision_data *dirty_pointer = &coll_get_minimum(level->movable_objects[with].colls_with_mov.root,
                                                                         level->movable_objects[with].colls_with_mov.nil)->key;
                if(dirty_pointer != level->movable_objects[with].next_collision){//check if DIRTY things happen
                    if(level->movable_objects[with].next_collision->time <= 1 &&
                       level->movable_objects[with].next_collision->time >= 0){
                        coll = *level->movable_objects[with].next_collision;
                        if(coll.with < coll.who){
                            coll.who = coll.with;
                            coll.with = level->movable_objects[with].next_collision->who;
                        }
                        coll_insert_node(&level->dirty_tree, &coll);
                    }
                    level->movable_objects[with].next_collision = dirty_pointer;
                }
            }
        }
    }
}

/**
    A DFS that does something like that, according to tree in-order:
    i  <--  some_value
    for(j = i + 1; j < length; ++j){
        DO_STUFF_DO_STUFF_DO_STUFF_DO_STUFF_DO_STUFF
        DO_MORE_STUFF
    }
    */

void for_each_higher_check_collision(level_data *level, fast_read_set *movable_done,
                                     short int who, RB_node *node, RB_node *nil, double time_passed){
    while(node != nil &&
          node->key < who){
        node = node->right;
    }
    if(node != nil){
        if(who != node->key){
            for_each_higher_check_collision(level, movable_done, who, node->left, nil, time_passed);
            if(!is_marked(movable_done, node->key)){
                set_mark(movable_done, node->key);
                get_and_check_mov_coll_if_valid(level, who, node->key, time_passed);
            }
        }
        in_order_check_collision(level, movable_done, who, node->right, nil, time_passed);
    }
}

void in_order_check_collision(level_data *level, fast_read_set *movable_done,
                              short int who, RB_node *node, RB_node *nil, double time_passed){
    if(node != nil){
        in_order_check_collision(level, movable_done, who, node->left, nil, time_passed);

        if(!is_marked(movable_done, node->key)){
            set_mark(movable_done, node->key);
            get_and_check_mov_coll_if_valid(level, who, node->key, time_passed);
        }

        in_order_check_collision(level, movable_done, who, node->right, nil, time_passed);
    }
}

/**
    Detecting
    */
/**
    Solving quadratic equation
    x, y - relative present position of second ball
    dx, dy - relative displacement of first ball (second is immobile in this equation)
    d - sum of radiuses
    dr - change in radius sum in this quantum of time

    This function is good both for mobile and immobile balls or points,
    that change size or not :)
    */
double check_collision_between_two_balls(double x, double y, double dx, double dy,
                                         double d, double dr){
    double a = dx * dx + dy * dy - dr * dr,
        b = 2 * (x * dx + y * dy - d * dr);
    if(a == 0){//linear
        if(b == 0){
            return EMPTY_COLLISION_TIME;
        }else{
            b = -(x * x + y * y - d * d) / b;
            if(b <= 0 && b >= 1){
                return b;
            }else{
                return EMPTY_COLLISION_TIME;
            }
        }
    }else{
        d *= d;
        d = b * b - 4 * a * (x * x + y * y - d);//delta
        if(d < 0){
            return EMPTY_COLLISION_TIME;
        }else{
            b = -b;
            a *= 2;
            if(d == 0){
                b /= a;
                if(b >= 0 && b <= 1){
                    return b;
                }
                else{
                    return EMPTY_COLLISION_TIME;
                }
            }else{
                d = sqrt(d);
                x = (b + d) / a;
                y = (b - d) / a;
                if(x >= 0 && x <= 1){
                    if(y >= 0 && y <= 1){
                        return double_min(x, y);
                    }else{
                        return x;
                    }
                }else{
                    if(y >= 0 && y <= 1){
                        return y;
                    }else{
                        return EMPTY_COLLISION_TIME;
                    }
                }
            }
        }
    }
}

/**
    Computing the point on the circle that is
    closest to segment and then if it collides
    during this quantum of time  dt using
    get_segment_intersection and dividing
    one segment by another...
    That was one way to do it unntil I didn't
    include changing radius. Now I use the
    line equation to compute when distance will
    be equal to radius already modified at that
    point. Then I check if ball moved to that
    point collides - if the collision point
    is within segment constraints
    */
double check_collision_between_ball_and_segment(double x, double y, double dx, double dy, double r, double dr, segment *seg){
    double cs = cos(seg->ang),
           sn = sin(seg->ang),
           d_into = sign(dy * cs - dx * sn),
    dt = seg->line_equation.A * dx + seg->line_equation.B * dy + d_into * seg->line_equation.sqrtAB * dr;
    if(double_abs(dt) < double_eps){
        return EMPTY_COLLISION_TIME;
    }else{
        dt = (-d_into * seg->line_equation.sqrtAB * r - seg->line_equation.A * x - seg->line_equation.B * y - seg->line_equation.C) / dt;
        if(dt >= 0 && dt <= 1){
            r += dr * dt;
            point A = {x, y},
                  B = {x + 2 * (dx - r * sn * d_into),
                       y + 2 * (dy + r * cs * d_into)};
            if(do_segments_intersect(&A, &B, &seg->A, &seg->B)){
                return dt;
            }else{
                return EMPTY_COLLISION_TIME;
            }
        }else{
            return EMPTY_COLLISION_TIME;
        }
    }
}

double check_exit(double dx, double dy, fixed_exit *ex){
    double fi = vector_angle(dx, dy);
    if(sqrt(dx * dx + dy * dy) - radius_rectangle(ex, fi) < 0){
        return 0.01;
    }
    else{
        return EMPTY_COLLISION_TIME;
    }
}
/**
    Colliding
    */
void get_velocities_after_two_balls_collision(double *v1x, double *v1y, double *v2x, double *v2y,
                                              double dx, double dy, double m1, double m2, double dr, double restitution){
    *v1x -= *v2x;
    *v1y -= *v2y;
    dy = vector_angle(dx, dy);
    dx = cos(dy);
    dy = sin(dy);
    double v_into = *v1x * dx + *v1y * dy,
           v_perp = *v1y * dx - *v1x * dy,
           mc = m1 + m2;
           v_into += dr * sign(v_into);
    *v1x = v_into * ((m1 - restitution * m2) / mc);
    *v1y = *v1x * dy + *v2y + v_perp * dx;
    *v1x = *v1x * dx + *v2x - v_perp * dy;
    v_perp = (((1 + restitution) * m1) / mc) * v_into;
    *v2x += v_perp * dx;
    *v2y += v_perp * dy;

}

/**
    I treat fixed points as ininitely small balls
    */
void get_velocity_after_ball_to_fixed_ball_collision(double *vx, double *vy, double dx, double dy, double dr, double restitution){
    double ang = vector_angle(dx, dy),
           cs = cos(ang);
           ang = sin(ang);
    double v_into = *vx * cs + *vy * ang,
           v_perp = *vy * cs - *vx * ang;
    v_into += dr * sign(v_into);
    v_into *= -restitution;
    *vx = v_into * cs - v_perp * ang;
    *vy = v_into * ang + v_perp * cs;
}

void get_velocity_after_ball_to_wall_collision(double *vx, double *vy, segment *seg, double dr, double restitution){
    double cs = cos(seg->ang),
           sn = sin(seg->ang),
           v_into = *vy * cs - *vx * sn, //y
           v_perp = *vx * cs + *vy * sn; //x
    v_into += dr * sign(v_into);
    v_into *= -restitution;
    *vx = v_perp * cs - v_into * sn;
    *vy = v_perp * sn + v_into * cs;
}

/**
    Separating
    */
bool separate_two_balls(double *x1, double *y1, double m1, double *x2, double *y2, double m2, double d){
    double dx = *x2 - *x1,
           dy = *y2 - *y1,
           ang = vector_angle(dx, dy);
    dx = sqrt(dx * dx + dy * dy);
    if(dx < d){
        d -= dx;
        dx = d * sin(ang); //y
        d *= cos(ang); //x
        ang = m1 + m2;

        dy = m1 / ang; //m1 mass factor
        ang = m2 / ang; //m2 mass factor
        *x1 -= d * ang;
        *y1 -= dx * ang;
        *x2 += d * dy;
        *y2 += dx * dy;
        return true;
    }
    return false;
}

/**
    x1, y1 - coordinates of movable
    x2, y2 - coordinates of fixed ball
    d - sum of radiuses

    Of course works also for fixed points
    */
bool separate_ball_from_fixed_ball(double *x1, double *y1, double x2, double y2, double d){
    double dx = x2 - *x1,
           dy = y2 - *y1,
           ang = vector_angle(dx, dy);
    dx = sqrt(dx * dx + dy * dy);
    if(dx < d){
        d -= dx;
        //for some reason it doesn't work without:
        d *= 1.01;
        d += 1.3;
        *x1 -= d * cos(ang);
        *y1 -= d * sin(ang);
        return true;
    }
    return false;
}

bool separate_ball_from_segment(double *x, double *y, double d, segment *seg){
    double dist = point_distance_from_line(*x, *y, &seg->line_equation);
    if(dist == -1){
        return separate_ball_from_fixed_ball(x, y, seg->A.x, seg->A.y, d);
    }else{
        if(dist < d){
            double ang = vector_angle(seg->line_equation.A, seg->line_equation.B),
                   cs = cos(ang);
                   ang = sin(ang);
            point helpy = {*x + 2 * d * cs,
                                  *y + 2 * d * ang},
                         begin = {*x, *y};
            d -= dist;
            if(do_segments_intersect(&seg->A, &seg->B, &helpy, &begin)){
                *x -= d * cs;
                *y -= d * ang;
                //-ang
            }else{
                *x += d * cs;
                *y += d * ang;
                //ang
            }
            return true;
        }else{
            return false;
        }
    }
}

void check_dx_dy(double *dx, double *dy){
    if(double_abs(*dx) > MAX_DISPLACEMENT){
        *dx = MAX_DISPLACEMENT * sign(*dx);
    }
    if(double_abs(*dy) > MAX_DISPLACEMENT){
        *dy = MAX_DISPLACEMENT * sign(*dy);
    }
}

/**
    Private methods
    */
void player_get_dx_dy(movable_object *obj, double dt){
    #define Data ((movable_player*)obj->object_data)
    obj->dx = Data->vx * dt;
    obj->dy = Data->vy * dt;
    check_dx_dy(&obj->dx, &obj->dy);
    #undef Data
}

void particle_get_dx_dy(movable_object *obj, double dt){
    #define Data ((movable_particle*)obj->object_data)
    obj->dx = Data->vx * dt;
    obj->dy = Data->vy * dt;
    check_dx_dy(&obj->dx, &obj->dy);
    #undef Data
}
