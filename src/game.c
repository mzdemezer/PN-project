#include "main.h"
#include "menu.h"
#include "game.h"
#include <allegro5/allegro.h>
#include <math.h>
#include <stdio.h>

bool initialize_iteration_threads(struct GameSharedData *Data){
    int i;

    Data->ThreadMainIteration = NULL;
    Data->ThreadMainIteration = al_create_thread(main_iteration, (void*)Data);
    if(!Data->ThreadMainIteration){
        fprintf(stderr, "Failed to initialize main_iteration thread\n");
        return false;
    }
    Data->IterationThreads[0].Job = iteration_0;
    Data->IterationThreads[1].Job = iteration_1;
    Data->IterationThreads[2].Job = iteration_2;

    for(i = 0; i < NumOfThreads; ++i){
        Data->IterationThreads[i].Thread = al_create_thread(Data->IterationThreads[i].Job, (void*)Data);
        if(!Data->IterationThreads[i].Thread){
            fprintf(stderr, "Failed to initialize iteration#%d thread\n", i);
            return false;
        }
    }
    return true;
}



/**
    This procedure forces termination of all iteration threads
    and is enough to be assured, that they are closed. Also, it
    always shuts them down in a way, that the whole iteration
    cycle is done, never in between.
    */
void terminate_iteration(struct GameSharedData *Data){
    int i;
    /**
        Terminating small threads
        */
    al_lock_mutex(Data->MutexIterations);
        for(i = 0; i < NumOfThreads; ++i){
            al_set_thread_should_stop(Data->IterationThreads[i].Thread);
        }
        for(i = 0; i < NumOfThreads; ++i){
            Data->IterationThreads[i].Finished = false;
        }
        al_broadcast_cond(Data->CondIterations);
    al_unlock_mutex(Data->MutexIterations);
    for(i = 0; i < NumOfThreads; ++i){printf("waiting for #%d thread\n", i);
        al_destroy_thread(Data->IterationThreads[i].Thread);
    }
    /**
        Terminating MainIteration Thread
        */
    al_lock_mutex(Data->MutexMainIteration);
        al_set_thread_should_stop(Data->ThreadMainIteration);
        for(i = 0; i < NumOfThreads; ++i){
            Data->IterationThreads[i].Finished = true;
        }
        Data->IterationFinished = false;
        al_broadcast_cond(Data->CondMainIteration);
    al_unlock_mutex(Data->MutexMainIteration);
    al_destroy_thread(Data->ThreadMainIteration);
    /**
        Clean-up direly needed!!!!
        */
    clear_level(&Data->Level);
}

