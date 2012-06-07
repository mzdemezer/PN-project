#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "gameshareddata.h"
#include "../mathematics.h"
#include "../stringprocessing.h"
#include "../menu.h"
#include "../draw.h"

/**
    display
    */

void calculate_scales(game_shared_data *Data){
    Data->scales.scale = double_min(Data->display_data.width / (double)SCREEN_BUFFER_WIDTH, Data->display_data.height / (double)SCREEN_BUFFER_HEIGHT);

    Data->scales.scale_w = SCREEN_BUFFER_WIDTH * Data->scales.scale;
    Data->scales.scale_h = SCREEN_BUFFER_HEIGHT * Data->scales.scale;
    Data->scales.scale_x = (Data->display_data.width - Data->scales.scale_w) / 2;
    Data->scales.scale_y = (Data->display_data.height - Data->scales.scale_h) / 2;
    Data->scales.trans_x = Data->scales.scale_x / Data->scales.scale;
    Data->scales.trans_y = Data->scales.scale_y / Data->scales.scale;
}

void calculate_transformation(game_shared_data *Data){
    al_identity_transform(&Data->transformation);
    al_scale_transform(&Data->transformation, Data->scales.scale, Data->scales.scale);
    al_use_transform(&Data->transformation);
}

void destroy_fonts(game_shared_data *Data){
    al_destroy_font(Data->font_menu_big);
    al_destroy_font(Data->font_menu_regular);
    al_destroy_font(Data->font_menu_config);
    al_destroy_font(Data->font_menu_config_selected);
    al_destroy_font(Data->font_time);
    al_destroy_font(Data->font_debug);
}
void scale_fonts(game_shared_data *Data){
    const char *buffer;
    destroy_fonts(Data);

    buffer = al_get_config_value(Data->config, "Fonts", "menu_font");
    Data->font_menu_big = al_load_ttf_font(buffer, (int)(Data->scales.scale_h / 10), 0);
    if (!Data->font_menu_big){
        fprintf(stderr, "Could not load '%s'.\n", buffer);
        Data->font_menu_big = al_load_ttf_font(DEFAULT_FONT_MENU, (int)(Data->scales.scale_h / 10), 0);
        if (!Data->font_menu_big){
            fprintf(stderr, "Could not load '%s'.\n", DEFAULT_FONT_MENU);
            exit(-1);
        }
        buffer = DEFAULT_FONT_MENU;
    }
    al_set_config_value(Data->config, "Fonts", "menu_font", buffer);
    Data->font_menu_regular = al_load_ttf_font(buffer, (int)(Data->font_menu_big->height * 0.62), 0);
    Data->font_menu_selected = al_load_ttf_font(buffer, (int)((Data->font_menu_big->height +  Data->font_menu_regular->height) / 2), 0);
    Data->font_menu_config = al_load_ttf_font(buffer, (int)(Data->font_menu_regular->height * 0.45), 0);
    Data->font_menu_config_selected = al_load_ttf_font(buffer, (int)(Data->font_menu_selected->height * 0.45), 0);

    buffer = al_get_config_value(Data->config, "Fonts", "time_font");
    Data->font_time = al_load_ttf_font(buffer, (int)(Data->scales.scale_h / 20), 0);
    if (!Data->font_time){
        fprintf(stderr, "Could not load '%s'.\n", buffer);
        Data->font_time = al_load_ttf_font(DEFAULT_FONT_TIME, (int)(Data->scales.scale_h / 20), 0);
        if (!Data->font_time){
            fprintf(stderr, "Could not load '%s'.\n", DEFAULT_FONT_TIME);
            exit(-1);
        }
        buffer = DEFAULT_FONT_TIME;
    }
    al_set_config_value(Data->config, "Fonts", "time_font", buffer);

    buffer = al_get_config_value(Data->config, "Fonts", "debug_font");
    Data->font_debug = al_load_ttf_font(buffer, (int)(Data->scales.scale_h / 36), 0);
    if (!Data->font_debug){
        fprintf(stderr, "Could not load '%s'.\n", buffer);
        Data->font_debug = al_load_ttf_font(DEFAULT_FONT_DEBUG, (int)(Data->scales.scale_h / 36), 0);
        if (!Data->font_debug){
            fprintf(stderr, "Could not load '%s'.\n", DEFAULT_FONT_DEBUG);
            exit(-1);
        }
        buffer = DEFAULT_FONT_DEBUG;
    }
    al_set_config_value(Data->config, "Fonts", "debug_font", buffer);
}

/**
    This procedure does not initialize threads
    */
