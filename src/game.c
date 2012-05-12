#include "main.h"
#include "game.h"
#include <math.h>
#include <stdio.h>

void terminate_i_thread(int i, struct GameSharedData *Data){printf("Terminating: #%d\n", i);
    al_lock_mutex(Data->MutexMainIteration);
        al_lock_mutex(Data->MutexIterations);
            Data->IterationThreads[i].Finished = true;
            al_broadcast_cond(Data->CondMainIteration);
        al_unlock_mutex(Data->MutexIterations);
    al_unlock_mutex(Data->MutexMainIteration);
}

void terminate_iteration(struct GameSharedData *Data){
    int i;
    al_lock_mutex(Data->MutexIterations);
        al_set_thread_should_stop(Data->ThreadMainIteration);
        for(i = 0; i < NumOfThreads; ++i){
            al_set_thread_should_stop(Data->IterationThreads[i].Thread);
        }
        for(i = 0; i < NumOfThreads; ++i){
            Data->IterationThreads[i].Finished = false;
        }
        al_broadcast_cond(Data->CondIterations);
    al_unlock_mutex(Data->MutexIterations);
}

void* main_iteration(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    int i;
    bool f;

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

    printf("In game: after main-iter-init, starting to operate\n");

    while(!al_get_thread_should_stop(thread)){

        /**
            Launching other iterations
            */
        Data->MainIterationThreadsIsWaiting = false;

        printf("In Game: locking  Iters_mutex\n");
        al_lock_mutex(Data->MutexIterations);
            for(i = 0; i < NumOfThreads; ++i){
                Data->IterationThreads[i].Finished = false;
            }
            printf("In Game: letting them goooo!\n");
            al_broadcast_cond(Data->CondIterations);
        al_unlock_mutex(Data->MutexIterations);
        printf("In Game: unlocking  Iters_mutex\n");

        /**
            Main iteration thread main work
            */
        al_rest(0.1);

        /**
            Waiting until other threads finish
            */
        printf("In Game: waiting for the lazy bastards, locking  Iters_mutex\n");
        al_lock_mutex(Data->MutexIterations);
            Data->MainIterationThreadsIsWaiting = true;
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
                }printf("In game: waiting %d\n", i);
                al_wait_cond(Data->CondMainIteration, Data->MutexIterations);
            }
        al_unlock_mutex(Data->MutexIterations);
        printf("In Game: unlocking  Iters_mutex\n");

        /**
            Main iteration thread after-work
            */
        printf("Iterated\n");

        al_lock_mutex(Data->MutexDrawCall);
            Data->DrawCall = true;
        al_unlock_mutex(Data->MutexDrawCall);

        if(!al_get_thread_should_stop(thread)){
            printf("lock mainM in game\n");
            al_lock_mutex(Data->MutexMainIteration);
                Data->IterationFinished = true;
                while(Data->IterationFinished){
                    printf("In Game: waiting for Timer\n");
                    al_wait_cond(Data->CondMainIteration, Data->MutexMainIteration);
                }
            al_unlock_mutex(Data->MutexMainIteration);
            printf("unlock mainM in game\n");
        }
    }
    printf("Closing Thread: main-iteration\n");

    return NULL;
    #undef Data
}

/**
    This function signals that the  Thread[i] finished its work
    and then sets it to wait
    */

void StopThread(int i, struct GameSharedData *Data){
    printf("In thread #%d: Stopping\n", i);
    al_lock_mutex(Data->MutexIterations);
        Data->IterationThreads[i].Finished = true;
        if(Data->MainIterationThreadsIsWaiting){
            al_broadcast_cond(Data->CondMainIteration);printf("In thread #%d: Sending signal  ------->\n", i);
        }else{printf("In thread #%d: No signal needed  X\n", i);}
        while(Data->IterationThreads[i].Finished){
            al_wait_cond(Data->CondIterations, Data->MutexIterations);
        }
    al_unlock_mutex(Data->MutexIterations);
    printf("In thread #%d: Going again! :)\n", i);
}

void* iteration_0(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(0, Data);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        printf("Thread #0 iterated\n");

        /**
            Signal and stop
            */

        StopThread(0, Data);
    }
    terminate_i_thread(0, Data);
    printf("Closing Thread #0\n");

    return NULL;
    #undef Data
}

void* iteration_1(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(1, Data);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        al_rest(0.02);
        printf("Thread #1 iterated\n");

        /**
            Signal and stop
            */

        StopThread(1, Data);
    }
    terminate_i_thread(1, Data);
    printf("Closing Thread #1\n");

    return NULL;
    #undef Data
}

void* iteration_2(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(2, Data);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        al_rest(0.05);
        printf("Thread #2 iterated\n");

        /**
            Signal and stop
            */

        StopThread(2, Data);
    }
    terminate_i_thread(2, Data);
    printf("Closing Thread #2\n");

    return NULL;
    #undef Data
}

void handle_event_game(struct GameSharedData *Data){
    //printf("event handled in game\n");
    switch(Data->LastEvent.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->CloseLevel = true;
            Data->CloseNow = true;
            break;
        default:
            if(Data->LastEvent.type == Data->Keyboard.KeyUp){

            }else if(Data->LastEvent.type == Data->Keyboard.KeyDown){

            }else if(Data->LastEvent.type == Data->Keyboard.KeyLeft){

            }else if(Data->LastEvent.type == Data->Keyboard.KeyRight){

            }
    }
}

void draw_game(struct GameSharedData *Data){
    al_clear_to_color(al_map_rgb(170, 0, 0));
    if(Data->Level.Background){
        al_draw_bitmap(Data->Level.Background, 0, 0, 0);
    }

    al_draw_text(Data->MenuBigFont,
                 al_map_rgb(255,255,255),
                 Data->DisplayData.width / 2,
                 Data->DisplayData.height / 2,
                 ALLEGRO_ALIGN_CENTRE,
                 "GAME");

    al_flip_display();
}

void request_game(struct GameSharedData *Data){
    al_lock_mutex(Data->MutexChangeState);
        Data->RequestChangeState = false;
        Data->GameState = gsGAME;
    al_unlock_mutex(Data->MutexChangeState);
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