void* main_iteration(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define dt Data->Level.dt
    #define Acc Data->Level.Acc

    int i, j;
    bool f;
    double Cx, S, vx, vy;
    float op;

    /**
        Initialization
        */

    Data->MainIterationThreadsIsWaiting = true;
    for(i = 0; i < NumOfThreads; ++i){
        al_start_thread(Data->IterationThreads[i].Thread);
    }
    printf("In game: Threads started\n");
    al_lock_mutex(Data->MutexIterations);
        while(true){
            f = true;
            for(i = 0; i < NumOfThreads; ++i){
                if(!Data->IterationThreads[i].Finished){
                    f = false;
                    printf("In game: %d not ready\n", i);
                    break;
                }
            }
            if(f){
                break;
            }
            al_wait_cond(Data->CondMainIteration, Data->MutexIterations);
        }
    al_unlock_mutex(Data->MutexIterations);

    Data->Level.last_time = clock() / (float)CLOCKS_PER_SEC;

    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
        for(j = 0; j < ACC_2nd_DIM; ++j){
            Acc[i].ax[j] = 0;
            Acc[i].ay[j] = 0;
        }
    }
    bool parity = false,
         imparity = true;
    float half_dt;

    printf("In game: after main-iter-init, starting to operate\n");
    Data->Level.start_time = al_get_time();

    while(!al_get_thread_should_stop(thread)){
        /**
            Init
            */

        dt = Data->Level.last_time;
        Data->Level.last_time = al_get_time();
        dt = Data->Level.last_time - dt;
        if(dt > MAX_DT){
            /**
                Security against slowdowns, whatever the reason.
                dt affects zones, soo when it raises too much
                each object would demand all zones.
                */
            dt = MAX_DT;
        }
        half_dt = dt / 2;

        op = Data->Level.Player->engine_state * THROTTLE * dt;
        Data->Level.Player->vx += op * cos(Data->Level.Player->ang);
        Data->Level.Player->vy += op * sin(Data->Level.Player->ang);

        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            Acc[i].x = ((struct circleData*)(Data->Level.MovableObjects[i].ObjectData))->center.x;
            Acc[i].y = ((struct circleData*)(Data->Level.MovableObjects[i].ObjectData))->center.y;
            switch(Data->Level.MovableObjects[i].Type){
                case motPLAYER:
                    Data->Level.MovableObjects[i].dx = ((struct playerData*)Data->Level.MovableObjects[i].ObjectData)->vx * dt;
                    Data->Level.MovableObjects[i].dy = ((struct playerData*)Data->Level.MovableObjects[i].ObjectData)->vy * dt;
                    change_zones_for_movable(Data, i, 1.);
                    break;
                case motPARTICLE:
                    Data->Level.MovableObjects[i].dx = ((struct particleData*)Data->Level.MovableObjects[i].ObjectData)->vx * dt;
                    Data->Level.MovableObjects[i].dy = ((struct particleData*)Data->Level.MovableObjects[i].ObjectData)->vy * dt;
                    change_zones_for_movable(Data, i, 1.);
                    break;
                default:
                    ;
            }
        }

        /**
            Launching other iterations
            */
        Data->MainIterationThreadsIsWaiting = false;

        al_lock_mutex(Data->MutexIterations);
            for(i = 0; i < NumOfThreads; ++i){
                Data->IterationThreads[i].Finished = false;
            }
            al_broadcast_cond(Data->CondIterations);
        al_unlock_mutex(Data->MutexIterations);

        /**
            Main iteration thread main work
            */

        /**
            1. Handle keyboard input
            */
        al_lock_mutex(Data->Keyboard.MutexKeyboard);
        if(Data->Keyboard.Flags[ekKEY_DOWN]){
            if(Data->Keyboard.Flags[ekKEY_UP]){
                Data->Level.Player->engine_state /= 2;
            }
            else{
                if(Data->Level.Player->engine_state > 0){
                    Data->Level.Player->engine_state /= 2;
                }else{
                    if(Data->Level.Player->engine_state > MAX_DECELERATE){
                        Data->Level.Player->engine_state -= 1;
                    }
                }
            }
        }
        else if(Data->Keyboard.Flags[ekKEY_UP]){
            if(Data->Level.Player->engine_state < 0){
                Data->Level.Player->engine_state /= 2;
            }else{
                if(Data->Level.Player->engine_state < MAX_ACCELERATE){
                    Data->Level.Player->engine_state += 1;
                }
            }
        }else{
            if(Data->Level.Player->engine_state){
                Data->Level.Player->engine_state /= 2;
            }
        }



        if(Data->Keyboard.Flags[ekKEY_RIGHT]){
            if(!Data->Keyboard.Flags[ekKEY_LEFT]){
                Data->Level.Player->ang += dANG;
            }
        }
        else if(Data->Keyboard.Flags[ekKEY_LEFT]){
            Data->Level.Player->ang -= dANG;
        }

        al_unlock_mutex(Data->Keyboard.MutexKeyboard);
        /**
            Waiting until other threads finish
            */
        al_lock_mutex(Data->MutexIterations);
            Data->MainIterationThreadsIsWaiting = true;
            while(true){
                f = true;
                for(i = 0; i < NumOfThreads; ++i){
                    if(!Data->IterationThreads[i].Finished){
                        f = false;
                        break;
                    }
                }
                if(f){
                    break;
                }
                al_wait_cond(Data->CondMainIteration, Data->MutexIterations);
            }
        al_unlock_mutex(Data->MutexIterations);

        /**
            Main iteration thread after-work
            */

        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            if(get_drag_data(&(Data->Level.MovableObjects[i]), &vx, &vy, &Cx, &S)){
                Cx = S * Cx * Data->Level.dens;
                vx = Data->Level.wind_vx - vx;
                Acc[i].ax[4] = vx * 3 * Cx * coefficient_multiplier(vx);
                vy = Data->Level.wind_vy - vy;
                Acc[i].ay[4] = vy * 3 * Cx * coefficient_multiplier(vy);
            }
            switch(Data->Level.MovableObjects[i].Type){
                case motPLAYER:
                    #define ObData ((struct playerData*)(Data->Level.MovableObjects[i].ObjectData))
                    op = ObData->mass;
                    Acc[i].ax[(int)parity] = Acc[i].ax[2];
                    for(j = 3; j < ACC_2nd_DIM; ++j){
                        Acc[i].ax[(int)parity] += Acc[i].ax[j];
                    }
                    Acc[i].ax[(int)parity] /= op;

                    Acc[i].ay[(int)parity] = Acc[i].ay[2];
                    for(j = 3; j < ACC_2nd_DIM; ++j){
                        Acc[i].ay[(int)parity] += Acc[i].ay[j];
                    }
                    Acc[i].ay[(int)parity] /= op;

                    ObData->vx += (Acc[i].ax[(int)parity] + Acc[i].ax[(int)imparity]) * half_dt;
                    ObData->vy += (Acc[i].ay[(int)parity] + Acc[i].ay[(int)imparity]) * half_dt;

                    /**
                        Simple bounce - to delete this I need better separation algorithm
                        */
                    if(ObData->center.x - ObData->r <= 0){
                        ObData->center.x = ObData->r + ObData->r - ObData->center.x;
                        if(ObData->vx < 0){
                           ObData->vx *= -PLAYER_TO_WALL_RESTITUTION;
                        }
                    }else if(ObData->center.x + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.x = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.x + ObData->r + ObData->r);
                        if(ObData->vx > 0){
                           ObData->vx *= -PLAYER_TO_WALL_RESTITUTION;
                        }
                    }
                    if(ObData->center.y - ObData->r <= 0){
                        ObData->center.y = ObData->r + ObData->r - ObData->center.y;
                        if(ObData->vy < 0){
                           ObData->vy *= -PLAYER_TO_WALL_RESTITUTION;
                        }
                    }else if(ObData->center.y + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.y = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.y + ObData->r + ObData->r);
                        if(ObData->vy > 0){
                           ObData->vy *= -PLAYER_TO_WALL_RESTITUTION;
                        }
                    }
                    #undef ObData
                    break;
                case motPARTICLE:
                    #define ObData ((struct particleData*)(Data->Level.MovableObjects[i].ObjectData))
                    op = ObData->mass;
                    Acc[i].ax[(int)parity] = Acc[i].ax[2];
                    for(j = 3; j < ACC_2nd_DIM; ++j){
                        Acc[i].ax[(int)parity] += Acc[i].ax[j];
                    }
                    Acc[i].ax[(int)parity] /= op;

                    Acc[i].ay[(int)parity] = Acc[i].ay[2];
                    for(j = 3; j < ACC_2nd_DIM; ++j){
                        Acc[i].ay[(int)parity] += Acc[i].ay[j];
                    }
                    Acc[i].ay[(int)parity] /= op;

                    ObData->vx += (Acc[i].ax[(int)parity] + Acc[i].ax[(int)imparity]) * half_dt;
                    ObData->vy += (Acc[i].ay[(int)parity] + Acc[i].ay[(int)imparity]) * half_dt;

                    /**
                        Simple bounce - just in case, segments are not perfect!
                        */
                    if(ObData->center.x - ObData->r <= 0){
                        ObData->center.x = ObData->r + ObData->r - ObData->center.x;
                        if(ObData->vx < 0){
                           ObData->vx *= -PARTICLE_TO_WALL_RESTITUTION;
                        }
                    }else if(ObData->center.x + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.x = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.x + ObData->r + ObData->r);
                        if(ObData->vx > 0){
                           ObData->vx *= -PARTICLE_TO_WALL_RESTITUTION;
                        }
                    }
                    if(ObData->center.y - ObData->r <= 0){
                        ObData->center.y = ObData->r + ObData->r - ObData->center.y;
                        if(ObData->vy < 0){
                           ObData->vy *= -PARTICLE_TO_WALL_RESTITUTION;
                        }
                    }else if(ObData->center.y + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.y = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.y + ObData->r + ObData->r);
                        if(ObData->vy > 0){
                           ObData->vy *= -PARTICLE_TO_WALL_RESTITUTION;
                        }
                    }
                    #undef ObData
                    break;
                case motSWITCH:
                    break;
                case motDOOR:
                    break;
            }
        }

        /**
            Ready to draw
            */
        al_lock_mutex(Data->MutexDrawCall);
            Data->DrawCall = true;
        al_unlock_mutex(Data->MutexDrawCall);

        /**
            After-after work, that can be done
            after sending ready-draw signal;
            clean-up
            */
        parity = !parity;
        imparity = !imparity;
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            coll_clear_tree(&Data->Level.MovableObjects[i].colls_with_mov);
        }

        if(!al_get_thread_should_stop(thread)){
            al_lock_mutex(Data->MutexMainIteration);
                Data->IterationFinished = true;
                if(Data->SynchronizeWithMainIteration){
                    Data->SynchronizeWithMainIteration = false;
                    al_broadcast_cond(Data->CondSynchronizeWithMainIteration);
                }
                while(Data->IterationFinished){
                    al_wait_cond(Data->CondMainIteration, Data->MutexMainIteration);
                }
            al_unlock_mutex(Data->MutexMainIteration);
        }
    }
    printf("Closing Thread: main-iteration\n");

    return NULL;
    #undef Acc
    #undef dt
    #undef Data
}