bool construct_game_shared_data(game_shared_data *Data, int max_fps){
    char buf[20];
    /**
        Getting resolution
        */
    Data->max_resolution_index = al_get_num_display_modes() - 1;
    Data->config = al_load_config_file(CONFIG_FILE_NAME);
    if(!Data->config){
        Data->config = al_create_config();
        al_set_config_value(Data->config, "Graphic", "fullscreen", "1");
        int_to_str(Data->max_resolution_index, buf);
        al_set_config_value(Data->config, "Graphic", "resolution", buf);
        al_set_config_value(Data->config, "Fonts", "menu_font", DEFAULT_FONT_MENU);
        al_set_config_value(Data->config, "Fonts", "time_font", DEFAULT_FONT_TIME);
        al_set_config_value(Data->config, "Fonts", "debug_font", DEFAULT_FONT_DEBUG);
    }

    /**
        Creating display
        */
    Data->fullscreen = atoi(al_get_config_value(Data->config, "Graphic", "fullscreen"));
    Data->chosen_resolution = atoi(al_get_config_value(Data->config, "Graphic", "resolution"));
    al_get_display_mode(Data->chosen_resolution, &Data->display_data);
    Data->in_menu_display_data = Data->display_data;
    Data->reso_chosen_in_menu = Data->chosen_resolution;
    calculate_scales(Data);

    if(Data->fullscreen){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    }else{
        al_set_new_display_flags(ALLEGRO_WINDOWED);
        al_set_new_window_position(8, 30);
    }
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    Data->display = NULL;
    Data->display = al_create_display(Data->display_data.width, Data->display_data.height);
    if(!Data->display){
        fprintf(stderr, "Problems when creating the display");
        return false;
    }
    if(al_get_display_option(Data->display, ALLEGRO_SAMPLE_BUFFERS)){
        printf("With multisampling, level %i\n", al_get_display_option(Data->display, ALLEGRO_SAMPLES));
    }
    else{
        printf("Without multisampling.\n");
    }
    calculate_transformation(Data);
    /**
        Setting font size accordingly to resolution
        */
    Data->font_menu_big = NULL;
    Data->font_menu_regular = NULL;
    Data->font_menu_selected = NULL;
    Data->font_menu_config = NULL;
    Data->font_menu_config_selected = NULL;
    Data->font_time = NULL;
    Data->font_debug = NULL;
    scale_fonts(Data);

    /**
        Initializing event_queue
        */
    Data->event_queue = NULL;
    Data->event_queue = al_create_event_queue();
    if(!Data->event_queue) {
        fprintf(stderr, "failed to create main_event_queue!\n");
        al_destroy_display(Data->display);
        return false;
    }
    Data->timer_draw = NULL;
    Data->timer_draw = al_create_timer(1.0 / max_fps);
    if(!Data->timer_draw) {
        fprintf(stderr, "failed to create timer!\n");
        al_destroy_display(Data->display);
        al_destroy_event_queue(Data->event_queue);
        return false;
    }
    al_register_event_source(Data->event_queue, al_get_timer_event_source(Data->timer_draw));
    al_register_event_source(Data->event_queue, al_get_keyboard_event_source());
    al_register_event_source(Data->event_queue, al_get_display_event_source(Data->display));

    /**
        menu initialization
        */
    Data->menu.current_menu = create_menu();
    Data->menu.current_elem = 1;

    /**
        Initializing Data
        */

    Data->debug_buffer[0] = '\0';

    construct_level(&Data->level);

    Data->keyboard.key_up    = ALLEGRO_KEY_UP;
    Data->keyboard.key_down  = ALLEGRO_KEY_DOWN;
    Data->keyboard.key_left  = ALLEGRO_KEY_LEFT;
    Data->keyboard.key_right = ALLEGRO_KEY_RIGHT;

    /**
        Mutexes
        */
    Data->mutex_change_state       = al_create_mutex();
    Data->mutex_draw               = al_create_mutex();
    Data->mutex_main_iteration     = al_create_mutex();
    Data->mutex_iteration          = al_create_mutex();
    Data->mutex_draw_call          = al_create_mutex();
    Data->mutex_thread_draw        = al_create_mutex();
    Data->keyboard.mutex_keyboard  = al_create_mutex();
    Data->mutex_special_main_call  = al_create_mutex();
    Data->mutex_FPS                = al_create_mutex();

    /**
        Conditions
        */
    Data->cond_iterations             = al_create_cond();
    Data->cond_main_iteration         = al_create_cond();
    Data->cond_draw_call              = al_create_cond();
    Data->cond_special_main_call      = al_create_cond();
    Data->cond_synchro_with_main_iter = al_create_cond();

    /**
        Flags
        */
    Data->game_state = gsMENU;
    Data->debug = false;
    Data->draw_call = false;
    Data->close_now = false;
    Data->iteration_finished = true;
    Data->request_change_state = false;
    Data->special_main_call = false;
    Data->synchro_with_main_iter = false;

    /**
        Others
        */
    Data->draw_function = draw_menu;
    Data->FPS = 0;

    return true;
}

void destroy_game_shared_data(game_shared_data *Data){
    al_save_config_file(CONFIG_FILE_NAME, Data->config);
    al_destroy_config(Data->config);
    al_destroy_display(Data->display);
    al_destroy_timer(Data->timer_draw);
    al_destroy_event_queue(Data->event_queue);
    al_destroy_mutex(Data->mutex_change_state);
    al_destroy_mutex(Data->mutex_draw);
    al_destroy_mutex(Data->mutex_main_iteration);
    al_destroy_mutex(Data->mutex_iteration);
    al_destroy_mutex(Data->mutex_draw_call);
    al_destroy_mutex(Data->mutex_thread_draw);
    al_destroy_mutex(Data->keyboard.mutex_keyboard);
    al_destroy_mutex(Data->mutex_special_main_call);
    al_destroy_mutex(Data->mutex_FPS);
    al_destroy_cond(Data->cond_iterations);
    al_destroy_cond(Data->cond_main_iteration);
    al_destroy_cond(Data->cond_draw_call);
    al_destroy_cond(Data->cond_special_main_call);
    al_destroy_cond(Data->cond_synchro_with_main_iter);
    destroy_fonts(Data);
    destroy_menu(&Data->menu);
    destroy_level(&Data->level);
}
