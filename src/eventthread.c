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
void handle_event_end_level(game_shared_data *);
void handle_event_score(game_shared_data *);
void handle_event_high_scores(game_shared_data *);

void request_loading(game_shared_data *);
void request_game(game_shared_data *);
void request_pause(game_shared_data *);
void request_menu(game_shared_data *);
void request_end_level(game_shared_data *);
void request_score(game_shared_data *);
void request_high_scores(game_shared_data *);

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

        if(Data->last_event.type == ALLEGRO_EVENT_TIMER){
            switch(Data->game_state){
                case gsGAME:
                    al_lock_mutex(Data->mutex_main_iteration);
                        if(Data->iteration_finished){
                            if(Data->level.at_exit){
                                al_lock_mutex(Data->mutex_change_state);
                                    Data->request_change_state = true;
                                    Data->new_state = gsENDLEVEL;
                                al_unlock_mutex(Data->mutex_change_state);
                                if(Data->level.victory){
                                    Data->level.score.level_number += 1;
                                    initialize_loading_thread(Data, load_level);
                                }
                            }else{
                                Data->iteration_finished = false;
                                al_broadcast_cond(Data->cond_main_iteration);
                            }
                        }
                    al_unlock_mutex(Data->mutex_main_iteration);
                    break;
                case gsENDLEVEL:
                    Data->loading_state += 1;
                    if(Data->loading_state >= TIMER_TICKS_FOR_END_LEVEL){
                        terminate_iteration(Data);
                        initialize_iteration_threads(Data);
                        al_lock_mutex(Data->mutex_change_state);
                            Data->request_change_state = true;
                            if(Data->level.victory){
                                Data->new_state = gsLOADING;
                            }else{
                                Data->new_state = gsSCORE;
                            }
                        al_unlock_mutex(Data->mutex_change_state);
                    }
                    if(Data->loading_state < 32){
                        synchronized_draw(draw_end_level, Data);
                        break;
                    }
                default:
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
            case gsENDLEVEL: handle_event_end_level(Data); break;
            case gsSCORE: handle_event_score(Data); break;
            case gsHIGHSCORES: handle_event_high_scores(Data); break;
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
                    case gsMENU: request_menu(Data); break;
                    case gsENDLEVEL: request_end_level(Data); break;
                    case gsSCORE: request_score(Data); break;
                    case gsHIGHSCORES: request_high_scores(Data); break;
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
    Handlers
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
        case ALLEGRO_EVENT_KEY_CHAR:
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
                        }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_shield){
                            Data->keyboard.flags[ekKEY_SHIELD] = true;
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
                }else if(Data->last_event.keyboard.keycode == Data->keyboard.key_shield){
                    Data->keyboard.flags[ekKEY_SHIELD] = false;
                }
            al_unlock_mutex(Data->keyboard.mutex_keyboard);
            break;
    }
}

void handle_event_end_level(game_shared_data *Data){
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            switch(Data->last_event.keyboard.keycode){
                case ALLEGRO_KEY_PAD_ENTER:
                case ALLEGRO_KEY_ENTER:
                case ALLEGRO_KEY_ESCAPE:
                case ALLEGRO_KEY_SPACE:
                    Data->loading_state = TIMER_TICKS_FOR_END_LEVEL;
                    break;
            }
            break;
    }
}

void handle_event_score(game_shared_data *Data){
    int key;
    char a;
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
        case ALLEGRO_EVENT_KEY_CHAR:
            key = Data->last_event.keyboard.keycode;
            switch(key){
                case ALLEGRO_KEY_PAD_ENTER:
                case ALLEGRO_KEY_ENTER:
                    if(Data->name_length > 0){
                        al_lock_mutex(Data->mutex_change_state);
                            Data->new_state = gsHIGHSCORES;
                            Data->request_change_state = true;
                        al_unlock_mutex(Data->mutex_change_state);
                    }
                    break;
                case ALLEGRO_KEY_SPACE:
                    add_char_to_name(Data->buffer, ' ', &Data->name_length, MAX_NAME_LENGTH);
                    break;
                case ALLEGRO_KEY_MINUS:
                    if(Data->last_event.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT){
                        a = '_';
                    }else{
                        a = '-';
                    }
                    add_char_to_name(Data->buffer, a, &Data->name_length, MAX_NAME_LENGTH);
                    break;
                case ALLEGRO_KEY_BACKSPACE:
                    if(Data->name_length > 0){
                        Data->name_length -= 1;
                        Data->buffer[Data->name_length] = '\0';
                    }
                    break;
                default:
                    if(key >= ALLEGRO_KEY_PAD_0 && key <= ALLEGRO_KEY_PAD_9){
                        add_char_to_name(Data->buffer, int_to_char(key - ALLEGRO_KEY_PAD_0), &Data->name_length, MAX_NAME_LENGTH);
                    }else if(key >= ALLEGRO_KEY_0 && key <= ALLEGRO_KEY_9){
                        if(Data->last_event.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT
                           && key == ALLEGRO_KEY_7){
                            a = '&';
                        }else{
                            a = int_to_char(key - ALLEGRO_KEY_0);
                        }
                        add_char_to_name(Data->buffer, a, &Data->name_length, MAX_NAME_LENGTH);
                    }else if(key >= ALLEGRO_KEY_A && key <= ALLEGRO_KEY_Z){
                        a = *al_keycode_to_name(key);
                        if(!(Data->last_event.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT)){
                            to_lower_case(&a);
                        }
                        add_char_to_name(Data->buffer, a, &Data->name_length, MAX_NAME_LENGTH);
                    }
            }
            break;
    }
}

