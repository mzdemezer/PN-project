#include <stdio.h>
#include "main.h"
#include "eventthread.h"
#include "game.h"
#include "loading.h"
#include "menu.h"

/**
    Private methods
    */

void handle_event_menu(game_shared_data*);
void handle_event_game(game_shared_data *);
void handle_event_loading(game_shared_data *);

void request_loading(game_shared_data *);
void request_game(game_shared_data *);
void request_pause(game_shared_data *);

extern void change_menu(game_shared_data *, menu_elem *, int);
extern void return_menu(game_shared_data *);
/**
    Code
    */
void* thread_event_queue_procedure(ALLEGRO_THREAD *thread, void *arg){
    int timer_ticks = 0;

    #define Data ((game_shared_data*)arg)
    /**
        Main loop
        */
    while(1){
    al_wait_for_event(Data->event_queue, &Data->last_event);
        //printf("event type#%d\n", Data->last_event.type);

        if(Data->last_event.type == ALLEGRO_EVENT_TIMER){
            if(Data->game_state == gsGAME){
                al_lock_mutex(Data->mutex_main_iteration);
                    if(Data->iteration_finished){
                        Data->iteration_finished = false;
                        al_broadcast_cond(Data->cond_main_iteration);
                    }
                al_unlock_mutex(Data->mutex_main_iteration);
            }else{
                al_lock_mutex(Data->mutex_draw_call);
                    Data->draw_call = true;
                al_unlock_mutex(Data->mutex_draw_call);
            }

            timer_ticks += 1;
            if(timer_ticks == MAX_FPS){
                al_lock_mutex(Data->mutex_FPS);
                    printf("\n\nSecond passed, FPS: %d\n\n", Data->FPS);
                    timer_ticks = 0;
                    Data->FPS = 0;
                al_unlock_mutex(Data->mutex_FPS);
            }
        }
        switch(Data->game_state){
            case gsGAME: handle_event_game(Data); break;
            case gsPAUSE:
            case gsMENU: handle_event_menu(Data); break;
            case gsLOADING: handle_event_loading(Data); break;
        }

        if(Data->close_now){
            printf("Closing\n");
            break;
        }

        al_lock_mutex(Data->mutex_change_state);
            if(Data->request_change_state){
                switch(Data->new_state){
                    case gsPAUSE: request_pause(Data); break;
                    case gsLOADING: request_loading(Data); break;
                    case gsGAME: request_game(Data); break;
                    case gsMENU: break;
                }
            }
        al_unlock_mutex(Data->mutex_change_state);

        al_lock_mutex(Data->mutex_draw_call);
            if(Data->draw_call){
                Data->draw_call = false;
                al_lock_mutex(Data->mutex_thread_draw);
                    if(Data->thread_draw_waiting){
                        Data->thread_draw_waiting = false;
                        al_broadcast_cond(Data->cond_draw_call);
                    }
                al_unlock_mutex(Data->mutex_thread_draw);
            }
        al_unlock_mutex(Data->mutex_draw_call);
    }
    /**
        Cleaning-up threads
        */

    terminate_iteration(Data);

    /**
        Sending shut-down signal to the main thread
        */
    al_lock_mutex(Data->mutex_thread_draw);
        Data->thread_draw_waiting = false;
        al_broadcast_cond(Data->cond_draw_call);
        printf("Event-queue thread: signal sent\n");
    al_unlock_mutex(Data->mutex_thread_draw);
    printf("Event queue thread closed\n");
    return NULL;
    #undef Data
}

/**
    menu
    */
void handle_event_menu(game_shared_data *Data){
    activation_argument arg;
    arg.Data = Data;
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            switch(Data->last_event.keyboard.keycode){
                case ALLEGRO_KEY_LEFT:
                    if(Data->menu.current_menu[Data->menu.current_elem].type == metUPDOWN){
                        arg.call_type = meatDOWN;
                        (Data->menu.current_menu[Data->menu.current_elem].activate)((void*)&arg);
                    }
                    break;
                case ALLEGRO_KEY_RIGHT:
                    if(Data->menu.current_menu[Data->menu.current_elem].type == metUPDOWN){
                        arg.call_type = meatUP;
                        (Data->menu.current_menu[Data->menu.current_elem].activate)((void*)&arg);
                    }
                    break;
                case ALLEGRO_KEY_UP:
                    Data->menu.current_elem -= 1;
                    normalize_menu_selection(&(Data->menu));
                    break;
                case ALLEGRO_KEY_DOWN:
                    Data->menu.current_elem += 1;
                    normalize_menu_selection(&(Data->menu));
                    break;
                case ALLEGRO_KEY_PAD_ENTER:
                case ALLEGRO_KEY_ENTER:
                    arg.call_type = meatACCEPT;
                    switch (Data->menu.current_menu[Data->menu.current_elem].type){
                        case metSUBMENU:
                            change_menu(Data, (menu_elem*) Data->menu.current_menu[Data->menu.current_elem].activate, 1);
                            break;
                        case metACTIVATE:
                            (Data->menu.current_menu[Data->menu.current_elem].activate)((void*)&arg);
                            break;
                        case metUPDOWN:
                            (Data->menu.current_menu[Data->menu.current_elem].activate)((void*)&arg);
                            break;
                    }
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    al_lock_mutex(Data->mutex_change_state);
                        void *temp = (void*)Data->menu.current_menu;
                        return_menu(Data);
                        if(Data->game_state == gsPAUSE){
                            if(temp == (void*)Data->menu.current_menu){
                                return_to_game_activate((void*)&arg);
                            }
                        }
                    al_unlock_mutex(Data->mutex_change_state);
                    break;
            }
            break;
    }
}

