#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "level.h"
#include "../structures.h"
#include "../mathematics.h"

/**
    Private methods
    */
bool add_primitive_to_zones_as_segment(level_data *level, const point *A, const point *B,
                                       short int prim);
/**
    level structure
    */
void construct_level(level_data *level){
    level->background = NULL;
    level->acc = NULL;
    level->at_exit = false;
    level->victory = false;
    new_scores(&level->score);

    level->number_of_movable_objects = 0;
    level->number_of_fixed_objects = 0;
    level->number_of_primitive_objects = 0;
    level->allocated_movable = INITIAL_ALLOCATED_MOVABLE;
    level->allocated_fixed = INITIAL_ALLOCATED_FIXED;
    level->allocated_primitive = INITIAL_ALLOCATED_PRIMITIVE;
    level->movable_objects =     (movable_object*)malloc(sizeof(movable_object)     * level->allocated_movable);
    level->fixed_objects =         (fixed_object*)malloc(sizeof(fixed_object)       * level->allocated_fixed);
    level->primitive_objects = (primitive_object*)malloc(sizeof(primitive_object)   * level->allocated_primitive);

    construct_heap(&level->collision_queue, INITIAL_OBJECT_COLLISION_QUEUE_SIZE);
    construct_zones(level);
    coll_construct_tree(&level->dirty_tree);
}

/**
    This function is called after
    loading level from file
    */
void initialize_level(level_data *level){
    add_borders(level);
    level->acc = (move_arrays*)malloc(sizeof(move_arrays) * level->number_of_movable_objects);
    level->dens = DEFAULT_FLUID_DENSITY;
    level->wind_vx = 0;
    level->wind_vy = 0;
    level->start_time = al_get_time();
    level->sum_time = 0;
}

void destroy_level(level_data *level){
    clear_level(level);
    al_destroy_bitmap(level->background);
    level->background = NULL;

    free(level->movable_objects);
    free(level->primitive_objects);
    free(level->fixed_objects);
    destroy_heap(&level->collision_queue);
    destroy_zones(level);
    coll_destroy_tree(&level->dirty_tree);
}

void clear_level(level_data *level){
    free(level->acc);
    level->acc = NULL;
    level->at_exit = false;

    clear_primitive_object_list(level);
    clear_fixed_object_list(level);
    clear_movable_object_list(level);
    clear_heap(&level->collision_queue);
    coll_clear_tree(&level->dirty_tree);
    clear_zones(level);
}

/**
    Scores
    */
/**
    This is done explicitly, not in clear_level!!!!
    */
score_struct clear_score(score_struct *score){
    score_struct res;
    score->total_score += score->score;
    res = *score;
    score->score = 0;
    return res;
}

void new_scores(score_struct *score){
    score->score = 0;
    score->total_score = 0;
    score->level_number = 1;
}

/**
    Dynamic arrays for game objects
    */

void add_movable_object(level_data *level, movable_object_type NewObjectType, void* new_object_data){
    if(level->number_of_movable_objects >= level->allocated_movable){
        level->allocated_movable *= 2;
        level->movable_objects = (movable_object*)realloc((void*)level->movable_objects, sizeof(movable_object) * level->allocated_movable);
    }
    level->movable_objects[level->number_of_movable_objects].type = NewObjectType;
    level->movable_objects[level->number_of_movable_objects].object_data = new_object_data;
    level->number_of_movable_objects += 1;
}

void delete_movable_object(movable_object *object){
    switch(object->type){
        case motPLAYER:
            free((movable_player*)object->object_data);
            break;
        case motPARTICLE:
            free((movable_particle*)object->object_data);
            break;
        case motDOOR:
            free((movable_door*)object->object_data);
            break;
        case motSWITCH:
            #define switch_data ((movable_switch*)object->object_data)
            if(switch_data->connected.number_of_doors > 0){
                free(switch_data->connected.doors);
            }
            if(switch_data->connected.number_of_switches > 0){
                free(switch_data->connected.switches);
            }
            free(switch_data);
            #undef switch_data
            break;
    }
    coll_destroy_tree(&object->colls_with_mov);
}

void clear_movable_object_list(level_data *level){
    int i;
    for(i = 0; i < level->number_of_movable_objects; ++i){
        delete_movable_object(&level->movable_objects[i]);
    }
    level->number_of_movable_objects = 0;
}

