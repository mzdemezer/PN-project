#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "main.h"
#include "menu.h"
#include "loading.h"
#include "mathematics.h"
#include "stringprocessing.h"

/**
    Private methods
    */
void menu_elem_init(menu_elem *item, menu_elem_type new_type,
                    char *new_name, void *new_activate);
void deallocate_submenus(menu_elem *submenu_descriptor);

void change_menu(game_shared_data *, menu_elem *, int);
void return_menu(game_shared_data *);
void return_to_main_menu(menu_structure *);
void restore_current_settings(game_shared_data *);

void normalize_resolution_selection(int *current, const int max);
void resolution_activate(void *);
void fullscreen_activate(void *);
void rescale_bitmaps(game_shared_data *);
void change_resolution(game_shared_data *);
void change_controls(void *argument);
void change_screen_mode(game_shared_data *Data);

/**
    Code
    */
void menu_elem_init(menu_elem *item,
                    menu_elem_type new_type,
                    char *new_name,
                    void *new_activate){
    item->type = new_type;
    item->name = new_name;
    item->activate = new_activate;
}

menu_elem *create_menu(){
    menu_elem *main_menu =        (menu_elem*)malloc(sizeof(menu_elem) * (2 + int_abs(MAIN_MENU_SIZE))),
                     *options_menu =     (menu_elem*)malloc(sizeof(menu_elem) * (1 + int_abs(OPTIONS_MENU_SIZE))),
                     *graphic_menu =     (menu_elem*)malloc(sizeof(menu_elem) * (1 + int_abs(GRAPHIC_MENU_SIZE))),
                     *sound_menu =       (menu_elem*)malloc(sizeof(menu_elem) * (1 + int_abs(SOUND_MENU_SIZE))),
                     *controls_menu =    (menu_elem*)malloc(sizeof(menu_elem) * (1 + int_abs(CONTROLS_MENU_SIZE)));

    menu_elem_init(&main_menu[mmeDESCRIPTOR], MAIN_MENU_SIZE,
                   "MAIN MENU", main_menu);
    menu_elem_init(&main_menu[mmeNEWGAME], metACTIVATE,
                   "NEW GAME", new_game_activate);
    menu_elem_init(&main_menu[mmeOPTIONS], metSUBMENU,
                   "OPTIONS", options_menu);
    menu_elem_init(&main_menu[mmeHIGHSCORES], metACTIVATE,
                   "HIGH SCORES", high_scores_activate);
    menu_elem_init(&main_menu[mmeEXIT], metACTIVATE,
                   "EXIT", exit_activate);
    menu_elem_init(&main_menu[mmeRETURN], metACTIVATE,
                   "RETURN", return_to_game_activate);

    menu_elem_init(&options_menu[omeDESCRIPTOR], OPTIONS_MENU_SIZE,
                   "OPTIONS", main_menu);
    menu_elem_init(&options_menu[omeGRAPHIC], metSUBMENU,
                   "GRAPHIC", graphic_menu);
    menu_elem_init(&options_menu[omeSOUND], metSUBMENU,
                   "SOUND", sound_menu);
    menu_elem_init(&options_menu[omeCONTROLS], metSUBMENU,
                   "CONTROLS", controls_menu);
    menu_elem_init(&options_menu[omeRETURN], metSUBMENU,
                   "RETURN", main_menu);

    menu_elem_init(&graphic_menu[gmeDESCRIPTOR], GRAPHIC_MENU_SIZE,
                   "GRAPHIC", options_menu);
    menu_elem_init(&graphic_menu[gmeRESOLUTION], metUPDOWN,
                   "RESOLUTION", resolution_activate);
    menu_elem_init(&graphic_menu[gmeFULLSCREEN], metUPDOWN,
                   "FULLSCREEN", fullscreen_activate);
    menu_elem_init(&graphic_menu[gmeRETURN], metSUBMENU,
                   "RETURN", options_menu);

    menu_elem_init(&sound_menu[smeDESCRIPTOR], SOUND_MENU_SIZE,
                   "SOUND", options_menu);
    menu_elem_init(&sound_menu[smeRETURN], metSUBMENU,
                   "RETURN", options_menu);

    menu_elem_init(&controls_menu[cmeDESCRIPTOR], CONTROLS_MENU_SIZE,
                   "CONTROLS", options_menu);
    menu_elem_init(&controls_menu[cmeUP], metINPUT,
                   "ACCELERATE", change_controls);
    menu_elem_init(&controls_menu[cmeDOWN], metINPUT,
                   "DECELERATE", change_controls);
    menu_elem_init(&controls_menu[cmeLEFT], metINPUT,
                   "TURN LEFT", change_controls);
    menu_elem_init(&controls_menu[cmeRIGHT], metINPUT,
                   "TURN RIGHT", change_controls);
    menu_elem_init(&controls_menu[cmeSHIELD], metINPUT,
                   "SHIELD", change_controls);
    menu_elem_init(&controls_menu[cmeNEG], metINPUT,
                   "NEGATIVE CHARGE", change_controls);
    menu_elem_init(&controls_menu[cmePOS], metINPUT,
                   "POSITIVE CHARGE", change_controls);
    menu_elem_init(&controls_menu[cmeGRAV], metINPUT,
                   "GRAVITY", change_controls);
    menu_elem_init(&controls_menu[cmeRETURN], metSUBMENU,
                   "RETURN", options_menu);

    return main_menu;
}

