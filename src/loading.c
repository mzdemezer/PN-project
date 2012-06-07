#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include "main.h"
#include "loading.h"

/**
    Private methods
    */
void read_line(char *, ALLEGRO_FILE *);
void read_color(char *, ALLEGRO_FILE *, ALLEGRO_COLOR *, int, const char *, int);

/**
    Code
    */
void* load_level(ALLEGRO_THREAD *thread, void *argument){
    printf("Loading... level\n");
    #define Data ((game_shared_data*)argument)

    Data->loading_state = 0;
    synchronized_draw(draw_loading, Data);

    // Actual loading
    load_and_initialize_level(Data);

    Data->loading_state = 14;

    printf("Loading finished\n");
    al_lock_mutex(Data->mutex_change_state);
        Data->new_state = gsGAME;
        Data->request_change_state = true;
    al_unlock_mutex(Data->mutex_change_state);

    return NULL;
    #undef Data
};

void load_and_initialize_level(game_shared_data *Data){
    add_borders(&Data->level);
    load_level_from_file(Data);
    special_call(draw_level_background, (void*)Data);

    Data->loading_state = 10;

    Data->level.acc = (move_arrays*)malloc(sizeof(move_arrays) * Data->level.number_of_movable_objects);
    Data->level.dens = DEFAULT_FLUID_DENSITY;
    Data->level.wind_vx = 0;
    Data->level.wind_vy = 0;
    Data->level.start_time = al_get_time();
    Data->level.sum_time = 0;
}

/**
    Load background, scale it, draw static objects
    */
void draw_level_background(game_shared_data *Data){
    ALLEGRO_BITMAP *tempBitmap;
    ALLEGRO_TRANSFORM tempT;

    /**
        Preparing bitmaps and loading the file
        This is the place to destroy bitmaps - swap them
        quickly for new ones when DrawThread is
        busy... helping to do this :D
        */
    al_destroy_bitmap(Data->level.background);
    Data->level.background = al_create_bitmap(Data->scales.scale_h, Data->scales.scale_h);
    if(strcmp(Data->level.filename + 12, "0") == 0){
        tempBitmap = NULL;
    }else{
        tempBitmap = al_load_bitmap(Data->level.filename);
    }

    /**
        Scaling and drawing
        */
    al_lock_mutex(Data->mutex_draw);
            Data->loading_state = 11;
            draw(draw_loading, Data);
        al_identity_transform(&tempT);
        al_use_transform(&tempT);
            al_set_target_bitmap(Data->level.background);
            if(tempBitmap){
                scale_bitmap(tempBitmap, Data->scales.scale_h, Data->scales.scale_h);
                al_destroy_bitmap(tempBitmap);
            }else{
                al_clear_to_color(DEFAULT_BACKGROUND_COLOR);
            }

        al_use_transform(&Data->transformation);
            al_set_target_backbuffer(Data->display);
            Data->loading_state = 12;
            draw(draw_loading, Data);
        al_use_transform(&tempT);

            tempBitmap = al_get_backbuffer(Data->display);
            al_set_target_bitmap(tempBitmap);
            al_draw_bitmap(Data->level.background, 0, 0, 0);

        al_use_transform(&Data->transformation);
            draw_all_fixed_objects(&Data->level);


            al_set_target_bitmap(Data->level.background);
        al_use_transform(&tempT);
            al_draw_bitmap_region(tempBitmap, 0, 0, Data->scales.scale_h, Data->scales.scale_h, 0, 0, 0);

            al_set_target_bitmap(tempBitmap);
        al_use_transform(&Data->transformation);
            Data->loading_state = 13;
            draw(draw_loading, Data);
    al_unlock_mutex(Data->mutex_draw);
}

