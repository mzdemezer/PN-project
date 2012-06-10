#ifndef _INCLUDE_GAMESHAREDDATA_H
#define _INCLUDE_GAMESHAREDDATA_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "level.h"
#include "../menu.h"

#define NUMBER_OF_SIGNIFICANT_KEYS 8

/**
    Config
    */
#define CONFIG_FILE_NAME "config.ini"
#define MAX_HIGH_SCORES 10
#define MAX_NAME_LENGTH 25

#define DEFAULT_FONT_MENU "pirulen.ttf"
#define DEFAULT_FONT_DEBUG "DejaVuSans.ttf"
#define DEFAULT_FONT_TIME "Oloron.ttf"

typedef enum game_state{
    gsMENU,
    gsLOADING,
    gsPAUSE,
    gsGAME,
    gsENDLEVEL,
    gsSCORE,
    gsHIGHSCORES
}game_state;

typedef enum enum_keys{
    ekKEY_UP = 0,
    ekKEY_DOWN,
    ekKEY_LEFT,
    ekKEY_RIGHT,
    ekKEY_SHIELD,
    ekKEY_NEG,
    ekKEY_POS,
    ekKEY_GRAV
}enum_keys;

typedef struct keyboard_structure{
    int key_up,
        key_down,
        key_left,
        key_right,
        key_shield,
        key_neg,
        key_pos,
        key_grav;
    bool flags[NUMBER_OF_SIGNIFICANT_KEYS];
    ALLEGRO_MUTEX *mutex_keyboard;
}keyboard_structure;


typedef struct scale_structure{
    double scale_x, scale_y,
          scale_w, scale_h,
          trans_x, trans_y,
          scale;
}scale_structure;

typedef struct high_score{
    int score;
    char name[MAX_NAME_LENGTH + 1];
}high_score;
/**
    Auxilary threads
    */

typedef struct auxilary_thread_structure{
    ALLEGRO_THREAD *thread;
    bool finished;
    void* (*job)(ALLEGRO_THREAD *, void*);
}auxilary_thread;

typedef struct game_shared_data{
    ALLEGRO_CONFIG *config;
    bool fullscreen;
    game_state game_state;
    bool request_change_state;
    game_state new_state;
    ALLEGRO_MUTEX *mutex_change_state;

    ALLEGRO_MUTEX *mutex_FPS;
    unsigned char FPS;
    ALLEGRO_MUTEX *mutex_draw;
    ALLEGRO_TIMER *timer_draw;
    void (*draw_function)(struct game_shared_data *);

    int loading_state;
    ALLEGRO_THREAD *thread_loading;
    ALLEGRO_THREAD *thread_event_queue;
    ALLEGRO_THREAD *thread_main_iteration;
    ALLEGRO_COND *cond_main_iteration;
    ALLEGRO_MUTEX *mutex_main_iteration;
    ALLEGRO_MUTEX *mutex_iteration;
    ALLEGRO_COND *cond_iterations;
    auxilary_thread iteration_threads[NumOfThreads];
    bool iteration_finished;
    bool main_iter_thread_waiting;

    scale_structure scales;
    ALLEGRO_TRANSFORM transformation;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_DISPLAY_MODE display_data;
    ALLEGRO_DISPLAY_MODE in_menu_display_data;
    int max_resolution_index;
    int chosen_resolution;
    int reso_chosen_in_menu;

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT last_event;

    menu_structure menu;
    ALLEGRO_FONT *font_menu_regular;
    ALLEGRO_FONT *font_menu_big;
    ALLEGRO_FONT *font_menu_selected;
    ALLEGRO_FONT *font_menu_config;
    ALLEGRO_FONT *font_menu_config_selected;

    ALLEGRO_FONT *font_time;

    keyboard_structure keyboard;

    level_data level;
    score_struct last_score;
    /**
        Flags
        */
    ALLEGRO_MUTEX *mutex_draw_call;
    ALLEGRO_COND *cond_draw_call;
    ALLEGRO_MUTEX *mutex_thread_draw;
    bool thread_draw_waiting;
    bool draw_call;

    ALLEGRO_MUTEX *mutex_special_main_call;
    ALLEGRO_COND *cond_special_main_call;
    bool special_main_call;
    void (*special_main_call_procedure)(struct game_shared_data*);

    ALLEGRO_COND *cond_synchro_with_main_iter;
    bool synchro_with_main_iter;

    int name_length;
    char buffer[256];
    high_score high_scores[MAX_HIGH_SCORES];

    bool debug;
    ALLEGRO_FONT *font_debug;

    bool close_now;
}game_shared_data;

/**
    menu elements that habe game_shared Data and need to be declared here
    */
typedef enum menu_elem_activation_type{
    meatACCEPT,
    meatUP,
    meatDOWN,
    meatRESTORE_CURRENT,
    meatDRAW
}menu_elem_activation_type;

typedef struct activation_argument{
    menu_elem_activation_type call_type;
    game_shared_data *Data;
}activation_argument;

bool construct_game_shared_data(game_shared_data *, int max_fps);
void destroy_game_shared_data(game_shared_data *);
void calculate_scales(game_shared_data *);
void calculate_transformation(game_shared_data *);
void scale_fonts(game_shared_data*);
void get_high_scores(game_shared_data *);
void set_high_scores(game_shared_data *);

#endif
