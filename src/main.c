#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

#include "main.h"
#include "game.h"

/**
    Call for  function_to_call  to be executed
    in main thread and wait for the results
    */

void special_call(void (*function_to_call)(game_shared_data*), game_shared_data *Data){
    al_lock_mutex(Data->mutex_special_main_call);
        al_lock_mutex(Data->mutex_thread_draw);
            printf("Special call made\n");
            Data->special_main_call_procedure = function_to_call;
            Data->special_main_call = true;

            if(Data->thread_draw_waiting){
                Data->thread_draw_waiting = false;
                al_broadcast_cond(Data->cond_draw_call);
            }
        al_unlock_mutex(Data->mutex_thread_draw);

        while(Data->special_main_call){
            al_wait_cond(Data->cond_special_main_call, Data->mutex_special_main_call);
        }
    al_unlock_mutex(Data->mutex_special_main_call);
}

void initialize_loading_thread(game_shared_data *Data, void* (*load_func)(ALLEGRO_THREAD *, void *)){
    Data->thread_loading = NULL;
    Data->thread_loading = al_create_thread(load_func, (void*)Data);
}

/**
    Loading thread is intialized separetly,
    at demand
    */
bool initialize_all_threads(game_shared_data *Data){
    Data->thread_event_queue = NULL;
    Data->thread_event_queue = al_create_thread(thread_event_queue_procedure, (void*)Data);
    if(!Data->thread_event_queue){
        fprintf(stderr, "Failed to initialize thread_event_queue");
        al_destroy_display(Data->display);
        al_destroy_event_queue(Data->event_queue);
        al_destroy_timer(Data->timer_draw);
        return false;
    }
    if(!initialize_iteration_threads(Data)){
        al_destroy_display(Data->display);
        al_destroy_event_queue(Data->event_queue);
        al_destroy_timer(Data->timer_draw);
        return false;
    }
    return true;
}

void start_game(game_shared_data *Data){
    printf("Starting The Game\n\n\n");
    al_start_thread(Data->thread_event_queue);
    al_start_timer(Data->timer_draw);
}

#ifdef TESTS
#include "Tests/CuTest.h"
extern void RunAllTests(void);

int main(){
    printf("Running in test mode\n\n");
    RunAllTests();
    return 0;
}

#else
int main(){
    if(!al_init()){
        fprintf(stderr, "Problems when initilizing Allegro");
        return -1;
    }

    /**
        Initializing addons
        */

    al_init_font_addon();
    al_init_ttf_addon();

    if(!al_init_image_addon()){
        fprintf(stderr, "Failed to initialize al_init_image_addon!");
        return -1;
    }

    if(!al_init_primitives_addon()){
        fprintf(stderr, "Failed to initialize al_init_primitives_addon!");
        return -1;
    }
    game_shared_data Data;
    /**
        Initializing keyboard
        */
    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        printf("Nie możesz grać w tą grę bez klawiatury gościu...");
        return -1;
    }


    if(!construct_game_shared_data(&Data, MAX_FPS)){
        fprintf(stderr, "Problems during constucting shared Data\n");
        return -1;
    }


    /**
        Initializing mouse
        */
    if(!al_install_mouse()){
        fprintf(stderr, "failed to initialize the mouse!\n");
    }
    else{
        al_hide_mouse_cursor(Data.display);
    }

    if(!initialize_all_threads(&Data)){
        fprintf(stderr, "Failed to initialize threads\n");
        return -1;
    }
    /**
        First draw
        */
    setlocale(LC_NUMERIC, "C");
    draw(draw_menu, &Data);

    /**
        Draw thread
        */
    start_game(&Data);
    while(!Data.close_now){
        al_lock_mutex(Data.mutex_special_main_call);
        al_lock_mutex(Data.mutex_thread_draw);
            if(Data.special_main_call){
                al_unlock_mutex(Data.mutex_thread_draw);
                /**
                    Receiving special call to do (generally draw)
                    something in the main thread. During this
                    operation the calling thread is synchronized
                    with the main thread.
                    */
                    Data.special_main_call_procedure(&Data);
                    Data.special_main_call = false;
                    al_broadcast_cond(Data.cond_special_main_call);
                al_unlock_mutex(Data.mutex_special_main_call);
            }
            else{
                al_unlock_mutex(Data.mutex_special_main_call);
                /**
                    Normal boring drawing routines
                    */

                    Data.thread_draw_waiting = true;
                    while(Data.thread_draw_waiting){
                        al_wait_cond(Data.cond_draw_call, Data.mutex_thread_draw);
                    }
                al_unlock_mutex(Data.mutex_thread_draw);

                al_lock_mutex(Data.mutex_draw);
                    draw(Data.draw_function, &Data);
                al_unlock_mutex(Data.mutex_draw);
                al_lock_mutex(Data.mutex_FPS);
                    Data.FPS += 1;
                al_unlock_mutex(Data.mutex_FPS);
            }
    }
    /**
        End draw thread
        */
    /**
        Clean-up
        */
    printf("Main thread at clean-up, waiting for EventQueue thread\n");
    al_destroy_thread(Data.thread_event_queue);

    printf("Main thread at last clean-up, all threads safely closed\n");
    destroy_game_shared_data(&Data);

    return 0;
}

#endif