void handle_event_high_scores(game_shared_data *Data){
    switch(Data->last_event.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->close_now = true;
            break;
        case ALLEGRO_EVENT_KEY_CHAR:
            switch(Data->last_event.keyboard.keycode){
                case ALLEGRO_KEY_PAD_ENTER:
                case ALLEGRO_KEY_ENTER:
                case ALLEGRO_KEY_ESCAPE:
                    high_scores_return(Data);
                    break;
            }
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
    new_scores(&Data->last_score);
    new_scores(&Data->level.score);
    initialize_loading_thread(Data, load_level);
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

void high_scores_activate(void *argument){
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    al_lock_mutex(Data->mutex_change_state);
        Data->request_change_state = true;
        Data->new_state = gsHIGHSCORES;
    al_unlock_mutex(Data->mutex_change_state);
    #undef Data
    #undef arg
}

void high_scores_return(game_shared_data *Data){
    al_lock_mutex(Data->mutex_change_state);
        if(Data->new_state != gsPAUSE){
            Data->new_state = gsMENU;
        }
        Data->request_change_state = true;
        Data->name_length = -1;
    al_unlock_mutex(Data->mutex_change_state);
}



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
    make_main_menu_pause_menu(&Data->menu);
    if(Data->game_state == gsHIGHSCORES){
        Data->menu.current_elem = mmeHIGHSCORES + 1;
    }
    Data->request_change_state = false;
    Data->game_state = gsPAUSE;
    Data->draw_function = draw_pause;
}

void request_loading(game_shared_data *Data){
    if(!Data->thread_loading){
        fprintf(stderr, "Failed to create loading thread.");
        Data->close_now = true;
    }
    else{
        if(Data->game_state == gsGAME){
            terminate_iteration(Data);
            initialize_iteration_threads(Data);
        }
        Data->request_change_state = false;
        Data->game_state = gsLOADING;
        Data->draw_function = draw_loading;
        al_start_thread(Data->thread_loading);
    }
}

void request_menu(game_shared_data *Data){
    make_main_menu_unpause(&Data->menu);
    Data->request_change_state = false;
    Data->game_state = gsMENU;
    Data->draw_function = draw_menu;
}

void request_end_level(game_shared_data *Data){
    /**
        Print screen
        */
    void draw_game_to_background(game_shared_data *arg){
        draw_content_to_background(draw_game, arg);
    };
    special_call(draw_game_to_background, Data);
    Data->last_score = clear_score(&Data->level.score);

    if(Data->last_score.level_number > 1){
        printf("Level finished\n"
               "Score:\t%d\n"
               "Highest level:\t%d\n", Data->last_score.total_score,
                                       Data->last_score.level_number - 1);
    }
    Data->loading_state = 0;
    Data->request_change_state = false;
    Data->game_state = gsENDLEVEL;
    Data->draw_function = draw_end_level;
}

void request_score(game_shared_data *Data){
    if(Data->last_score.level_number > 1 &&
       Data->last_score.total_score > 0){
        Data->request_change_state = false;
        Data->game_state = gsSCORE;
        Data->draw_function = draw_score;
        Data->buffer[0] = '\0';
        Data->name_length = 0;
    }else{
        //Either load error, or 0 score
        printf("Request score: redirect to menu\n");
        request_menu(Data);
    }
}

void request_high_scores(game_shared_data *Data){
    int i;
    /**
        Update score
        Data->name_length has the place number;
        if score was too low, then it has -1
        */
    if(Data->game_state == gsSCORE){
        for(i = MAX_HIGH_SCORES - 1; i >= 0; --i){
            if(Data->last_score.total_score < Data->high_scores[i].score){
                break;
            }
        }
        if(i < MAX_HIGH_SCORES - 1){
            Data->name_length = i + 1;
            for(i = MAX_HIGH_SCORES - 1; i > Data->name_length; --i){
                Data->high_scores[i].score = Data->high_scores[i - 1].score;
                strcpy(Data->high_scores[i].name, Data->high_scores[i - 1].name);
            }
            Data->high_scores[i].score = Data->last_score.total_score;
            strcpy(Data->high_scores[i].name, Data->buffer);
            set_high_scores(Data);
        }else{
            Data->name_length = -1;
        }
    }
    Data->new_state = Data->game_state; //very important line
    Data->request_change_state = false;
    Data->game_state = gsHIGHSCORES;
    Data->draw_function = draw_high_scores;
}