void deallocate_submenus(menu_elem *submenu_descriptor){
    int i;
    for(i = 0; i < int_abs(submenu_descriptor->type); ++i){
        if(submenu_descriptor[i].type == metSUBMENU &&
           submenu_descriptor[i].activate != submenu_descriptor->activate){
            deallocate_submenus((menu_elem*)submenu_descriptor[i].activate);
        }
    }
    free(submenu_descriptor);
}

void destroy_menu(menu_structure *menu){
    return_to_main_menu(menu);
    deallocate_submenus(menu->current_menu);
}

void normalize_menu_selection(menu_structure *menu){
    int num_of_elems = int_abs(menu->current_menu->type);
    if(menu->current_elem > num_of_elems){
        menu->current_elem = 1;
    }
    else if(menu->current_elem <= 0){
        menu->current_elem = num_of_elems;
    }
}

void change_menu(game_shared_data *Data, menu_elem *new_menu, int new_current){
    restore_current_settings(Data);
    Data->menu.current_menu = new_menu;
    Data->menu.current_elem = new_current;
    printf("MENU CHANGED\n");
}

void return_menu(game_shared_data *Data){
    change_menu(Data, (menu_elem*)Data->menu.current_menu[0].activate, 1);
    normalize_menu_selection(&(Data->menu));
}

/**
        Return to main menu
        Descriptor - menu[0] -
        always has pointer to parent menu
        in its activate field.
        Main menu has itself as its parent menu,
        so everything should be fine
        The i variable is only a security
        */
void return_to_main_menu(menu_structure *menu){
    short int i;
    for(i = DEEPEST_MENU_LEVEL; (menu->current_menu[0].activate != (void*)menu->current_menu) && (i > 0); --i){
        menu->current_menu = (menu_elem*)menu->current_menu[0].activate;
    }
}

void restore_current_settings(game_shared_data *Data){
    int i;
    activation_argument arg;
    arg.call_type = meatRESTORE_CURRENT;
    arg.Data = Data;
    if((int)Data->menu.current_menu[0].type < 0){
        for(i = 1; i <= int_abs((int)Data->menu.current_menu[0].type); ++i){
            if(Data->menu.current_menu[i].type == metUPDOWN){
                Data->menu.current_menu[i].activate((void*)&arg);
            }
        }
    }
}

void make_main_menu_pause_menu(menu_structure *menu){
    short int i;
    menu_elem temp;

    return_to_main_menu(menu);

    if((short int)menu->current_menu->type != MAIN_MENU_SIZE + 1){
        /**
            Put  RETURN  at first position in   main menu
            */
        temp = menu->current_menu[mmeRETURN];
        for(i = (short int)menu->current_menu->type; i > 0; --i){
            menu->current_menu[i + 1] = menu->current_menu[i];
        }
        menu->current_menu[1] = temp;

        /**
            Change size
            */
        menu->current_menu->type = MAIN_MENU_SIZE + 1;
    }
}

