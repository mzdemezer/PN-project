#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include "main.h"
#include "game.h"

/**
    Private methods
    */
void StopThread(int i, game_shared_data *Data, ALLEGRO_THREAD *thread);
bool get_drag_data(movable_object *obj, double *vx, double *vy, double *Cx, double *S);
bool get_grav_data(movable_object *obj, double *mass, double *r);
bool get_elec_data(movable_object *obj, double *charge, double *r);

/**
    Code
    */
bool initialize_iteration_threads(game_shared_data *Data){
    int i;

    Data->thread_main_iteration = NULL;
    Data->thread_main_iteration = al_create_thread(main_iteration, (void*)Data);
    if(!Data->thread_main_iteration){
        fprintf(stderr, "Failed to initialize main_iteration thread\n");
        return false;
    }
    Data->iteration_threads[0].job = compute_gravity;
    Data->iteration_threads[1].job = compute_electrostatics;
    Data->iteration_threads[2].job = compute_collisions;

    for(i = 0; i < NumOfThreads; ++i){
        Data->iteration_threads[i].thread = al_create_thread(Data->iteration_threads[i].job, (void*)Data);
        if(!Data->iteration_threads[i].thread){
            fprintf(stderr, "Failed to initialize iteration#%d thread\n", i);
            return false;
        }
    }
    return true;
}

/**
    This function signals that the  thread[i] finished its work
    and then sets it to wait. This is part of synchronization.
    */
void StopThread(int i, game_shared_data *Data, ALLEGRO_THREAD *thread){
    al_lock_mutex(Data->mutex_iteration);
        Data->iteration_threads[i].finished = true;
        if(Data->main_iter_thread_waiting){
            al_broadcast_cond(Data->cond_main_iteration);
        }
        while(Data->iteration_threads[i].finished && !al_get_thread_should_stop(thread)){
            al_wait_cond(Data->cond_iterations, Data->mutex_iteration);
        }
    al_unlock_mutex(Data->mutex_iteration);
}

/**
    This procedure forces termination of all iteration threads
    and is enough to be sure, that they are closed. Also, it
    always shuts them down in a way, that the whole iteration
    cycle is done, never in between. At the end it does level
    clean-up. This way, game is ready for either exit or another
    level.
    */
void terminate_iteration(game_shared_data *Data){
    int i;
    /**
        Terminating small threads
        */
    al_lock_mutex(Data->mutex_iteration);
        for(i = 0; i < NumOfThreads; ++i){
            al_set_thread_should_stop(Data->iteration_threads[i].thread);
        }
        for(i = 0; i < NumOfThreads; ++i){
            Data->iteration_threads[i].finished = false;
        }
        al_broadcast_cond(Data->cond_iterations);
    al_unlock_mutex(Data->mutex_iteration);
    for(i = 0; i < NumOfThreads; ++i){
        printf("waiting for #%d thread\n", i);
        al_destroy_thread(Data->iteration_threads[i].thread);
    }
    /**
        Terminating MainIteration thread
        */
    al_lock_mutex(Data->mutex_main_iteration);
        al_set_thread_should_stop(Data->thread_main_iteration);
        for(i = 0; i < NumOfThreads; ++i){
            Data->iteration_threads[i].finished = true;
        }
        Data->iteration_finished = false;
        al_broadcast_cond(Data->cond_main_iteration);
    al_unlock_mutex(Data->mutex_main_iteration);
    al_destroy_thread(Data->thread_main_iteration);

    /**
        Clean-up
        */
    clear_level(&Data->level);
}

/**
    This thread on one hand communicates
    with event thread and draw thread and on the other
    with auxilary threads that compute certain tasks for
    the next iteration
    */