void load_level_from_file(game_shared_data *Data){
    ALLEGRO_FILE *level;
    char buffer[1024];
    int n, i, op0, op1;
    object_workshop Factory;

    strcpy(buffer, "Data/levels/level");
    int_to_str(Data->level.levelNumber, buffer + 17);
    strcat(buffer, ".lev");

    level = al_fopen(buffer, "r");

    if (level == NULL) {
        fprintf(stderr, "Failed to open %s\n", buffer);
        Data->close_now = true;
        return;
    }

    /**
        Loading level background
        */

    read_line(buffer, level);

    strcpy(Data->level.filename, "Data/levels/");
    strcpy(Data->level.filename + 12, buffer);

    /**
        Loading Objects
        */
    /**
        player(s)
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);

    for(i = 0; i < n; ++i){
        Data->level.player = (movable_player*)malloc(sizeof(movable_player));
        add_movable_object(&Data->level, motPLAYER, (void*)Data->level.player);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf", &Data->level.player->center.x,
                                   &Data->level.player->center.y,
                                   &Data->level.player->ang);

        op0 = Data->level.number_of_movable_objects - 1;
        construct_movable(&Data->level.movable_objects[op0]);
        initialize_zones_with_movable(&Data->level, Data->level.movable_objects[op0].zones, op0);
    }
    Data->loading_state = 1;

    /**
        Rectangles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_rectangle = (fixed_rectangle*)malloc(sizeof(fixed_rectangle));
        add_fixed_object(&Data->level, fotRECTANGLE, (void*)Factory.new_rectangle);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf %d", &Factory.new_rectangle->center.x,
                                            &Factory.new_rectangle->center.y,
                                            &Factory.new_rectangle->a,
                                            &Factory.new_rectangle->b,
                                            &Factory.new_rectangle->ang,
                                            &op0);

        read_color(buffer, level, &Factory.new_rectangle->color, op0, "Invalid level input: rectangle#%d color\n", i);

        construct_rectangle(&Data->level.fixed_objects[Data->level.number_of_fixed_objects - 1]);
        add_rectangle(&Data->level, Factory.new_rectangle);
    }
    Data->loading_state = 2;

    /**
        Circles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_circle = (fixed_circle*)malloc(sizeof(fixed_circle));
        add_fixed_object(&Data->level, fotCIRCLE, (void*)Factory.new_circle);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %d",   &Factory.new_circle->center.x,
                                        &Factory.new_circle->center.y,
                                        &Factory.new_circle->r,
                                        &op0);
        read_color(buffer, level, &Factory.new_circle->color, op0, "Invalid level input: circle#%d color\n", i);

        construct_circle(&Data->level.fixed_objects[Data->level.number_of_fixed_objects - 1]);
        add_circle(&Data->level, Factory.new_circle->r, Factory.new_circle->center);
    }

    Data->loading_state = 3;
    /**
        Squares
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_square = (fixed_square*)malloc(sizeof(fixed_square));
        add_fixed_object(&Data->level, fotSQUARE, (void*)Factory.new_square);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %d",    &Factory.new_square->center.x,
                                            &Factory.new_square->center.y,
                                            &Factory.new_square->bok,
                                            &Factory.new_square->ang,
                                            &op0);
        read_color(buffer, level, &Factory.new_square->color, op0, "Invalid level input: square#%d color\n", i);

        construct_square(&Data->level.fixed_objects[Data->level.number_of_fixed_objects - 1]);
        add_square(&Data->level, Factory.new_square);
    }

    Data->loading_state = 4;
    /**
        Entrances
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_entrance = (fixed_entrance*)malloc(sizeof(fixed_entrance));
        add_fixed_object(&Data->level, fotENTRANCE, (void*)Factory.new_entrance);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf %d", &Factory.new_entrance->center.x,
                                            &Factory.new_entrance->center.y,
                                            &Factory.new_entrance->a,
                                            &Factory.new_entrance->b,
                                            &Factory.new_entrance->ang,
                                            &op0);
        read_color(buffer, level, &Factory.new_entrance->color, op0, "Invalid level input: entrance#%d color\n", i);

        construct_rectangle(&Data->level.fixed_objects[Data->level.number_of_fixed_objects - 1]);
    }

    Data->loading_state = 5;
    /**
        Exits
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_exit = (fixed_exit*)malloc(sizeof(fixed_exit));
        add_fixed_object(&Data->level, fotEXIT, (void*)Factory.new_exit);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf %d", &Factory.new_exit->center.x,
                                            &Factory.new_exit->center.y,
                                            &Factory.new_exit->a,
                                            &Factory.new_exit->b,
                                            &Factory.new_exit->ang,
                                            &op0);
        read_color(buffer, level, &Factory.new_exit->color, op0, "Invalid level input: rectangle#%d color\n", i);

        construct_rectangle(&Data->level.fixed_objects[Data->level.number_of_fixed_objects - 1]);
    }

    Data->loading_state = 6;
    /**
        Switches
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_switch = (movable_switch*)malloc(sizeof(movable_switch));
        add_movable_object(&Data->level, motSWITCH, (void*)Factory.new_switch);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf %d %d %d",   &Factory.new_switch->center.x,
                                                    &Factory.new_switch->center.y,
                                                    &Factory.new_switch->a,
                                                    &Factory.new_switch->b,
                                                    &Factory.new_switch->ang,
                                                    &op1,
                                                    &Factory.new_switch->pos,
                                                    &Factory.new_switch->connected.number_of_doors);
        Factory.new_switch->type = (switch_type) op1;
        /**
            Reading connected doors
            */

        if(Factory.new_switch->connected.number_of_doors != 0){
            read_line(buffer, level);
            Factory.new_switch->connected.doors = (int*)malloc(sizeof(int) * Factory.new_switch->connected.number_of_doors);
            sscanf(buffer, "%d", &Factory.new_switch->connected.doors[0]);
            for(op1 = 1; op1 < Factory.new_switch->connected.number_of_doors; ++op1){
                sscanf(buffer, " %d", &Factory.new_switch->connected.doors[op1]);
            }
        }

        /**
            Reading connected switches
            */
        read_line(buffer, level);
        sscanf(buffer, "%d", &Factory.new_switch->connected.number_of_switches);
        if(Factory.new_switch->connected.number_of_switches != 0){
            read_line(buffer, level);
            Factory.new_switch->connected.switches = (int*)malloc(sizeof(int) * Factory.new_switch->connected.number_of_switches);
            sscanf(buffer, "%d", &Factory.new_switch->connected.switches[0]);
            for(op1 = 1; op1 < Factory.new_switch->connected.number_of_switches; ++op1){
                sscanf(buffer, " %d", &Factory.new_switch->connected.switches[op1]);
            }
        }

        /**
            Reading mass and color
            */
        read_line(buffer, level);
        sscanf(buffer, "%lf %d", &Factory.new_switch->mass,
                                &op0);
        read_color(buffer, level, &Factory.new_switch->color, op0, "Invalid level input: switch#%d color\n", i);

        op0 = Data->level.number_of_movable_objects - 1;
        construct_movable(&Data->level.movable_objects[op0]);
        initialize_zones_with_movable(&Data->level, Data->level.movable_objects[op0].zones, op0);
    }

    Data->loading_state = 7;
    /**
        Doors
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_door = (movable_door*)malloc(sizeof(movable_door));
        add_movable_object(&Data->level, motDOOR, (void*)Factory.new_door);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf %d %d %lf %d",    &Factory.new_door->center.x,
                                                        &Factory.new_door->center.y,
                                                        &Factory.new_door->a,
                                                        &Factory.new_door->b,
                                                        &Factory.new_door->ang,
                                                        &op1,
                                                        &Factory.new_door->pos,
                                                        &Factory.new_door->opening_time,
                                                        &op0);
        Factory.new_door->type = (switch_type) op1;
        read_color(buffer, level, &Factory.new_door->color, op0, "Invalid level input: door#%d color\n", i);

        op0 = Data->level.number_of_movable_objects - 1;
        construct_movable(&Data->level.movable_objects[op0]);
        initialize_zones_with_movable(&Data->level, Data->level.movable_objects[op0].zones, op0);
    }

    Data->loading_state = 8;
    /**
        Particles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.new_particle = (movable_particle*)malloc(sizeof(movable_particle));
        add_movable_object(&Data->level, motPARTICLE, (void*)Factory.new_particle);

        read_line(buffer, level);
        sscanf(buffer, "%lf %lf %lf %lf %lf",    &Factory.new_particle->center.x,
                                            &Factory.new_particle->center.y,
                                            &Factory.new_particle->r,
                                            &Factory.new_particle->mass,
                                            &Factory.new_particle->charge);

        op0 = Data->level.number_of_movable_objects - 1;
        construct_movable(&Data->level.movable_objects[op0]);
        initialize_zones_with_movable(&Data->level, Data->level.movable_objects[op0].zones, op0);
    }

    al_fclose(level);
    Data->loading_state = 9;
}

/**
    Almost the same thing as al_fgets
    */
void read_line(char *buffer, ALLEGRO_FILE *file){
    short int i = -1;
    do{
        ++i;
        buffer[i] = (char)al_fgetc(file);
    }while((buffer[i] != '\n') && (buffer[i] != '\0') && !(al_feof(file)));
    buffer[i] = '\0';
}

void read_color(char *buffer, ALLEGRO_FILE *level, ALLEGRO_COLOR *color, int colorType, const char *error_message, int index){
    int op1, op2, op3, op4;
    read_line(buffer, level);

    switch(colorType){
            case 3:
                sscanf(buffer, "%d %d %d", &op1, &op2, &op3);
                *color = al_map_rgb(op1, op2, op3);
                break;
            case 4:
                sscanf(buffer, "%d %d %d %d", &op1, &op2, &op3, &op4);
                *color = al_map_rgba(op1, op2, op3, op4);
                break;
            default:
                fprintf(stderr, error_message, index);
                *color = ERROR_COLOR;
    }
}
