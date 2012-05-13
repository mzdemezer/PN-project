#include "main.h"
#include "game.h"
#include <math.h>
#include <stdio.h>

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

        al_lock_mutex(Data->MutexIterations);
            for(i = 0; i < NumOfThreads; ++i){
                Data->IterationThreads[i].Finished = false;
            }
            al_broadcast_cond(Data->CondIterations);
        al_unlock_mutex(Data->MutexIterations);

        /**
            Main iteration thread main work
            */
        if(Data->Keyboard.Flags[ekKEY_DOWN]){
            Data->Level.Player->center.y += 2;
        }
        if(Data->Keyboard.Flags[ekKEY_UP]){
            Data->Level.Player->center.y -= 2;
        }
        if(Data->Keyboard.Flags[ekKEY_RIGHT]){
            Data->Level.Player->center.x += 2;
        }
        if(Data->Keyboard.Flags[ekKEY_LEFT]){
            Data->Level.Player->center.x -= 2;
        }
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

        al_lock_mutex(Data->MutexDrawCall);
            Data->DrawCall = true;
        al_unlock_mutex(Data->MutexDrawCall);

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

void* iteration_0(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(0, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */

        /**
            Signal and stop
            */

        StopThread(0, Data, thread);
    }
    printf("Closing Thread #0\n");

    return NULL;
    #undef Data
}

void* iteration_1(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(1, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        al_rest(0.002);

        /**
            Signal and stop
            */

        StopThread(1, Data, thread);
    }
    printf("Closing Thread #1\n");

    return NULL;
    #undef Data
}

void* iteration_2(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)

    StopThread(2, Data, thread);
    while(!al_get_thread_should_stop(thread)){
        /**
            Work
            */
        al_rest(0.005);

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
            if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyUp){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_UP] = true;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyDown){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_DOWN] = true;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyLeft){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_LEFT] = true;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyRight){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_RIGHT] = true;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }
            break;
        case ALLEGRO_EVENT_KEY_UP:
             if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyUp){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_UP] = false;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyDown){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_DOWN] = false;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyLeft){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_LEFT] = false;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }else if(Data->LastEvent.keyboard.keycode == Data->Keyboard.KeyRight){
                al_lock_mutex(Data->Keyboard.MutexKeyboard);
                    Data->Keyboard.Flags[ekKEY_RIGHT] = false;
                al_unlock_mutex(Data->Keyboard.MutexKeyboard);
            }
            break;
    }
}

void draw_game(struct GameSharedData *Data){
    int i;

    al_clear_to_color(al_map_rgb(170, 0, 0));
    if(Data->Level.Background){
        al_draw_bitmap(Data->Level.Background, 0, 0, 0);
    }

    for(i = 0; i < Data->Level.NumberOfMovableObjects; ++i){
           DRAW(Data->Level.MovableObjects[i]);
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
    int i;
    al_lock_mutex(Data->MutexChangeState);
        /**
            Game init
            */
        for(i = 0; i < NUMBER_OF_SIGNIFICANT_KEYS; ++i){
            Data->Keyboard.Flags[i] = false;
        }
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