void* main_iteration(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((game_shared_data*)argument)
    #define dt Data->level.dt
    #define acc Data->level.acc

    int i, j;
    bool f;
    double Cx, S, vx, vy;
    double op;

    /**
        Initialization
        */

    Data->main_iter_thread_waiting = true;
    for(i = 0; i < NumOfThreads; ++i){
        al_start_thread(Data->iteration_threads[i].thread);
    }
    printf("In game: Threads started\n");
    al_lock_mutex(Data->mutex_iteration);
        while(true){
            f = true;
            for(i = 0; i < NumOfThreads; ++i){
                if(!Data->iteration_threads[i].finished){
                    f = false;
                    printf("In game: %d not ready\n", i);
                    break;
                }
            }
            if(f){
                break;
            }
            al_wait_cond(Data->cond_main_iteration, Data->mutex_iteration);
        }
    al_unlock_mutex(Data->mutex_iteration);

    Data->level.last_time = al_get_time();

    for(i = 0; i < Data->level.number_of_movable_objects; ++i){
        for(j = 0; j < ACC_2nd_DIM; ++j){
            acc[i].ax[j] = 0;
            acc[i].ay[j] = 0;
        }
    }
    bool parity = false,
         imparity = true;
    double half_dt;

    printf("In game: after main-iter-init, starting to operate\n");
    Data->level.start_time = al_get_time();

    while(!al_get_thread_should_stop(thread)){
        /**
            Init
            */

        dt = Data->level.last_time;
        Data->level.last_time = al_get_time();
        dt = Data->level.last_time - dt;
        if(dt > MAX_DT){
            /**
                Security against slowdowns, whatever the reason.
                dt affects zones, soo when it raises too much
                each object would demand all zones.
                */
            dt = MAX_DT;
        }
        half_dt = dt / 2;

        op = Data->level.player->engine_state * THROTTLE * dt;
        Data->level.player->vx += op * cos(Data->level.player->ang);
        Data->level.player->vy += op * sin(Data->level.player->ang);

        if(!Data->level.player->shield_generator){
            if(Data->level.player->shield > eps){
                Data->level.player->shield -= 1;
                if(Data->level.player->shield <= eps){
                    Data->level.player->shield = 0;
                }
            }
        }else{
            if(Data->level.player->shield < PLAYER_MAX_SHIELD){
                Data->level.player->shield += Data->level.player->shield_generator * PLAYER_SHIELD_THROTTLE;
                if(Data->level.player->shield > PLAYER_MAX_SHIELD){
                    Data->level.player->shield = PLAYER_MAX_SHIELD;
                }
            }
        }

        get_player_radius(Data->level.player, op);

        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            acc[i].x = ((fixed_circle*)(Data->level.movable_objects[i].object_data))->center.x;
            acc[i].y = ((fixed_circle*)(Data->level.movable_objects[i].object_data))->center.y;
            switch(Data->level.movable_objects[i].type){
                case motPLAYER:
                    Data->level.movable_objects[i].dx = ((movable_player*)Data->level.movable_objects[i].object_data)->vx * dt;
                    Data->level.movable_objects[i].dy = ((movable_player*)Data->level.movable_objects[i].object_data)->vy * dt;
                    change_zones_for_movable(&Data->level, i, 1.);
                    break;
                case motPARTICLE:
                    Data->level.movable_objects[i].dx = ((movable_particle*)Data->level.movable_objects[i].object_data)->vx * dt;
                    Data->level.movable_objects[i].dy = ((movable_particle*)Data->level.movable_objects[i].object_data)->vy * dt;
                    change_zones_for_movable(&Data->level, i, 1.);
                    break;
                default:
                    ;
            }
        }

        /**
            Launching other iterations
            */
        Data->main_iter_thread_waiting = false;

        al_lock_mutex(Data->mutex_iteration);
            for(i = 0; i < NumOfThreads; ++i){
                Data->iteration_threads[i].finished = false;
            }
            al_broadcast_cond(Data->cond_iterations);
        al_unlock_mutex(Data->mutex_iteration);

        /**
            Main iteration thread main work
            */

        /**
            1. Handle keyboard input
            */
        al_lock_mutex(Data->keyboard.mutex_keyboard);
        if(Data->keyboard.flags[ekKEY_DOWN]){
            if(Data->keyboard.flags[ekKEY_UP]){
                Data->level.player->engine_state /= 2;
            }
            else{
                if(Data->level.player->engine_state > 0){
                    Data->level.player->engine_state /= 2;
                }else{
                    if(Data->level.player->engine_state > MAX_DECELERATE){
                        Data->level.player->engine_state -= 1;
                    }
                }
            }
        }
        else if(Data->keyboard.flags[ekKEY_UP]){
            if(Data->level.player->engine_state < 0){
                Data->level.player->engine_state /= 2;
            }else{
                if(Data->level.player->engine_state < MAX_ACCELERATE){
                    Data->level.player->engine_state += 1;
                }
            }
        }else{
            if(Data->level.player->engine_state){
                Data->level.player->engine_state /= 2;
            }
        }



        if(Data->keyboard.flags[ekKEY_RIGHT]){
            if(!Data->keyboard.flags[ekKEY_LEFT]){
                Data->level.player->ang += dANG;
            }
        }
        else if(Data->keyboard.flags[ekKEY_LEFT]){
            Data->level.player->ang -= dANG;
        }


        if(Data->keyboard.flags[ekKEY_SHIELD]){
            if(Data->level.player->shield_generator < PLAYER_MAX_SHIELD_GENERATOR){
                Data->level.player->shield_generator += 1;
            }
            Data->level.player->energy_generator -= 5;
        }else{
            if(Data->level.player->shield_generator > 0){
                Data->level.player->shield_generator /= 2;
            }
            if(Data->level.player->energy_generator < PLAYER_MAX_ENERGY){
                Data->level.player->energy_generator += 1;
            }
        }
        if(Data->level.player->energy_generator <= 0){
            Data->level.player->energy_generator = 0;
            Data->keyboard.flags[ekKEY_SHIELD] = false;
        }

        al_unlock_mutex(Data->keyboard.mutex_keyboard);
        /**
            Waiting until other threads finish
            */
        al_lock_mutex(Data->mutex_iteration);
            Data->main_iter_thread_waiting = true;
            while(true){
                f = true;
                for(i = 0; i < NumOfThreads; ++i){
                    if(!Data->iteration_threads[i].finished){
                        f = false;
                        break;
                    }
                }
                if(f){
                    break;
                }
                al_wait_cond(Data->cond_main_iteration, Data->mutex_iteration);
            }
        al_unlock_mutex(Data->mutex_iteration);

        /**
            Main iteration thread after-work
            */
        al_lock_mutex(Data->mutex_main_iteration);
        if(Data->level.player->HP <= 0){
                Data->level.sum_time -= Data->level.start_time;
                Data->level.start_time = al_get_time();
                Data->level.sum_time += Data->level.start_time;
                Data->level.victory = false;
                Data->level.at_exit = true;
                Data->level.score.level_number += 1;
            al_unlock_mutex(Data->mutex_main_iteration);
        }else if(Data->level.at_exit){
                Data->level.sum_time -= Data->level.start_time;
                Data->level.start_time = al_get_time();
                Data->level.sum_time += Data->level.start_time;
                Data->level.victory = true;
                Data->level.score.score += Data->level.player->HP;
                dt = TIME_LIMIT + Data->level.score.level_number - Data->level.sum_time;
                if(dt < 0){
                    dt = 0;
                }else{
                    dt *= TIME_SCORE_MULTIPLIER;
                }
                Data->level.score.score += dt;
            al_unlock_mutex(Data->mutex_main_iteration);
        }else{
            al_unlock_mutex(Data->mutex_main_iteration);
            for(i = 0; i < Data->level.number_of_movable_objects; ++i){
                if(get_drag_data(&(Data->level.movable_objects[i]), &vx, &vy, &Cx, &S)){
                    Cx = S * Cx * Data->level.dens;
                    vx = Data->level.wind_vx - vx;
                    acc[i].ax[4] = vx * 3 * Cx * coefficient_multiplier(vx);
                    vy = Data->level.wind_vy - vy;
                    acc[i].ay[4] = vy * 3 * Cx * coefficient_multiplier(vy);
                }
                switch(Data->level.movable_objects[i].type){
                    case motPLAYER:
                        #define obj_data ((movable_player*)(Data->level.movable_objects[i].object_data))
                        op = obj_data->mass;
                        acc[i].ax[(int)parity] = acc[i].ax[2];
                        for(j = 3; j < ACC_2nd_DIM; ++j){
                            acc[i].ax[(int)parity] += acc[i].ax[j];
                        }
                        acc[i].ax[(int)parity] /= op;

                        acc[i].ay[(int)parity] = acc[i].ay[2];
                        for(j = 3; j < ACC_2nd_DIM; ++j){
                            acc[i].ay[(int)parity] += acc[i].ay[j];
                        }
                        acc[i].ay[(int)parity] /= op;

                        obj_data->vx += (acc[i].ax[(int)parity] + acc[i].ax[(int)imparity]) * half_dt;
                        obj_data->vy += (acc[i].ay[(int)parity] + acc[i].ay[(int)imparity]) * half_dt;

                        /**
                            Simple bounce - to delete this I need better separation algorithm
                            */
                        if(obj_data->center.x - obj_data->r <= 0){
                            obj_data->center.x = obj_data->r + obj_data->r - obj_data->center.x;
                            if(obj_data->vx < 0){
                               obj_data->vx *= -PLAYER_TO_WALL_RESTITUTION;
                            }
                        }else if(obj_data->center.x + obj_data->r >= SCREEN_BUFFER_HEIGHT){
                            obj_data->center.x = 2 * SCREEN_BUFFER_HEIGHT - (obj_data->center.x + obj_data->r + obj_data->r);
                            if(obj_data->vx > 0){
                               obj_data->vx *= -PLAYER_TO_WALL_RESTITUTION;
                            }
                        }
                        if(obj_data->center.y - obj_data->r <= 0){
                            obj_data->center.y = obj_data->r + obj_data->r - obj_data->center.y;
                            if(obj_data->vy < 0){
                               obj_data->vy *= -PLAYER_TO_WALL_RESTITUTION;
                            }
                        }else if(obj_data->center.y + obj_data->r >= SCREEN_BUFFER_HEIGHT){
                            obj_data->center.y = 2 * SCREEN_BUFFER_HEIGHT - (obj_data->center.y + obj_data->r + obj_data->r);
                            if(obj_data->vy > 0){
                               obj_data->vy *= -PLAYER_TO_WALL_RESTITUTION;
                            }
                        }
                        #undef obj_data
                        break;
                    case motPARTICLE:
                        #define obj_data ((movable_particle*)(Data->level.movable_objects[i].object_data))
                        op = obj_data->mass;
                        acc[i].ax[(int)parity] = acc[i].ax[2];
                        for(j = 3; j < ACC_2nd_DIM; ++j){
                            acc[i].ax[(int)parity] += acc[i].ax[j];
                        }
                        acc[i].ax[(int)parity] /= op;

                        acc[i].ay[(int)parity] = acc[i].ay[2];
                        for(j = 3; j < ACC_2nd_DIM; ++j){
                            acc[i].ay[(int)parity] += acc[i].ay[j];
                        }
                        acc[i].ay[(int)parity] /= op;

                        obj_data->vx += (acc[i].ax[(int)parity] + acc[i].ax[(int)imparity]) * half_dt;
                        obj_data->vy += (acc[i].ay[(int)parity] + acc[i].ay[(int)imparity]) * half_dt;

                        /**
                            Simple bounce - just in case, segments are not perfect!
                            */
                        if(obj_data->center.x - obj_data->r <= 0){
                            obj_data->center.x = obj_data->r + obj_data->r - obj_data->center.x;
                            if(obj_data->vx < 0){
                               obj_data->vx *= -PARTICLE_TO_WALL_RESTITUTION;
                            }
                        }else if(obj_data->center.x + obj_data->r >= SCREEN_BUFFER_HEIGHT){
                            obj_data->center.x = 2 * SCREEN_BUFFER_HEIGHT - (obj_data->center.x + obj_data->r + obj_data->r);
                            if(obj_data->vx > 0){
                               obj_data->vx *= -PARTICLE_TO_WALL_RESTITUTION;
                            }
                        }
                        if(obj_data->center.y - obj_data->r <= 0){
                            obj_data->center.y = obj_data->r + obj_data->r - obj_data->center.y;
                            if(obj_data->vy < 0){
                               obj_data->vy *= -PARTICLE_TO_WALL_RESTITUTION;
                            }
                        }else if(obj_data->center.y + obj_data->r >= SCREEN_BUFFER_HEIGHT){
                            obj_data->center.y = 2 * SCREEN_BUFFER_HEIGHT - (obj_data->center.y + obj_data->r + obj_data->r);
                            if(obj_data->vy > 0){
                               obj_data->vy *= -PARTICLE_TO_WALL_RESTITUTION;
                            }
                        }
                        #undef obj_data
                        break;
                    case motSWITCH:
                        break;
                    case motDOOR:
                        break;
                }
            }
        }
        /**
            Ready to draw
            */
        al_lock_mutex(Data->mutex_draw_call);
            Data->draw_call = true;
        al_unlock_mutex(Data->mutex_draw_call);

        /**
            After-after work, that can be done
            after sending ready-draw signal;
            clean-up
            */
        parity = !parity;
        imparity = !imparity;
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            coll_clear_tree(&Data->level.movable_objects[i].colls_with_mov);
        }

        if(!al_get_thread_should_stop(thread)){
            al_lock_mutex(Data->mutex_main_iteration);
                Data->iteration_finished = true;
                if(Data->synchro_with_main_iter){
                    Data->synchro_with_main_iter = false;
                    al_broadcast_cond(Data->cond_synchro_with_main_iter);
                }
                while(Data->iteration_finished){
                    al_wait_cond(Data->cond_main_iteration, Data->mutex_main_iteration);
                }
            al_unlock_mutex(Data->mutex_main_iteration);
        }
    }
    printf("Closing thread: main-iteration\n");

    return NULL;
    #undef acc
    #undef dt
    #undef Data
}