void make_main_menu_unpause(menu_structure *menu){
    short int i;
    menu_elem temp;

    return_to_main_menu(menu);

    if((short int)menu->current_menu->type != MAIN_MENU_SIZE){
        /**
            RETURN goes at the end
            */
        temp = menu->current_menu[1];
        for(i = 2; i <= (short int)menu->current_menu->type; ++i){
            menu->current_menu[i - 1] = menu->current_menu[i];
        }
        menu->current_menu[mmeRETURN] = temp;

        /**
            Change size
            */
        menu->current_menu->type = MAIN_MENU_SIZE;
    }
}


void normalize_resolution_selection(int *current, const int max){
    if(*current < 0){
        *current = 0;
    }
    else if(*current > max){
        *current = max;
    }
};

void rescale_bitmaps(game_shared_data *Data){
    if(Data->level.background){
        draw_level_background(Data);
    }
    if(Data->ship_loaded){
        scale_ship(Data);
    }
}

void change_resolution(game_shared_data *Data){
    char buf[20];
    Data->chosen_resolution = Data->reso_chosen_in_menu;
    Data->display_data = Data->in_menu_display_data;
    int_to_str(Data->chosen_resolution, buf);
    al_set_config_value(Data->config, "Graphic", "resolution", buf);
    calculate_scales(Data);
    al_unregister_event_source(Data->event_queue, al_get_display_event_source(Data->display));
    if(!al_resize_display(Data->display, Data->display_data.width, Data->display_data.height)){
        fprintf(stderr, "Error occured while trying to resize!\n");
        Data->close_now = true;
    }else{
        al_register_event_source(Data->event_queue, al_get_display_event_source(Data->display));
        calculate_transformation(Data);
        scale_fonts(Data);
        rescale_bitmaps(Data);
        printf("Resolution changed: %d x %d\n", Data->display_data.width, Data->display_data.height);
    }
}

void resolution_activate(void* argument){
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    ALLEGRO_FONT *font;
    ALLEGRO_COLOR color;
    char current_resolution[20];

    switch(arg->call_type){
        case meatACCEPT:
            if(Data->chosen_resolution != Data->reso_chosen_in_menu){
                call_in_main_thread(change_resolution, Data);
            }
            break;
        case meatUP:
            Data->reso_chosen_in_menu += 1;
            normalize_resolution_selection(&Data->reso_chosen_in_menu, Data->max_resolution_index);
            al_get_display_mode(Data->reso_chosen_in_menu, &Data->in_menu_display_data);
            break;
        case meatDOWN:
            Data->reso_chosen_in_menu -= 1;
            normalize_resolution_selection(&Data->reso_chosen_in_menu, Data->max_resolution_index);
            al_get_display_mode(Data->reso_chosen_in_menu, &Data->in_menu_display_data);
            break;
        case meatRESTORE_CURRENT:
            Data->reso_chosen_in_menu = Data->chosen_resolution;
            Data->in_menu_display_data = Data->display_data;
            break;
        default:
            /**
                Draw
                */
            if(arg->call_type - (int) meatDRAW == Data->menu.current_elem){
                font = Data->font_menu_config_selected;
                color = al_map_rgb(255, 255, 0);
            }
            else{
                font = Data->font_menu_config;
                color = al_map_rgb(255, 255, 255);
            }
            stringify_resolution(&Data->in_menu_display_data, current_resolution);
            al_draw_text(font, color, Data->scales.scale_w * 0.9 + Data->scales.scale_x,
                         (arg->call_type - meatDRAW) * Data->font_menu_regular->height + Data->font_menu_big->height * 1.665 + Data->scales.scale_y,
                         ALLEGRO_ALIGN_RIGHT, current_resolution);
    }
    #undef Data
    #undef arg
}