/**
    This function signals that the  Thread[i] finished its work
    and then sets it to wait
    */

void StopThread(int i, struct GameSharedData *Data, ALLEGRO_THREAD *thread){
    al_lock_mutex(Data->MutexIterations);
        Data->IterationThreads[i].Finished = true;
        if(Data->MainIterationThreadsIsWaiting){
            al_broadcast_cond(Data->CondMainIteration);
        }
        while(Data->IterationThreads[i].Finished && !al_get_thread_should_stop(thread)){
            al_wait_cond(Data->CondIterations, Data->MutexIterations);
        }
    al_unlock_mutex(Data->MutexIterations);
}

/**
    Gravity
    */
bool get_grav_data(struct movable_object_structure *Obj, double *mass, double *r){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *mass = ObData->mass;
            *r = ObData->r;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *mass = ObData->mass;
            *r = ObData->r;
            #undef ObData
            return true;
        default:
            return false;
    }
}

void* iteration_0(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc
    int i, j;
    double m1, m2, dx, dy, r1, r2, d, r;

    StopThread(0, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            Acc[i].ax[2] = 0;
            Acc[i].ay[2] = 0;
        }
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            if(get_grav_data(&(Data->Level.MovableObjects[i]), &m1, &r1)){
                for(j = i + 1; j < Data->Level.number_of_movable_objects; ++j){
                    if(get_grav_data(&(Data->Level.MovableObjects[j]), &m2, &r2)){
                        dx = (double)Acc[j].x - (double)Acc[i].x;
                        dy = (double)Acc[j].y - (double)Acc[i].y;
                        d = dx * dx + dy * dy;
                        r = r1 + r2;
                        if(r * r > d){
                            d = sqrt(d);
                            m2 *= m1 * ((GRAV * d) / (r * r * r));
                        }else{
                            m2 *= m1 * (GRAV / d);
                        }
                        dy = VectorAngle(dx, dy);
                        dx = cos(dy);
                        dy = sin(dy);
                        Acc[i].ax[2] += m2 * dx;
                        Acc[i].ay[2] += m2 * dy;
                        Acc[j].ax[2] += -m2 * dx;
                        Acc[j].ay[2] += -m2 * dy;
                    }
                }
            }
        }

        /**
            Signal and stop
            */

        StopThread(0, Data, thread);
    }
    printf("Closing Thread #0\n");

    return NULL;
    #undef Acc
    #undef Data
}