/**
    Auxilary threads
    */
void* compute_gravity(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((game_shared_data*)argument)
    #define acc Data->level.acc
    int i, j;
    double m1, m2, dx, dy, r1, r2, d, r;

    StopThread(0, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            acc[i].ax[2] = 0;
            acc[i].ay[2] = 0;
        }
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            if(get_grav_data(&(Data->level.movable_objects[i]), &m1, &r1)){
                for(j = i + 1; j < Data->level.number_of_movable_objects; ++j){
                    if(get_grav_data(&(Data->level.movable_objects[j]), &m2, &r2)){
                        dx = acc[j].x - acc[i].x;
                        dy = acc[j].y - acc[i].y;
                        d = dx * dx + dy * dy;
                        r = r1 + r2;
                        if(r * r > d){
                            d = sqrt(d);
                            m2 *= m1 * ((GRAV * d) / (r * r * r));
                        }else{
                            m2 *= m1 * (GRAV / d);
                        }
                        dy = vector_angle(dx, dy);
                        dx = cos(dy);
                        dy = sin(dy);
                        acc[i].ax[2] += m2 * dx;
                        acc[i].ay[2] += m2 * dy;
                        acc[j].ax[2] += -m2 * dx;
                        acc[j].ay[2] += -m2 * dy;
                    }
                }
            }
        }

        /**
            Signal and stop
            */

        StopThread(0, Data, thread);
    }
    printf("Closing thread #0\n");

    return NULL;
    #undef acc
    #undef Data
}

