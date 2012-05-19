#include "main.h"
#include "game.h"
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

                    ObData->center.x += ObData->vx * dt;
                    ObData->center.y += ObData->vy * dt;
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

                    ObData->center.x += ObData->vx * dt;
                    ObData->center.y += ObData->vy * dt;
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
bool get_grav_data(struct movable_object_structure *Obj, double *mass, double *x, double *y){
    switch(Obj->Type){
        case motPLAYER:
            #define ObData ((struct playerData*)Obj->ObjectData)
            *mass = ObData->mass;
            *x = ObData->center.x;
            *y = ObData->center.y;
            #undef ObData
            return true;
        case motPARTICLE:
            #define ObData ((struct particleData*)Obj->ObjectData)
            *mass = ObData->mass;
            *x = ObData->center.x;
            *y = ObData->center.y;
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
    double m1, m2, x1, y1, x2, y2, ang;

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
            if(get_grav_data(&(Data->Level.MovableObjects[i]), &m1, &x1, &y1)){
                for(j = i + 1; j < Data->Level.number_of_movable_objects; ++j){
                    if(get_grav_data(&(Data->Level.MovableObjects[j]), &m2, &x2, &y2)){
                        x2 -= x1;
                        y2 -= y1;
                        m2 *= m1 * (GRAV / (x2 * x2 + y2 * y2));//add linear for collision
                        ang = VectorAngle(x2, y2);
                        x2 = cos(ang);
                        ang = sin(ang);
                        Acc[i].ax[2] += m2 * x2;
                        Acc[i].ay[2] += m2 * ang;
                        Acc[j].ax[2] += -m2 * x2;
                        Acc[j].ay[2] += -m2 * ang;
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

void* iteration_1(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    #define Acc Data->Level.Acc

    StopThread(1, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */


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

void* iteration_2(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(2, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */

        /**
            Signal and stop
            */

        StopThread(2, Data, thread);
    }
    printf("Closing Thread #2\n");

    return NULL;
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
