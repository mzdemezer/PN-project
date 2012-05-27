#include "main.h"
#include "game.h"
#include <allegro5/allegro.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

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
printf("Small threads closed, waiting for Main-iter-thread\n");

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

}

void* main_iteration(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define dt Data->Level.dt
    #define Acc Data->Level.Acc

    int i, j;
    bool f;

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

    while(!al_get_thread_should_stop(thread)){
        /**
            Init
            */

        dt = Data->Level.last_time;
        Data->Level.last_time = clock() / (float)CLOCKS_PER_SEC;
        dt = Data->Level.last_time - dt;
        half_dt = dt / 2;

        op = Data->Level.Player->engine_state * THROTTLE * dt;
        Data->Level.Player->vx += op * cos(Data->Level.Player->ang);
        Data->Level.Player->vy += op * sin(Data->Level.Player->ang);

        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            Acc[i].x = ((struct circleData*)(Data->Level.MovableObjects[i].ObjectData))->center.x;
            Acc[i].y = ((struct circleData*)(Data->Level.MovableObjects[i].ObjectData))->center.y;
            switch(Data->Level.MovableObjects[i].Type){
                case motPLAYER:
                    Acc[i].vx = ((struct playerData*)(Data->Level.MovableObjects[i].ObjectData))->vx;
                    Acc[i].vy = ((struct playerData*)(Data->Level.MovableObjects[i].ObjectData))->vy;
                    Data->Level.MovableObjects[i].dx = Acc[i].vx * dt;
                    Data->Level.MovableObjects[i].dy = Acc[i].vy * dt;
                    change_zones_for_movable(Data, i, 1.);
                    break;
                case motPARTICLE:
                    Acc[i].vx = ((struct particleData*)(Data->Level.MovableObjects[i].ObjectData))->vx;
                    Acc[i].vy = ((struct particleData*)(Data->Level.MovableObjects[i].ObjectData))->vy;
                    Data->Level.MovableObjects[i].dx = Acc[i].vx * dt;
                    Data->Level.MovableObjects[i].dy = Acc[i].vy * dt;
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
                        Simple bounce
                        */
                    if(ObData->center.x - ObData->r <= 0){
                        ObData->center.x = ObData->r + ObData->r - ObData->center.x;
                        if(ObData->vx < 0){
                           ObData->vx *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }else if(ObData->center.x + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.x = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.x + ObData->r + ObData->r);
                        if(ObData->vx > 0){
                           ObData->vx *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }
                    if(ObData->center.y - ObData->r <= 0){
                        ObData->center.y = ObData->r + ObData->r - ObData->center.y;
                        if(ObData->vy < 0){
                           ObData->vy *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }else if(ObData->center.y + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.y = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.y + ObData->r + ObData->r);
                        if(ObData->vy > 0){
                           ObData->vy *= -WALL_COLLISION_COEFFICIENT;
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
                        Simple bounce
                        */
                    if(ObData->center.x - ObData->r <= 0){
                        ObData->center.x = ObData->r + ObData->r - ObData->center.x;
                        if(ObData->vx < 0){
                           ObData->vx *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }else if(ObData->center.x + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.x = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.x + ObData->r + ObData->r);
                        if(ObData->vx > 0){
                           ObData->vx *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }
                    if(ObData->center.y - ObData->r <= 0){
                        ObData->center.y = ObData->r + ObData->r - ObData->center.y;
                        if(ObData->vy < 0){
                           ObData->vy *= -WALL_COLLISION_COEFFICIENT;
                        }
                    }else if(ObData->center.y + ObData->r >= SCREEN_BUFFER_HEIGHT){
                        ObData->center.y = 2 * SCREEN_BUFFER_HEIGHT - (ObData->center.y + ObData->r + ObData->r);
                        if(ObData->vy > 0){
                           ObData->vy *= -WALL_COLLISION_COEFFICIENT;
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

        parity = !parity;
        imparity = !imparity;

        if(!al_get_thread_should_stop(thread)){
            al_lock_mutex(Data->MutexMainIteration);
                Data->IterationFinished = true;
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
bool get_grav_data(struct movable_object_structure *Obj, double *mass){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *mass = ObData->mass;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *mass = ObData->mass;
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
    double m1, m2, dx, dy;

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
            if(get_grav_data(&(Data->Level.MovableObjects[i]), &m1)){
                for(j = i + 1; j < Data->Level.number_of_movable_objects; ++j){
                    if(get_grav_data(&(Data->Level.MovableObjects[j]), &m2)){
                        dx = (double)Acc[j].x - (double)Acc[i].x;
                        dy = (double)Acc[j].y - (double)Acc[i].y;
                        m2 *= m1 * (GRAV / (dx * dx + dy * dy));//add linear for collision
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

bool get_elec_data(struct movable_object_structure *Obj, double *charge){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *charge = ObData->charge;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *charge = ObData->charge;
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
    double q1, q2, dx, dy;


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
            if(get_elec_data(&(Data->Level.MovableObjects[i]), &q1)){
                for(j = i + 1; j < Data->Level.number_of_movable_objects; ++j){
                    if(get_elec_data(&(Data->Level.MovableObjects[j]), &q2)){
                        q2 *= q1;
                        if(q2){
                            dx = (double)Acc[j].x - (double)Acc[i].x;
                            dy = (double)Acc[j].y - (double)Acc[i].y;
                            q2 *= (COULOMB / (dx * dx + dy * dy));//add linear for collision
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

bool get_drag_data(struct movable_object_structure *Obj, double *Cx, double *S){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = ObData->r * ObData->r * PI;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *Cx = SPHERE_DRAG_COEFFICENT;
            *S = ObData->surface_field;
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

void* iteration_2(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc

    int i;
    double vx, vy, Cx, S;

    StopThread(2, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            if(get_drag_data(&(Data->Level.MovableObjects[i]), &Cx, &S)){
                vx = (double)Acc[i].vx - Data->Level.wind_vx;
                vy = (double)Acc[i].vy - Data->Level.wind_vy;
                Cx = S * Cx * Data->Level.dens;
                Acc[i].ax[4] = -vx * Cx * coefficient_multiplier(vx);
                Acc[i].ay[4] = -vy * Cx * coefficient_multiplier(vy);
            }
        }
        /**
            Signal and stop
            */

        StopThread(2, Data, thread);
    }
    printf("Closing Thread #2\n");

    return NULL;
    #undef Acc
    #undef Data
}

/**
    Collision thread
    */
void* iteration_3(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc

    float time;
    int i, j, k, l;
    struct collision_data coll;
    short int temp;
    bool fixed_done[Data->Level.number_of_fixed_objects],
         movable_done[Data->Level.number_of_movable_objects];

    StopThread(3, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        time = 0;
        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            for(j = 0; j < Data->Level.number_of_fixed_objects; ++j){
                fixed_done[j] = false;
            }
            for(j = Data->Level.MovableObjects[i].zones[0]; j <= Data->Level.MovableObjects[i].zones[2]; ++j){
                for(k = Data->Level.MovableObjects[i].zones[1]; k <= Data->Level.MovableObjects[i].zones[3]; ++k){
                    for(l = 0; l < Data->Level.zones[j][k].number_of_fixed; ++l){
                        if(!fixed_done[Data->Level.zones[j][k].fixed[l]]){
                            fixed_done[Data->Level.zones[j][k].fixed[l]] = true;
                            coll = get_collision_with_fixed(&Data->Level.MovableObjects[i],
                                                                &Data->Level.FixedObjects[Data->Level.zones[j][k].fixed[l]]);
                            if(coll.time >= 0 && coll.time <= 1){
                                if(coll.time < Data->Level.MovableObjects[i].coll_with_fixed.time){
                                    coll.with = Data->Level.zones[j][k].fixed[l];
                                    Data->Level.MovableObjects[i].coll_with_fixed = coll;
                                }
                            }
                        }
                    }
                }
            }
        }

        for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
            for(j = 0; j < Data->Level.number_of_movable_objects; ++j){
                movable_done[j] = false;
            }
            ;//find among movables
        }

        while(Data->Level.collision_queue.length > 0){
            coll = pop_min(&Data->Level.collision_queue);
            if(Data->Level.dirty_tree.root != Data->Level.dirty_tree.nil){
                //check if dirty
                //for each: who is the one, with is the other one
                //if a coll was put down due to getting something
                //sooner, then it must be here

                if(coll_delete_node(&Data->Level.dirty_tree, &coll)){
                    continue;
                }
            }
            move_objects(Data, coll.time - time);
            time = coll.time;
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
                find_next_collision(Data, coll.with, coll.who, fixed_done, movable_done, time);
                find_next_collision(Data, coll.who, coll.with, fixed_done, movable_done, time);
            }else{
                find_next_collision(Data, coll.who, -coll.with, fixed_done, movable_done, time);
            }
        }

        if(time < 1){
            move_objects(Data, 1 - time);
        }
        /**
            Signal and stop
            */

        StopThread(3, Data, thread);
    }
    printf("Closing Thread #3\n");

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
            al_lock_mutex(Data->Keyboard.MutexKeyboard);
            if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyUp){
                Data->Keyboard.Flags[ekKEY_UP] = true;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyDown){
                Data->Keyboard.Flags[ekKEY_DOWN] = true;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyLeft){
                Data->Keyboard.Flags[ekKEY_LEFT] = true;
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyRight){
                Data->Keyboard.Flags[ekKEY_RIGHT] = true;
            }
            al_unlock_mutex(Data->Keyboard.MutexKeyboard);
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

void draw_game(struct GameSharedData *Data){
    int i;
    ALLEGRO_TRANSFORM tempT;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    al_draw_bitmap(Data->Level.ScaledBackground, Data->scales.scale_x, Data->scales.scale_y, 0);
    al_use_transform(&Data->Transformation);

    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
       DRAW_MOVABLE(Data->Level.MovableObjects[i]);
    }

    draw_stat_bar(Data);
}

void draw_stat_bar(struct GameSharedData *Data){
    al_draw_filled_rectangle(SCREEN_BUFFER_HEIGHT + Data->scales.trans_x, Data->scales.trans_y,
                             SCREEN_BUFFER_WIDTH + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT + Data->scales.trans_y,
                             al_map_rgb(45,0,0));
}

void request_game(struct GameSharedData *Data){
    int i;

    /**
        Game init
        */
    for(i = 0; i < NUMBER_OF_SIGNIFICANT_KEYS; ++i){
        Data->Keyboard.Flags[i] = false;
    }
    Data->RequestChangeState = false;
    Data->GameState = gsGAME;

    al_start_thread(Data->ThreadMainIteration);
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