void* compute_electrostatics(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((game_shared_data*)argument)
    #define acc Data->level.acc
    int i, j;
    double q1, q2, dx, dy, r, r1, r2, d;


    StopThread(1, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            acc[i].ax[3] = 0;
            acc[i].ay[3] = 0;
        }
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            if(get_elec_data(&(Data->level.movable_objects[i]), &q1, &r1)){
                for(j = i + 1; j < Data->level.number_of_movable_objects; ++j){
                    if(get_elec_data(&(Data->level.movable_objects[j]), &q2, &r2)){
                        q2 *= q1;
                        if(q2){
                            dx = acc[j].x - acc[i].x;
                            dy = acc[j].y - acc[i].y;
                            d = dx * dx + dy * dy;
                            r = r1 + r2;
                            if(r * r > d){
                                d = sqrt(d);
                                q2 *= (COULOMB * d) / (r * r * r);
                            }else{
                                q2 *= (COULOMB / d);//add linear for collision
                            }
                            dy = vector_angle(dx, dy);
                            dx = cos(dy);
                            dy = sin(dy);
                            acc[i].ax[3] += -q2 * dx;
                            acc[i].ay[3] += -q2 * dy;
                            acc[j].ax[3] += q2 * dx;
                            acc[j].ay[3] += q2 * dy;
                        }
                    }
                }
            }
        }

        /**
            Signal and stop
            */

        StopThread(1, Data, thread);
    }
    printf("Closing thread #1\n");

    return NULL;
    #undef acc
    #undef Data
}