/**
    Electrostatics
    */

bool get_elec_data(struct movable_object_structure *Obj, double *charge, double *r){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *charge = ObData->charge;
            *r = ObData->r;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *charge = ObData->charge;
            *r = ObData->r;
            #undef ObData
            return true;
        default:
            return false;
    }
}

void* iteration_1(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc
    int i, j;
    double q1, q2, dx, dy, r, r1, r2, d;


    StopThread(1, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            Acc[i].ax[3] = 0;
            Acc[i].ay[3] = 0;
        }
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            if(get_elec_data(&(Data->Level.MovableObjects[i]), &q1, &r1)){
                for(j = i + 1; j < Data->Level.number_of_movable_objects; ++j){
                    if(get_elec_data(&(Data->Level.MovableObjects[j]), &q2, &r2)){
                        q2 *= q1;
                        if(q2){
                            dx = (double)Acc[j].x - (double)Acc[i].x;
                            dy = (double)Acc[j].y - (double)Acc[i].y;
                            d = dx * dx + dy * dy;
                            r = r1 + r2;
                            if(r * r > d){
                                d = sqrt(d);
                                q2 *= (COULOMB * d) / (r * r * r);
                            }else{
                                q2 *= (COULOMB / d);//add linear for collision
                            }
                            dy = VectorAngle(dx, dy);
                            dx = cos(dy);
                            dy = sin(dy);
                            Acc[i].ax[3] += -q2 * dx;
                            Acc[i].ay[3] += -q2 * dy;
                            Acc[j].ax[3] += q2 * dx;
                            Acc[j].ay[3] += q2 * dy;
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
    printf("Closing Thread #1\n");

    return NULL;
    #undef Acc
    #undef Data
}

/**
    Fluid resistance
    basically bullshit, but it seems to work pretty well
    */

bool get_drag_data(struct movable_object_structure *Obj, double *vx, double *vy, double *Cx, double *S){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = ObData->r * ObData->r * PI;
            *vx = ObData->vx;
            *vy = ObData->vy;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = ObData->surface_field;
            *vx = ObData->vx;
            *vy = ObData->vy;
            #undef ObData
            return true;
        default:
            return false;
    }
}

double coefficient_multiplier(double v){
    v = double_abs(v) / MACH_SPEED;
    if(v < 1){
        return exp(v * 1.3862943611198906);
    }else if(v < 1.5){
        v -= 1;
        return -4 * v * v + 4.0;
    }else{
        return 0.538859 * exp(-v * 1.098612 + 3.295837) + 0.2;
    }
}

/**
    Collision thread
    */
void* iteration_2(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc

    float time;
    int i, j, k, l;
    struct collision_data coll;
    short int temp;
    fast_read_set primitive_done, movable_done;
    int collisions_this_turn;

    initialize_fast_read_set(&primitive_done, Data->Level.number_of_primitive_objects);
    initialize_fast_read_set(&movable_done, Data->Level.number_of_movable_objects);

    StopThread(2, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        time = 0;
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            Data->Level.MovableObjects[i].coll_with_fixed.time = EMPTY_COLLISION_TIME;
            for(j = Data->Level.MovableObjects[i].zones[0]; j <= Data->Level.MovableObjects[i].zones[2]; ++j){
                for(k = Data->Level.MovableObjects[i].zones[1]; k <= Data->Level.MovableObjects[i].zones[3]; ++k){
                    for(l = 0; l < Data->Level.zones[j][k].number_of_primitives; ++l){
                        if(!is_marked(&primitive_done, Data->Level.zones[j][k].primitives[l])){
                            set_mark(&primitive_done, Data->Level.zones[j][k].primitives[l]);
                            coll = get_collision_with_primitive(&Data->Level.MovableObjects[i],
                                                                &Data->Level.PrimitiveObjects[Data->Level.zones[j][k].primitives[l]]);
                            if(coll.time >= 0 && coll.time <= 1){
                                if(coll.time < Data->Level.MovableObjects[i].coll_with_fixed.time){
                                    coll.with = Data->Level.zones[j][k].primitives[l];
                                    coll.who = (short int)i;
                                    Data->Level.MovableObjects[i].coll_with_fixed = coll;
                                }
                            }

                        }
                    }
                }
            }
            reset_marks(&primitive_done);
        }


        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            for(j = Data->Level.MovableObjects[i].zones[0]; j <= Data->Level.MovableObjects[i].zones[2]; ++j){
                for(k = Data->Level.MovableObjects[i].zones[1]; k <= Data->Level.MovableObjects[i].zones[3]; ++k){
                    for_each_higher_check_collision(Data, &movable_done, (short int)i,
                                                    Data->Level.zones[j][k].movable.root,
                                                    Data->Level.zones[j][k].movable.nil, 0);
                }
            }
            collision_min_for_object(Data, i);
            if(Data->Level.MovableObjects[i].next_collision->time >= 0 &&
               Data->Level.MovableObjects[i].next_collision->time <= 1){
                if(!(Data->Level.MovableObjects[i].next_collision->with_movable &&
                     Data->Level.MovableObjects[i].next_collision->with < Data->Level.MovableObjects[i].next_collision->who &&
                     coll_rev_comp(Data->Level.MovableObjects[i].next_collision,
                                   Data->Level.MovableObjects[Data->Level.MovableObjects[i].next_collision->with].next_collision) == EQUAL)){
                    heap_insert(&Data->Level.collision_queue, Data->Level.MovableObjects[i].next_collision);
                }
            }
            reset_marks(&movable_done);
        }

        collisions_this_turn = 0;
        while(Data->Level.collision_queue.length > 0){
            coll = pop_min(&Data->Level.collision_queue);
            if(Data->Debug){
                sprintf(Data->DeBuffer, "last coll: %.3f, %hd - %hd, %d", coll.time,
                                      coll.who,
                                      coll.with,
                                      (int)coll.with_movable);
            }
            if(Data->Level.dirty_tree.root != Data->Level.dirty_tree.nil){
                //check if dirty
                //for each: who is the one, with is the other one
                //if a coll was put down due to getting something
                //sooner, then it must be here

                if(coll_delete_node(&Data->Level.dirty_tree, &coll)){
                    continue;
                }
            }
            if(time > coll.time){
                continue;
            }
            move_objects(Data, coll.time - time);
            time = coll.time;
//                printf("buggey: %.3f, %hd - %hd, %d\n", coll.time,
//                                                  coll.who,
//                                                  coll.with,
//                                                  (int)coll.with_movable);
            //Collide  avec:
            //new dx,dy
            collide(Data, coll.who, coll.with, coll.with_movable, Data->Level.dt);
            change_zones_for_movable(Data, coll.who, 1 - time);

            //clear collision tree
            coll_delete_node(&Data->Level.MovableObjects[coll.who].colls_with_mov, &coll);
            coll_clear_trash(Data, Data->Level.MovableObjects[coll.who].colls_with_mov.root,
                                   Data->Level.MovableObjects[coll.who].colls_with_mov.nil);
            Data->Level.MovableObjects[coll.who].colls_with_mov.root = Data->Level.MovableObjects[coll.who].colls_with_mov.nil;

            if(coll.with_movable){
                //if with_movable, the same for him
                //new dx, dy from collision
                temp = coll.with;
                coll.with = coll.who;
                coll.who = temp;
                change_zones_for_movable(Data, coll.who, 1 - time);
                coll_delete_node(&Data->Level.MovableObjects[coll.who].colls_with_mov, &coll);
                coll_clear_trash(Data, Data->Level.MovableObjects[coll.who].colls_with_mov.root,
                                       Data->Level.MovableObjects[coll.who].colls_with_mov.nil);
                Data->Level.MovableObjects[coll.who].colls_with_mov.root = Data->Level.MovableObjects[coll.who].colls_with_mov.nil;

                //New collisions
                set_mark(&movable_done, coll.who);
                find_next_collision(Data, coll.with, &primitive_done, &movable_done, time);
                set_mark(&movable_done, coll.with);
                find_next_collision(Data, coll.who, &primitive_done, &movable_done, time);
            }else{
                set_mark(&primitive_done, coll.with);
                find_next_collision(Data, coll.who, &primitive_done, &movable_done, time);
            }
            collisions_this_turn += 1;
            if(collisions_this_turn >= MAX_COLLISIONS_PER_TURN){
                clear_heap(&Data->Level.collision_queue);
                break;
            }
        }

        if(time < 1){
            move_objects(Data, 1 - time);
        }
        coll_clear_tree(&Data->Level.dirty_tree);
        /**
            Signal and stop
            */
        StopThread(2, Data, thread);
    }
    destroy_fast_read_set(&primitive_done);
    destroy_fast_read_set(&movable_done);
    printf("Closing Thread #2\n");

    return NULL;
    #undef Acc
    #undef Data
}

void handle_event_game(struct GameSharedData *Data){
    switch(Data->LastEvent.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->CloseLevel = true;
            Data->CloseNow = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            switch(Data->LastEvent.keyboard.keycode){
                case ALLEGRO_KEY_ESCAPE:
                    al_lock_mutex(Data->MutexMainIteration);
                        al_lock_mutex(Data->MutexChangeState);
                            Data->NewState = gsPAUSE;
                            Data->RequestChangeState = true;
                        al_unlock_mutex(Data->MutexChangeState);
                        if(!Data->IterationFinished){
                                Data->SynchronizeWithMainIteration = true;
                                while(Data->SynchronizeWithMainIteration){
                                    al_wait_cond(Data->CondSynchronizeWithMainIteration,
                                                 Data->MutexMainIteration);
                                }
                        }
                    al_unlock_mutex(Data->MutexMainIteration);
                    Data->Level.sum_time += al_get_time() - Data->Level.start_time;
                    printf("Synchronized with main iteration that is now waiting\n");
                    break;
                default:
                    al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyUp){
                        Data->Keyboard.Flags[ekKEY_UP] = true;
                    }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyDown){
                        Data->Keyboard.Flags[ekKEY_DOWN] = true;
                    }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyLeft){
                        Data->Keyboard.Flags[ekKEY_LEFT] = true;
                    }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyRight){
                        Data->Keyboard.Flags[ekKEY_RIGHT] = true;
                    }else if(Data->LastEvent.keyboard.keycode == ALLEGRO_KEY_TILDE){
                        Data->Debug = !Data->Debug;
                    }
                    al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }
            break;
        case ALLEGRO_EVENT_KEY_UP:
            al_lock_mutex(Data->Keyboard.MutexKeyboard);
            if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyUp){
                Data->Keyboard.Flags[ekKEY_UP] = false;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyDown){
                Data->Keyboard.Flags[ekKEY_DOWN] = false;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyLeft){
                Data->Keyboard.Flags[ekKEY_LEFT] = false;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyRight){
                Data->Keyboard.Flags[ekKEY_RIGHT] = false;
            }
            al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            break;
    }
}