void handle_event_loading(game_shared_data *Data){
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
    }
}

void handle_event_game(game_shared_data *Data){
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            switch(Data->last_event.keyboard.keycode){
                case ALLEGRO_KEY_ESCAPE:
                    al_lock_mutex(Data->mutex_main_iteration);
                        al_lock_mutex(Data->mutex_change_state);
                            Data->new_state = gsPAUSE;
                            Data->request_change_state = true;
                        al_unlock_mutex(Data->mutex_change_state);
                        if(!Data->iteration_finished){
                                Data->synchro_with_main_iter = true;
                                while(Data->synchro_with_main_iter){
                                    al_wait_cond(Data->cond_synchro_with_main_iter,
                                                 Data->mutex_main_iteration);
                                }
                        }
                    al_unlock_mutex(Data->mutex_main_iteration);
                    Data->level.sum_time += al_get_time() - Data->level.start_time;
                    printf("Synchronized with main iteration that is now waiting\n");
                    break;
                default:
                    al_lock_mutex(Data->keyboard.mutex_keyboard);
                    if(Data->last_event.keyboard.keycode == Data->keyboard.key_up){
                        Data->keyboard.flags[ekKEY_UP] = true;
                    }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_down){
                        Data->keyboard.flags[ekKEY_DOWN] = true;
                    }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_left){
                        Data->keyboard.flags[ekKEY_LEFT] = true;
                    }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_right){
                        Data->keyboard.flags[ekKEY_RIGHT] = true;
                    }else if(Data->last_event.keyboard.keycode == ALLEGRO_KEY_TILDE){
                        Data->debug = !Data->debug;
                    }
                    al_unlock_mutex(Data->keyboard.mutex_keyboard);
            }
            break;
        case ALLEGRO_EVENT_KEY_UP:
            al_lock_mutex(Data->keyboard.mutex_keyboard);
            if(Data->last_event.keyboard.keycode == Data->keyboard.key_up){
                Data->keyboard.flags[ekKEY_UP] = false;
            }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_down){
                Data->keyboard.flags[ekKEY_DOWN] = false;
            }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_left){
                Data->keyboard.flags[ekKEY_LEFT] = false;
            }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_right){
                Data->keyboard.flags[ekKEY_RIGHT] = false;
            }
            al_unlock_mutex(Data->keyboard.mutex_keyboard);
            break;
    }
}

void new_game_activate(void *argument){
    printf("NEW GAME ACTIVATED\n");
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    //necessary settings
    al_lock_mutex(Data->mutex_change_state);
        if(Data->game_state == gsPAUSE){
            terminate_iteration(Data);
            initialize_iteration_threads(Data);
        }
        Data->request_change_state = true;
        Data->new_state = gsLOADING;
    al_unlock_mutex(Data->mutex_change_state);
    Data->menu.current_elem = 1;
    Data->level.levelNumber = 1;
    Data->thread_loading = NULL;
    Data->thread_loading = al_create_thread(&load_level, (void*)Data);
    #undef Data
    #undef arg
};

void return_to_game_activate(void *argument){
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    al_lock_mutex(Data->mutex_change_state);
        Data->request_change_state = true;
        Data->new_state = gsGAME;
    al_unlock_mutex(Data->mutex_change_state);
    #undef arg
}

void exit_activate(void *argument){
    printf("EXIT ACTIVATED\n");
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    Data->close_now = true;
    #undef Data
    #undef arg
};

/**
    Requests
    */
void request_game(game_shared_data *Data){
    int i;

    switch(Data->game_state){
        case gsLOADING:
            /**
                Game init
                */
            for(i = 0; i < NUMBER_OF_SIGNIFICANT_KEYS; ++i){
                Data->keyboard.flags[i] = false;
            }
            Data->request_change_state = false;
            Data->game_state = gsGAME;
            Data->draw_function = draw_game;

            al_destroy_thread(Data->thread_loading);
            al_start_thread(Data->thread_main_iteration);
            break;
        case gsPAUSE:
            Data->request_change_state = false;
            Data->game_state = gsGAME;
            Data->draw_function = draw_game;
            Data->level.last_time = al_get_time() - 1./MAX_FPS;
            Data->level.start_time = al_get_time();
            printf("Unpause!\n\n");
            break;
        default:
            break;
    }
}

void request_pause(game_shared_data *Data){
    Data->request_change_state = false;
    Data->game_state = gsPAUSE;
    Data->draw_function = draw_pause;
    make_main_menu_pause_menu(&Data->menu);
}

void request_loading(game_shared_data *Data){
    if(!Data->thread_loading){
        fprintf(stderr, "Failed to create loading thread.");
        Data->close_now = true;
    }
    else{
        Data->request_change_state = false;
        Data->game_state = gsLOADING;
        Data->draw_function = draw_loading;
        al_start_thread(Data->thread_loading);
    }
}