void* compute_collisions(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((game_shared_data*)argument)
    #define acc Data->level.acc

    double time;
    int i, j, k, l;
    collision_data coll;
    short int temp;
    fast_read_set primitive_done, movable_done;
    int collisions_this_turn;

    construct_fast_read_set(&primitive_done, Data->level.number_of_primitive_objects);
    construct_fast_read_set(&movable_done, Data->level.number_of_movable_objects);

    StopThread(2, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        time = 0;
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            Data->level.movable_objects[i].coll_with_fixed.time = EMPTY_COLLISION_TIME;
            for(j = Data->level.movable_objects[i].zones[0]; j <= Data->level.movable_objects[i].zones[2]; ++j){
                for(k = Data->level.movable_objects[i].zones[1]; k <= Data->level.movable_objects[i].zones[3]; ++k){
                    for(l = 0; l < Data->level.zones[j][k].number_of_primitives; ++l){
                        if(!is_marked(&primitive_done, Data->level.zones[j][k].primitives[l])){
                            set_mark(&primitive_done, Data->level.zones[j][k].primitives[l]);
                            coll = get_collision_with_primitive(&Data->level.movable_objects[i],
                                                                &Data->level.primitive_objects[Data->level.zones[j][k].primitives[l]]);
                            if(coll.time >= 0 && coll.time <= 1){
                                if(coll.time < Data->level.movable_objects[i].coll_with_fixed.time){
                                    coll.with = Data->level.zones[j][k].primitives[l];
                                    coll.who = (short int)i;
                                    Data->level.movable_objects[i].coll_with_fixed = coll;
                                }
                            }

                        }
                    }
                }
            }
            reset_marks(&primitive_done);
        }


        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            for(j = Data->level.movable_objects[i].zones[0]; j <= Data->level.movable_objects[i].zones[2]; ++j){
                for(k = Data->level.movable_objects[i].zones[1]; k <= Data->level.movable_objects[i].zones[3]; ++k){
                    for_each_higher_check_collision(&Data->level, &movable_done, (short int)i,
                                                    Data->level.zones[j][k].movable.root,
                                                    Data->level.zones[j][k].movable.nil, 0);
                }
            }
            collision_min_for_object(&Data->level, i);
            if(Data->level.movable_objects[i].next_collision->time >= 0 &&
               Data->level.movable_objects[i].next_collision->time <= 1){
                //Now, if the collision NOT is already in the queue...
                if(!(Data->level.movable_objects[i].next_collision->with_movable &&
                     Data->level.movable_objects[i].next_collision->with < Data->level.movable_objects[i].next_collision->who &&
                     coll_rev_comp(Data->level.movable_objects[i].next_collision,
                                   Data->level.movable_objects[Data->level.movable_objects[i].next_collision->with].next_collision) == EQUAL)){
                    heap_add(&Data->level.collision_queue, Data->level.movable_objects[i].next_collision);
                }
            }
            reset_marks(&movable_done);
        }
        build_heap(&Data->level.collision_queue);

        collisions_this_turn = 0;
        while(Data->level.collision_queue.length > 0){
            coll = pop_min(&Data->level.collision_queue);
            if(Data->level.dirty_tree.root != Data->level.dirty_tree.nil){
                //if a coll was put down due to getting something
                //sooner, then it must be here

                if(coll_delete_node(&Data->level.dirty_tree, &coll)){
                    continue;
                }
            }
            if(time > coll.time){
                continue;
            }
            move_objects(&Data->level, coll.time - time);
            time = coll.time;

            collide(&Data->level, coll.who, coll.with, coll.with_movable, Data->level.dt);
            if(Data->level.at_exit){
                break;
            }
            change_zones_for_movable(&Data->level, coll.who, 1 - time);

            coll_delete_node(&Data->level.movable_objects[coll.who].colls_with_mov, &coll);
            coll_clear_trash(&Data->level, Data->level.movable_objects[coll.who].colls_with_mov.root,
                                   Data->level.movable_objects[coll.who].colls_with_mov.nil);
            Data->level.movable_objects[coll.who].colls_with_mov.root = Data->level.movable_objects[coll.who].colls_with_mov.nil;

            if(coll.with_movable){
                temp = coll.with;
                coll.with = coll.who;
                coll.who = temp;
                change_zones_for_movable(&Data->level, coll.who, 1 - time);
                coll_delete_node(&Data->level.movable_objects[coll.who].colls_with_mov, &coll);
                coll_clear_trash(&Data->level, Data->level.movable_objects[coll.who].colls_with_mov.root,
                                       Data->level.movable_objects[coll.who].colls_with_mov.nil);
                Data->level.movable_objects[coll.who].colls_with_mov.root = Data->level.movable_objects[coll.who].colls_with_mov.nil;

                set_mark(&movable_done, coll.who);
                find_next_collision(&Data->level, coll.with, &primitive_done, &movable_done, time);
                set_mark(&movable_done, coll.with);
                find_next_collision(&Data->level, coll.who, &primitive_done, &movable_done, time);
            }else{
                set_mark(&primitive_done, coll.with);
                find_next_collision(&Data->level, coll.who, &primitive_done, &movable_done, time);
            }
            collisions_this_turn += 1;
            if(collisions_this_turn >= MAX_COLLISIONS_PER_TURN){
                clear_heap(&Data->level.collision_queue);
                break;
            }
        }

        if((time < 1) && !Data->level.at_exit){
            move_objects(&Data->level, 1 - time);
        }
        coll_clear_tree(&Data->level.dirty_tree);
        clear_heap(&Data->level.collision_queue);
        /**
            Signal and stop
            */
        StopThread(2, Data, thread);
    }
    destroy_fast_read_set(&primitive_done);
    destroy_fast_read_set(&movable_done);
    printf("Closing thread #2\n");

    return NULL;
    #undef acc
    #undef Data
}