void highlight_zone(struct GameSharedData *Data, short int x, short int y, ALLEGRO_COLOR color){
    #define OFFSET 0.5
    al_draw_filled_rectangle(x * ZONE_SIZE + OFFSET + Data->scales.trans_x, y * ZONE_SIZE + OFFSET + Data->scales.trans_y,
                        (x + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_x, (y + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_y,
                        color);
    #undef OFFSET
}

void draw_zones(struct GameSharedData *Data, struct movable_object_structure *Obj, ALLEGRO_COLOR color){
    #define OFFSET 0.5
    al_draw_filled_rectangle(Obj->zones[0] * ZONE_SIZE + OFFSET + Data->scales.trans_x, Obj->zones[1] * ZONE_SIZE + OFFSET + Data->scales.trans_y,
                        (Obj->zones[2] + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_x, (Obj->zones[3] + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_y,
                        color);
    #undef OFFSET
}

void draw_grid(struct GameSharedData *Data){
    #define OFFSET 0.5
    short int i;
    for(i = 1; i < ZONE_FACTOR; ++i){
        al_draw_line(OFFSET + Data->scales.trans_x + i * ZONE_SIZE, OFFSET + Data->scales.trans_y,
                     OFFSET + Data->scales.trans_x + i * ZONE_SIZE, OFFSET + Data->scales.trans_y + SCREEN_BUFFER_HEIGHT,
                     al_map_rgba(0, 0, 0, 70), 1);
        al_draw_line(OFFSET + Data->scales.trans_x, OFFSET + Data->scales.trans_y + i * ZONE_SIZE,
                     OFFSET + Data->scales.trans_x + SCREEN_BUFFER_WIDTH, OFFSET + Data->scales.trans_y + i * ZONE_SIZE,
                     al_map_rgba(0, 0, 0, 70), 1);
    }
    #undef OFFSET
}

void draw_arrow(struct GameSharedData *Data, float cx, float cy, float ang, int size, ALLEGRO_COLOR color){
    float fx = cx + size * 0.5 * cos(ang),
          fy = cy + size * 0.5 * sin(ang);

    size *= 0.5;
    al_draw_line(fx, fy,
                 cx - size * cos(ang), cy - size * sin(ang),
                 color, 1.5);
    size *= 0.7;
    al_draw_line(fx, fy,
                 cx + size * cos(ang + dANG), cy + size * sin(ang + dANG),
                 color, 1.5);
    al_draw_line(fx, fy,
                 cx + size * cos(ang - dANG), cy + size * sin(ang - dANG),
                 color, 1.5);
}

void draw_game(struct GameSharedData *Data){
    int i, j, k;
    ALLEGRO_TRANSFORM tempT;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    al_draw_bitmap(Data->Level.ScaledBackground, Data->scales.scale_x, Data->scales.scale_y, 0);
    al_use_transform(&Data->Transformation);

    if(Data->Debug){
        for(i = 0; i < ZONE_FACTOR; ++i){
            for(j = 0; j < ZONE_FACTOR; ++j){
                for(k = 0; k < Data->Level.zones[i][j].number_of_primitives; ++k){
                    highlight_zone(Data, i, j, al_map_rgba(30, 3, 0, 0.01));
                }
            }
        }
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            draw_zones(Data, &Data->Level.MovableObjects[i], al_map_rgba(150, 90, 30, 0.01));
        }
        draw_grid(Data);
    }

    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
       DRAW_MOVABLE(Data->Level.MovableObjects[i]);
    }


    draw_stat_bar(Data);

    if(Data->Debug){
        if(Data->DeBuffer){
            if(Data->DeFont){
                al_use_transform(&tempT);
                al_draw_text(Data->DeFont, al_map_rgba(255, 255, 255, 0.01),
                             Data->scales.scale_w + Data->scales.scale_x, Data->scales.scale_y + Data->scales.scale_h * 0.9,
                             ALLEGRO_ALIGN_RIGHT, Data->DeBuffer);
                al_use_transform(&Data->Transformation);
            }
            draw_arrow(Data, SCREEN_BUFFER_HEIGHT + 30 + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT * 0.58 + Data->scales.trans_y, Data->DeCollAngs[0], 60, al_map_rgba(0, 0, 160, 0.01));
            draw_arrow(Data, SCREEN_BUFFER_HEIGHT + 120 + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT * 0.58 + Data->scales.trans_y, Data->DeCollAngs[1], 60, al_map_rgba(0, 0, 160, 0.01));
            draw_arrow(Data, SCREEN_BUFFER_HEIGHT + 30 + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT * 0.7 + Data->scales.trans_y, Data->DeCollAngs[2], 60, al_map_rgba(160, 90, 10, 0.01));
            draw_arrow(Data, SCREEN_BUFFER_HEIGHT + 120 + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT * 0.7 + Data->scales.trans_y, Data->DeCollAngs[3], 60, al_map_rgba(160, 90, 10, 0.01));
        }
    }
}

void clear_stat_bar(struct GameSharedData *Data){
    al_draw_filled_rectangle(SCREEN_BUFFER_HEIGHT + Data->scales.trans_x, Data->scales.trans_y,
                             SCREEN_BUFFER_WIDTH + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT + Data->scales.trans_y,
                             al_map_rgb(45,0,0));
}

void print_time(char *buffer, double time){
    int help = ((int)time) / 3600;

    time -= help * 3600;
    sprintf(buffer, "%02d:", help % 100);

    help = ((int)time) / 60;
    time -= help * 60;
    sprintf(buffer + 3, "%02d:%04.1lf", help, time);
    buffer[8] = '`'; //depending on the font
}

void draw_time(struct GameSharedData *Data){
    char buffer[11];
    double actual_time = al_get_time() - Data->Level.start_time + Data->Level.sum_time;
    ALLEGRO_TRANSFORM tempT;

    print_time(buffer, actual_time);
    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    al_draw_text(Data->TimeFont, al_map_rgb(255, 255, 255),
                 Data->scales.scale_w * 0.99 + Data->scales.scale_x,
                 Data->scales.scale_h / 25 + Data->scales.scale_y,
                 ALLEGRO_ALIGN_RIGHT, buffer);
    al_use_transform(&Data->Transformation);
}

void draw_stat_bar(struct GameSharedData *Data){
    clear_stat_bar(Data);
    draw_time(Data);
}

void request_game(struct GameSharedData *Data){
    int i;

    switch(Data->GameState){
        case gsLOADING:
            /**
                Game init
                */
            for(i = 0; i < NUMBER_OF_SIGNIFICANT_KEYS; ++i){
                Data->Keyboard.Flags[i] = false;
            }
            Data->RequestChangeState = false;
            Data->GameState = gsGAME;
            Data->DrawFunction = draw_game;

            al_destroy_thread(Data->ThreadLoading);
            al_start_thread(Data->ThreadMainIteration);
            break;
        case gsPAUSE:
            Data->RequestChangeState = false;
            Data->GameState = gsGAME;
            Data->DrawFunction = draw_game;
            Data->Level.last_time = al_get_time() - 1./MAX_FPS;
            Data->Level.start_time = al_get_time();
            printf("Unpause!\n\n");
            break;
        default:
            break;
    }
}

void request_pause(struct GameSharedData *Data){
    Data->RequestChangeState = false;
    Data->GameState = gsPAUSE;
    Data->DrawFunction = draw_pause;
    make_main_menu_pause_menu(Data);
}

float VectorAngle(float x, float y){
   if(x != 0){
		float b;
		b = atan(float_abs(y / x));
		if(y == 0){
			if(x < 0){
				b = PI;
			}
		}
		else if(x < 0){
			if(y > 0){
				b = PI - b;
			}else{
				b += PI;
			}
		}
		else if(y < 0){
			b = dwaPI - b;
		}
        return b;
    }
    else{
		if(y == 0){
			return 0;
		}else if(y > 0){
			return PIpol;
		}else{
            return PI32;
		}
    }
}