void add_fixed_object(level_data *level, fixed_object_type NewObjectType, void* new_object_data){
    if(level->number_of_fixed_objects >= level->allocated_fixed){
        level->allocated_fixed *= 2;
        level->fixed_objects = (fixed_object*)realloc((void*)level->fixed_objects, sizeof(fixed_object) * level->allocated_fixed);
    }
    level->fixed_objects[level->number_of_fixed_objects].type = NewObjectType;
    level->fixed_objects[level->number_of_fixed_objects].object_data = new_object_data;
    level->number_of_fixed_objects += 1;
}

void delete_fixed_object(fixed_object *object){
    switch(object->type){
        case fotSQUARE:
            free((fixed_square*)object->object_data);
            break;
        case fotRECTANGLE:
            free((fixed_rectangle*)object->object_data);
            break;
        case fotCIRCLE:
            free((fixed_circle*)object->object_data);
            break;
        case fotENTRANCE:
            free((fixed_entrance*)object->object_data);
            break;
        case fotEXIT:
            free((fixed_exit*)object->object_data);
            break;
    }
}

void clear_fixed_object_list(level_data *level){
    int i;
    for(i = 0; i < level->number_of_fixed_objects; ++i){
        delete_fixed_object(&level->fixed_objects[i]);
    }
    level->number_of_fixed_objects = 0;
}

void add_primitive_object(level_data *level, primitive_object_type NewObjectType, void* new_object_data){
    if(level->number_of_primitive_objects >= level->allocated_primitive){
        level->allocated_primitive *= 2;
        level->primitive_objects = (primitive_object*)realloc((void*)level->primitive_objects, sizeof(primitive_object) * level->allocated_primitive);
    }
    level->primitive_objects[level->number_of_primitive_objects].type = NewObjectType;
    level->primitive_objects[level->number_of_primitive_objects].object_data = new_object_data;
    level->number_of_primitive_objects += 1;
}

void delete_primitive_object(primitive_object *object){
    switch(object->type){
        case potPOINT:
            free((point*)object->object_data);
            break;
        case potSEGMENT:
            free((segment*)object->object_data);
            break;
        case potCIRCLE:
            free((circle*)object->object_data);
            break;
        case potEXIT:
            free((prim_exit*)object->object_data);
    }
}

void clear_primitive_object_list(level_data *level){
    int i;
    for(i = 0; i < level->number_of_primitive_objects; ++i){
        delete_primitive_object(&level->primitive_objects[i]);
    }
    level->number_of_primitive_objects = 0;
}


/**
    Zones matrix
    */
void construct_zones(level_data *level){
    short int i, j;
    for(i = 0; i < ZONE_FACTOR; ++i){
        for(j = 0; j < ZONE_FACTOR; ++j){
            construct_zone(&level->zones[i][j]);
        }
    }
}

void destroy_zones(level_data *level){
    short int i, j;
    for(i = 0; i < ZONE_FACTOR; ++i){
        for(j = 0; j < ZONE_FACTOR; ++j){
            destroy_zone(&level->zones[i][j]);
        }
    }
}

void clear_zones(level_data *level){
    short int i, j;
    for(i = 0; i < ZONE_FACTOR; ++i){
        for(j = 0; j < ZONE_FACTOR; ++j){
            clear_zone(&level->zones[i][j]);
        }
    }
}

void initialize_zones_with_movable(level_data *level, short int *zones, short int index){
    int i, j;
    for(i = zones[0]; i <= zones[2]; ++i){
        for(j = zones[1]; j <= zones[3]; ++j){
            insert_node(&level->zones[i][j].movable, index);
        }
    }
}

/**
    Sets zones for new  dx  and  dy  values
    Requires setting new dx and dy before calling
    It only looks bad, but it seems quite optimal
    actually :P
    */