/**
    Private methods
    */
bool get_drag_data(movable_object *obj, double *vx, double *vy, double *Cx, double *S){
    switch(obj->type){
        case motPLAYER:
            #define obj_data ((movable_player*)obj->object_data)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = obj_data->r * obj_data->r * PI;
            *vx = obj_data->vx;
            *vy = obj_data->vy;
            #undef obj_data
            return true;
        case motPARTICLE:
            #define obj_data ((movable_particle*)obj->object_data)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = obj_data->surface_field;
            *vx = obj_data->vx;
            *vy = obj_data->vy;
            #undef obj_data
            return true;
        default:
            return false;
    }
}

bool get_grav_data(movable_object *obj, double *mass, double *r){
    switch(obj->type){
        case motPLAYER:
            #define obj_data ((movable_player*)obj->object_data)
            *mass = obj_data->mass;
            *r = obj_data->r;
            #undef obj_data
            return true;
        case motPARTICLE:
            #define obj_data ((movable_particle*)obj->object_data)
            *mass = obj_data->mass;
            *r = obj_data->r;
            #undef obj_data
            return true;
        default:
            return false;
    }
}

bool get_elec_data(movable_object *obj, double *charge, double *r){
    switch(obj->type){
        case motPLAYER:
            #define obj_data ((movable_player*)obj->object_data)
            *charge = obj_data->charge;
            *r = obj_data->r;
            #undef obj_data
            return true;
        case motPARTICLE:
            #define obj_data ((movable_particle*)obj->object_data)
            *charge = obj_data->charge;
            *r = obj_data->r;
            #undef obj_data
            return true;
        default:
            return false;
    }
}