void change_controls(void *argument){
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    int elem;
    ALLEGRO_FONT *font;
    ALLEGRO_COLOR color;
    const char *buffer;
    char buf[20], buf2[20];
    switch(arg->call_type){
        case meatACCEPT:
            Data->ask_for_input = true;
            break;
        case meatDOWN:
            Data->ask_for_input = false;
            elem = Data->menu.current_elem - 1;
            Data->keyboard.keys[elem] = Data->input;

            int_to_str(elem, buf);
            int_to_str(Data->input, buf2);
            al_set_config_value(Data->config, "Controls", buf, buf2);
            break;
        default:
            elem = arg->call_type - (int)meatDRAW;
            if(elem > 0){
                if(elem == Data->menu.current_elem){
                    font = Data->font_menu_config_selected;
                    color = al_map_rgb(255, 255, 0);
                    if(Data->ask_for_input){
                        buffer = "...";
                    }else{
                        buffer = al_keycode_to_name(Data->keyboard.keys[elem - 1]);
                    }
                }
                else{
                    font = Data->font_menu_config;
                    color = al_map_rgb(255, 255, 255);
                    buffer = al_keycode_to_name(Data->keyboard.keys[elem - 1]);
                }
            al_draw_text(font, color,
                         Data->scales.scale_w * 0.9 + Data->scales.scale_x,
                         elem * Data->font_menu_regular->height + Data->font_menu_big->height * 1.665 + Data->scales.scale_y,
                         ALLEGRO_ALIGN_RIGHT, buffer);
            }
    }
    #undef Data
    #undef arg
}

void change_screen_mode(game_shared_data *Data){
    char buf[20];
    int_to_str(Data->fullscreen, buf);
    al_set_config_value(Data->config, "Graphic", "fullscreen", buf);
    al_unregister_event_source(Data->event_queue, al_get_display_event_source(Data->display));

    al_destroy_display(Data->display);
    if(Data->fullscreen){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    }else{
        al_set_new_display_flags(ALLEGRO_WINDOWED);
        al_set_new_window_position(8, 30);
    }
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    Data->display = NULL;
    Data->display = al_create_display(Data->display_data.width, Data->display_data.height);
    if(!Data->display){
        fprintf(stderr, "Problems when creating the display\n");
        Data->close_now = true;
    }else{
        al_register_event_source(Data->event_queue, al_get_display_event_source(Data->display));
        scale_fonts(Data);
        rescale_bitmaps(Data);
        if(Data->mouse_working){
            al_hide_mouse_cursor(Data->display);
        }
    }
}

void fullscreen_activate(void *argument){
    #define arg ((activation_argument*)argument)
    #define Data arg->Data
    int elem;
    ALLEGRO_FONT *font;
    ALLEGRO_COLOR color;
    const char *buf;
    switch(arg->call_type){
        case meatACCEPT:
            if(Data->fullscreen != Data->fullscreen_in_menu){
                Data->fullscreen = Data->fullscreen_in_menu;
                call_in_main_thread(change_screen_mode, Data);
            }
            break;
        case meatUP:
        case meatDOWN:
            Data->fullscreen_in_menu = !Data->fullscreen_in_menu;
            break;
        case meatRESTORE_CURRENT:
            Data->fullscreen_in_menu = Data->fullscreen;
            break;
        default:
            /**
                Draw
                */
            elem = arg->call_type - (int)meatDRAW;
            if(elem == Data->menu.current_elem){
                font = Data->font_menu_config_selected;
                color = al_map_rgb(255, 255, 0);
            }
            else{
                font = Data->font_menu_config;
                color = al_map_rgb(255, 255, 255);
            }
            if(Data->fullscreen_in_menu){
                buf = "ON";
            }else{
                buf = "OFF";
            }
            al_draw_text(font, color,
                         Data->scales.scale_w * 0.9 + Data->scales.scale_x,
                         elem * Data->font_menu_regular->height + Data->font_menu_big->height * 1.665 + Data->scales.scale_y,
                         ALLEGRO_ALIGN_RIGHT, buf);
    }
    #undef Data
    #undef arg
}