void change_zones_for_movable(level_data *level, short int index, long double t){
    short int oldz[4], xleft, xright;
    int i, j;
    movable_object *obj = &level->movable_objects[index];
    for(i = 0; i < 4; ++i){
        oldz[i] = obj->zones[i];
    }
    if(obj->type == motPLAYER){
        #define player ((movable_player*)obj->object_data)
        get_zone_for_object(player->center.x, player->center.y,
                            obj->dx * t, obj->dy * t, player->r + player->shield_push * 2,
                            obj->zones);
        #undef player
    }else{
        get_zone_for_object(((point*)(obj->object_data))->x,
                            ((point*)(obj->object_data))->y,
                            obj->dx * t, obj->dy * t, ((fixed_circle*)obj->object_data)->r,
                            obj->zones);
    }

    #define newz(x) (obj->zones[x])
    #define Zonez(x, y) (level->zones[x][y])
    if(newz(2) >= oldz[0] &&
       newz(0) <= oldz[2] &&
       newz(3) >= oldz[1] &&
       newz(1) <= oldz[3]){//There are some common zones

        if(newz(0) > oldz[0]){
            for(i = oldz[0]; i < newz(0); ++i){
                for(j = oldz[1]; j <= oldz[3]; ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = newz(0); i < oldz[0]; ++i){
                for(j = newz(1); j <= newz(3); ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }

        if(oldz[2] > newz(2)){
            for(i = newz(2) + 1; i <= oldz[2]; ++i){
                for(j = oldz[1]; j <= oldz[3]; ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = oldz[2] + 1; i <= newz(2); ++i){
                for(j = newz(1); j <= newz(3); ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }

        xleft = short_max(newz(0), oldz[0]);
        xright = short_min(newz(2), oldz[2]);

        if(newz(1) < oldz[1]){
            for(j = newz(1); j < oldz[1]; ++j){
                for(i = xleft; i <= xright; ++i){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(j = oldz[1]; j < newz(1); ++j){
                for(i = xleft; i <= xright; ++i){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }

        if(newz(3) > oldz[3]){
            for(j = oldz[3] + 1; j <= newz(3); ++j){
                for(i = xleft; i <= xright; ++i){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(j = newz(3) + 1; j <= oldz[3]; ++j){
                for(i = xleft; i <= xright; ++i){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }
    }else{
        for(i = oldz[0]; i <= oldz[2]; ++i){
            for(j = oldz[1]; j <= oldz[3]; ++j){
                delete_node(&Zonez(i, j).movable, index);
            }
        }

        for(i = newz(0); i <= newz(2); ++i){
            for(j = newz(1); j <= newz(3); ++j){
                insert_node(&Zonez(i, j).movable, index);
            }
        }
    }
    #undef Zonez
    #undef newz
}

/**
    A lot of freakin' cases!
    This function should be fired
    after checking rectangular
    limits of the segment and the map
    */
bool get_intersection_within_borders(const point *A, const point *B, short int *zones){
    point correct, bord_1, bord_2;
    if(zones[0] < 0){
        bord_1.x = 0;
        bord_1.y = 0;
        bord_2.x = 0;
        bord_2.y = SCREEN_BUFFER_HEIGHT;
        if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
            get_zone(correct.x, correct.y, zones);
            return true;
        }else{
            if(zones[1] < 0){
                bord_2.x = SCREEN_BUFFER_HEIGHT;
                bord_2.y = 0;
                if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else if(zones[1] >= ZONE_FACTOR){
                bord_1.x = SCREEN_BUFFER_HEIGHT;
                bord_1.y = SCREEN_BUFFER_HEIGHT;
                if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }
    }else if(zones[0] >= ZONE_FACTOR){
        bord_1.x = SCREEN_BUFFER_HEIGHT;
        bord_1.y = 0;
        bord_2.x = SCREEN_BUFFER_HEIGHT;
        bord_2.y = SCREEN_BUFFER_HEIGHT;
        if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
            get_zone(correct.x, correct.y, zones);
            return true;
        }else{
            if(zones[1] < 0){
                bord_2.x = 0;
                bord_2.y = 0;
                if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else if(zones[1] >= ZONE_FACTOR){
                bord_1.x = 0;
                bord_1.y = SCREEN_BUFFER_HEIGHT;
                if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }
    }else{
        if(zones[1] < 0){
            bord_1.x = 0;
            bord_1.y = 0;
            bord_2.x = SCREEN_BUFFER_HEIGHT;
            bord_2.y = 0;
            if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                get_zone(correct.x, correct.y, zones);
                return true;
            }else{
                return false;
            }
        }else if(zones[1] >= ZONE_FACTOR){
            bord_1.x = 0;
            bord_1.y = SCREEN_BUFFER_HEIGHT;
            bord_2.x = SCREEN_BUFFER_HEIGHT;
            bord_2.y = SCREEN_BUFFER_HEIGHT;
            if(get_segment_intersection(A, B, &bord_1, &bord_2, &correct)){
                get_zone(correct.x, correct.y, zones);
                return true;
            }else{
                return false;
            }
        }else{
            /**
            The point is fully inside map
            and doesn't need any correction
            */
            return true;
        }
    }
}

bool get_outer_zones_of_segment(const point *A, const point *B, short int *zones){
    get_zone(A->x, A->y, zones);
    get_zone(B->x, B->y, zones + 2);
    /**
        Rule out the cases when the rectangular limits
        of the segments and the map don't collide
        */
          if(zones[0] < 0 && zones[2] < 0){
        return false;
    }else if(zones[1] < 0 && zones[3] < 0){
        return false;
    }else if(zones[0] >= ZONE_FACTOR && zones[2] >= ZONE_FACTOR){
        return false;
    }else if(zones[1] >= ZONE_FACTOR && zones[3] >= ZONE_FACTOR){
        return false;
    }else{
        /**
            Now, if any point is outside the bonds
            it is either possible to find intersection
            with the border and continue from there
            or segment is still outside the map
            */
        if(!get_intersection_within_borders(A, B, zones)){
            return false;
        }else if(!get_intersection_within_borders(B, A, zones + 2)){
            return false;
        }else{
            return true;
        }
    }
}

void normalize_segment_zones(short int *zones){
    short int temp;
    if(zones[0] > zones[2]){
        temp = zones[2];
        zones[2] = zones[0];
        zones[0] = temp;
        temp = zones[3];
        zones[3] = zones[1];
        zones[1] = temp;
    }
}

/**
    Adding
    */

/**
    Something universal; needs the segment points,
    but adds  prim as anything you want - as it's
    a number on the primitives list.
    Returns true if at least a part of segment
    intersects with the map, false if not or if
    A == B
    */
bool add_primitive_to_zones_as_segment(level_data *level, const point *A, const point *B,
                                       short int prim){
    if(A->x != B->x || A->y != B->y){
        short int zn[4];
        if(get_outer_zones_of_segment(A, B, zn)){
            normalize_segment_zones(zn);
            short int i = short_sign(zn[3] - zn[1]),
                      j;
            if(zn[0] == zn[2]){//to simplify: vertical
                for(j = zn[1]; j != zn[3]; j += i){
                    add_primitive_to_zone(&level->zones[zn[0]][j], prim);
                }
                add_primitive_to_zone(&level->zones[zn[0]][zn[3]], prim);
            }else if(zn[1] == zn[3]){//horizontal
                short int j;
                for(j = zn[0]; j != zn[2]; ++j){
                    add_primitive_to_zone(&level->zones[j][zn[1]], prim);
                }
                add_primitive_to_zone(&level->zones[zn[2]][zn[1]], prim);
            }else{
                point bord_1, bord_2;
                while(zn[0] < zn[2] ||
                      short_sign(zn[3] - zn[1]) == i){
                    add_primitive_to_zone(&level->zones[zn[0]][zn[1]], prim);

                    bord_1.x = (zn[0] + 1) * ZONE_SIZE;
                    bord_1.y = zn[1] * ZONE_SIZE;
                    bord_2.x = bord_1.x;
                    bord_2.y = bord_1.y + ZONE_SIZE;
                    if(do_segments_intersect(&bord_1, &bord_2, A, B)){
                        zn[0] += 1;
                    }else{
                        zn[1] += i;
                    }
                }
                add_primitive_to_zone(&level->zones[zn[2]][zn[3]], prim);
            }
            return true;
        }
    }
    return false;
}

/**
    This function does all the allocating
    and counting.
    If segment is outside the map
    it is not added anywhere.
    If A and B are the same point, then nothing is done;
    this assures that seg->line_equation->sqrtAB is different from 0.
    At least it should be...
    */
void add_segment(level_data *level, const point *A, const point *B){
    /**
        Reversed order: first check if there is anything to add to zones
        then if it's true do it and at the end allocate the thing ^^
        */
    if(add_primitive_to_zones_as_segment(level, A, B, level->number_of_primitive_objects)){
        segment *seg = construct_segment(A, B);
        add_primitive_object(level, potSEGMENT, seg);
    }
}

void add_borders(level_data *level){
    point A = {0, 0},
                 B = {0, SCREEN_BUFFER_HEIGHT};
    add_segment(level, &A, &B);
    B.x = SCREEN_BUFFER_HEIGHT;
    B.y = 0;
    add_segment(level, &A, &B);
    A.x = SCREEN_BUFFER_HEIGHT;
    A.y = SCREEN_BUFFER_HEIGHT;
    add_segment(level, &A, &B);
    B.x = 0;
    B.y = SCREEN_BUFFER_HEIGHT;
    add_segment(level, &A, &B);
}

/**
    This function does not allocate
    the object Data - it needs to
    get newly allocated point.
    */
void add_point(level_data *level, point *A){
    short int zn[2];
    get_zone(A->x, A->y, zn);
    if(zn[0] >= 0 && zn[0] < ZONE_FACTOR &&
       zn[1] >= 0 && zn[1] < ZONE_FACTOR){
        add_primitive_object(level, potPOINT, (void*)A);
        add_primitive_to_zone(&level->zones[zn[0]][zn[1]], level->number_of_primitive_objects - 1);
    }
}

void add_circle(level_data *level, long double r, point center){
    short int zones[4];
    get_zone(center.x - r, center.y - r, zones);
    get_zone(center.x + r, center.y + r, zones + 2);
    if(zones[0] < ZONE_FACTOR && zones[1] < ZONE_FACTOR &&
       zones[2] >= 0 && zones[3] >= 0){
        if(zones[0] < 0){
            zones[0] = 0;
        }
        if(zones[1] < 0){
            zones[1] = 0;
        }
        if(zones[2] >= ZONE_FACTOR){
            zones[2] = ZONE_FACTOR - 1;
        }
        if(zones[3] >= ZONE_FACTOR){
            zones[3] = ZONE_FACTOR - 1;
        }
        circle *C = (circle*)malloc(sizeof(circle));
        C->center = center;
        C->r = r;
        short int i, j, key = level->number_of_primitive_objects;
        add_primitive_object(level, potCIRCLE, C);
        long double r0 = (SQRT2 / 2) * ZONE_SIZE,
              dx, dy;
        for(i = zones[0]; i <= zones[2]; ++i){
            for(j = zones[1]; j <= zones[3]; ++j){
                dx = C->center.x - (i + 0.5) * ZONE_SIZE;
                dy = C->center.y - (j + 0.5) * ZONE_SIZE;
                if(sqrt(dx * dx + dy * dy) < square_equation(r0, vector_angle(dx, dy)) + C->r){
                    add_primitive_to_zone(&level->zones[i][j], key);
                }
            }
        }
    }
}

void add_square(level_data *level, fixed_square *square){
    add_point(level, square->v1);
    add_point(level, square->v2);
    add_point(level, square->v3);
    add_point(level, square->v4);
    add_segment(level, square->v1, square->v2);
    add_segment(level, square->v2, square->v3);
    add_segment(level, square->v3, square->v4);
    add_segment(level, square->v4, square->v1);
}

void add_rectangle(level_data *level, fixed_rectangle *rectangle){
    add_point(level, rectangle->v1);
    add_point(level, rectangle->v2);
    add_point(level, rectangle->v3);
    add_point(level, rectangle->v4);
    add_segment(level, rectangle->v1, rectangle->v2);
    add_segment(level, rectangle->v2, rectangle->v3);
    add_segment(level, rectangle->v3, rectangle->v4);
    add_segment(level, rectangle->v4, rectangle->v1);
}

void add_exit(level_data *level, fixed_exit *ex){
    short int key = level->number_of_primitive_objects;
    bool flag = false;
    flag |= add_primitive_to_zones_as_segment(level, ex->v1, ex->v2, key);
    flag |= add_primitive_to_zones_as_segment(level, ex->v2, ex->v3, key);
    flag |= add_primitive_to_zones_as_segment(level, ex->v3, ex->v4, key);
    flag |= add_primitive_to_zones_as_segment(level, ex->v4, ex->v1, key);
    if(flag){
        prim_exit *pr_ex = construct_prim_exit(ex);
        add_primitive_object(level, potEXIT, (void*)pr_ex);
    }
}
