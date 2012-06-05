#include "main.h"
#include "loading.h"
#include "game.h"
#include <allegro5/allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void first_loading_screen(struct GameSharedData *Data){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    draw_loading(Data);
    al_flip_display();
}

void* load_level(ALLEGRO_THREAD *thread, void *argument){
    printf("Loading... level\n");
    #define Data ((struct GameSharedData*)argument)

    Data->loading_state = 0;
    special_call(first_loading_screen, Data);

    // Actual loading
    load_and_initialize_level(Data);

    Data->loading_state = 15;

    printf("Loading finished\n");
    al_lock_mutex(Data->MutexChangeState);
        Data->NewState = gsGAME;
        Data->RequestChangeState = true;
    al_unlock_mutex(Data->MutexChangeState);

    return NULL;
    #undef Data
};

/**
    Load background, scale it, draw static objects

    Draws two versions for scaled and unscaled background.
    If user changes resolution during the game,
    then background ----scale---> scaledbackground
    guarantees always quite good quality.
    If however, display is smaller than default
    background size when loading the level, then only
    one scaled version is drawn and the other one
    is drawn without antialiasing.
    So after changing to bigger resolution there will be a bug,
    for no anti-aliasing will be visible in the background.

    Don't mess with the resolution too much! -.=
    */

void draw_level_background(struct GameSharedData *Data){
    ALLEGRO_BITMAP *tempBitmap;
    ALLEGRO_TRANSFORM tempT;

    /**
        Preparing bitmaps and loading the file
        This is the place to destroy bitmaps - swap them
        quickly for new ones when DrawThread is
        busy helping... doing this :D
        */
    al_destroy_bitmap(Data->Level.Background);
    Data->Level.Background = al_create_bitmap(SCREEN_BUFFER_HEIGHT, SCREEN_BUFFER_HEIGHT);
    al_destroy_bitmap(Data->Level.ScaledBackground);
    Data->Level.ScaledBackground = al_create_bitmap(Data->scales.scale_h, Data->scales.scale_h);
    if(strcmp(Data->Level.filename + 12, "0") == 0){
        tempBitmap = NULL;
    }else{
        tempBitmap = al_load_bitmap(Data->Level.filename);
    }

    /**
        Scaling and drawing
        */
    al_lock_mutex(Data->DrawMutex);
        Data->loading_state = 11;
        draw(draw_loading, Data);
        al_identity_transform(&tempT);
        al_use_transform(&tempT);

        al_set_target_bitmap(Data->Level.Background);
        if(tempBitmap){
            scale_bitmap(tempBitmap, SCREEN_BUFFER_HEIGHT, SCREEN_BUFFER_HEIGHT);

            al_use_transform(&Data->Transformation);
            al_set_target_backbuffer(Data->Display);
            Data->loading_state = 12;
            draw(draw_loading, Data);
            al_use_transform(&tempT);

            al_set_target_bitmap(Data->Level.ScaledBackground);
            scale_bitmap(tempBitmap, Data->scales.scale_h, Data->scales.scale_h);
            al_destroy_bitmap(tempBitmap);
        }else{
            al_clear_to_color(DEFAULT_BACKGROUND_COLOR);

            al_use_transform(&Data->Transformation);
            al_set_target_backbuffer(Data->Display);
            Data->loading_state = 12;
            draw(draw_loading, Data);
            al_use_transform(&tempT);

            al_set_target_bitmap(Data->Level.ScaledBackground);
            al_clear_to_color(DEFAULT_BACKGROUND_COLOR);
        }


        tempBitmap = al_get_backbuffer(Data->Display);

        al_set_target_bitmap(tempBitmap);
        al_use_transform(&Data->Transformation);
        Data->loading_state = 13;
        draw(draw_loading, Data);

        al_draw_bitmap(Data->Level.ScaledBackground, 0, 0, 0);

        draw_all_fixed_objects(Data);
        al_use_transform(&tempT);

        al_set_target_bitmap(Data->Level.ScaledBackground);
        al_draw_bitmap_region(tempBitmap, 0, 0, Data->scales.scale_h, Data->scales.scale_h, 0, 0, 0);


        if(SCREEN_BUFFER_HEIGHT <= Data->scales.scale_h){
            al_set_target_bitmap(tempBitmap);
            al_draw_bitmap(Data->Level.Background, 0, 0, 0);

            draw_all_fixed_objects(Data);

            al_set_target_bitmap(Data->Level.Background);
            al_draw_bitmap_region(tempBitmap, 0, 0, SCREEN_BUFFER_HEIGHT, SCREEN_BUFFER_HEIGHT, 0, 0, 0);
        }else{
            al_set_target_bitmap(Data->Level.Background);
            draw_all_fixed_objects(Data);
        }

        al_set_target_bitmap(tempBitmap);
        al_use_transform(&Data->Transformation);
        Data->loading_state = 14;
        draw(draw_loading, Data);
    al_unlock_mutex(Data->DrawMutex);
}

void load_level_from_file(struct GameSharedData *Data){
    ALLEGRO_FILE *level;
    char buffer[1024];
    int n, i, op0, op1;
    struct ObjectWorkshop Factory;

    strcpy(buffer, "Data/Levels/level");
    int_to_str(Data->Level.LevelNumber, buffer + 17);
    strcat(buffer, ".lev");

    level = al_fopen(buffer, "r");

    if (level == NULL) {
        fprintf(stderr, "Failed to open %s\n", buffer);
        Data->CloseNow = true;
        return;
    }

    /**
        Loading level Background
        */

    read_line(buffer, level);

    strcpy(Data->Level.filename, "Data/Levels/");
    strcpy(Data->Level.filename + 12, buffer);

    /**
        Loading Objects
        */
    /**
        Player(s)
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);

    for(i = 0; i < n; ++i){
        Data->Level.Player = (struct playerData*)malloc(sizeof(struct playerData));
        add_movable_object(&Data->Level, motPLAYER, (void*)Data->Level.Player);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f", &Data->Level.Player->center.x,
                                   &Data->Level.Player->center.y,
                                   &Data->Level.Player->ang);

        construct_movable(&Data->Level.MovableObjects[Data->Level.number_of_movable_objects - 1]);
    }
    Data->loading_state = 1;

    /**
        Rectangles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewRectangle = (struct rectangleData*)malloc(sizeof(struct rectangleData));
        add_fixed_object(&Data->Level, fotRECTANGLE, (void*)Factory.NewRectangle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewRectangle->center.x,
                                            &Factory.NewRectangle->center.y,
                                            &Factory.NewRectangle->a,
                                            &Factory.NewRectangle->b,
                                            &Factory.NewRectangle->ang,
                                            &op0);

        read_color(buffer, level, &Factory.NewRectangle->color, op0, "Invalid level input: rectangle#%d color\n", i);

        construct_rectangle(&Data->Level.FixedObjects[Data->Level.number_of_fixed_objects - 1]);
        add_rectangle(Data, Factory.NewRectangle);
    }
    Data->loading_state = 2;

    /**
        Circles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewCircle = (struct circleData*)malloc(sizeof(struct circleData));
        add_fixed_object(&Data->Level, fotCIRCLE, (void*)Factory.NewCircle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %d",   &Factory.NewCircle->center.x,
                                        &Factory.NewCircle->center.y,
                                        &Factory.NewCircle->r,
                                        &op0);
        read_color(buffer, level, &Factory.NewCircle->color, op0, "Invalid level input: circle#%d color\n", i);

        construct_circle(&Data->Level.FixedObjects[Data->Level.number_of_fixed_objects - 1]);
        add_circle(Data, Factory.NewCircle->r, Factory.NewCircle->center);
    }

    Data->loading_state = 3;
    /**
        Squares
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewSquare = (struct squareData*)malloc(sizeof(struct squareData));
        add_fixed_object(&Data->Level, fotSQUARE, (void*)Factory.NewSquare);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %d",    &Factory.NewSquare->center.x,
                                            &Factory.NewSquare->center.y,
                                            &Factory.NewSquare->bok,
                                            &Factory.NewSquare->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewSquare->color, op0, "Invalid level input: square#%d color\n", i);

        construct_square(&Data->Level.FixedObjects[Data->Level.number_of_fixed_objects - 1]);
        add_square(Data, Factory.NewSquare);
    }

    Data->loading_state = 4;
    /**
        Entrances
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewEntrance = (struct entranceData*)malloc(sizeof(struct entranceData));
        add_fixed_object(&Data->Level, fotENTRANCE, (void*)Factory.NewEntrance);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewEntrance->center.x,
                                            &Factory.NewEntrance->center.y,
                                            &Factory.NewEntrance->a,
                                            &Factory.NewEntrance->b,
                                            &Factory.NewEntrance->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewEntrance->color, op0, "Invalid level input: entrance#%d color\n", i);

        construct_rectangle(&Data->Level.FixedObjects[Data->Level.number_of_fixed_objects - 1]);
    }

    Data->loading_state = 5;
    /**
        Exits
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewExit = (struct exitData*)malloc(sizeof(struct exitData));
        add_fixed_object(&Data->Level, fotEXIT, (void*)Factory.NewExit);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewExit->center.x,
                                            &Factory.NewExit->center.y,
                                            &Factory.NewExit->a,
                                            &Factory.NewExit->b,
                                            &Factory.NewExit->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewExit->color, op0, "Invalid level input: rectangle#%d color\n", i);

        construct_rectangle(&Data->Level.FixedObjects[Data->Level.number_of_fixed_objects - 1]);
    }

    Data->loading_state = 6;
    /**
        Switches
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewSwitch = (struct switchData*)malloc(sizeof(struct switchData));
        add_movable_object(&Data->Level, motSWITCH, (void*)Factory.NewSwitch);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d %d %d",   &Factory.NewSwitch->center.x,
                                                    &Factory.NewSwitch->center.y,
                                                    &Factory.NewSwitch->a,
                                                    &Factory.NewSwitch->b,
                                                    &Factory.NewSwitch->ang,
                                                    &op1,
                                                    &Factory.NewSwitch->pos,
                                                    &Factory.NewSwitch->connected.number_of_doors);
        Factory.NewSwitch->swType = (enum switch_type) op1;
        /**
            Reading connected doors
            */

        if(Factory.NewSwitch->connected.number_of_doors != 0){
            read_line(buffer, level);
            Factory.NewSwitch->connected.doors = (int*)malloc(sizeof(int) * Factory.NewSwitch->connected.number_of_doors);
            sscanf(buffer, "%d", &Factory.NewSwitch->connected.doors[0]);
            for(op1 = 1; op1 < Factory.NewSwitch->connected.number_of_doors; ++op1){
                sscanf(buffer, " %d", &Factory.NewSwitch->connected.doors[op1]);
            }
        }

        /**
            Reading connected switches
            */
        read_line(buffer, level);
        sscanf(buffer, "%d", &Factory.NewSwitch->connected.number_of_switches);
        if(Factory.NewSwitch->connected.number_of_switches != 0){
            read_line(buffer, level);
            Factory.NewSwitch->connected.switches = (int*)malloc(sizeof(int) * Factory.NewSwitch->connected.number_of_switches);
            sscanf(buffer, "%d", &Factory.NewSwitch->connected.switches[0]);
            for(op1 = 1; op1 < Factory.NewSwitch->connected.number_of_switches; ++op1){
                sscanf(buffer, " %d", &Factory.NewSwitch->connected.switches[op1]);
            }
        }

        /**
            Reading mass and color
            */
        read_line(buffer, level);
        sscanf(buffer, "%f %d", &Factory.NewSwitch->mass,
                                &op0);
        read_color(buffer, level, &Factory.NewSwitch->color, op0, "Invalid level input: switch#%d color\n", i);

        construct_movable(&Data->Level.MovableObjects[Data->Level.number_of_movable_objects - 1]);
    }

    Data->loading_state = 7;
    /**
        Doors
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewDoor = (struct doorData*)malloc(sizeof(struct doorData));
        add_movable_object(&Data->Level, motDOOR, (void*)Factory.NewDoor);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d %d %f %d",    &Factory.NewDoor->center.x,
                                                        &Factory.NewDoor->center.y,
                                                        &Factory.NewDoor->a,
                                                        &Factory.NewDoor->b,
                                                        &Factory.NewDoor->ang,
                                                        &op1,
                                                        &Factory.NewDoor->pos,
                                                        &Factory.NewDoor->openingTime,
                                                        &op0);
        Factory.NewDoor->doorType = (enum switch_type) op1;
        read_color(buffer, level, &Factory.NewDoor->color, op0, "Invalid level input: door#%d color\n", i);

        construct_movable(&Data->Level.MovableObjects[Data->Level.number_of_movable_objects - 1]);
    }

    Data->loading_state = 8;
    /**
        Particles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewParticle = (struct particleData*)malloc(sizeof(struct particleData));
        add_movable_object(&Data->Level, motPARTICLE, (void*)Factory.NewParticle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f",    &Factory.NewParticle->center.x,
                                            &Factory.NewParticle->center.y,
                                            &Factory.NewParticle->r,
                                            &Factory.NewParticle->mass,
                                            &Factory.NewParticle->charge);

        construct_movable(&Data->Level.MovableObjects[Data->Level.number_of_movable_objects - 1]);
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

void draw_loading(struct GameSharedData *Data){
    ALLEGRO_TRANSFORM tempT;
    char buffer[11];
    strcpy(buffer, "LOADING........");
    int i;
    for(i = 0; i < 15 - Data->loading_state; ++i){
        buffer[15 - i] = ' ';
    }
    al_identity_transform(&tempT);
    al_use_transform(&tempT);

    al_draw_text(Data->MenuBigFont,
                 al_map_rgb(255, 255, 255),
                 Data->DisplayData.width / 2,
                 Data->DisplayData.height / 2,
                 ALLEGRO_ALIGN_CENTRE,
                 buffer);

    al_use_transform(&Data->Transformation);
}

void handle_event_loading(struct GameSharedData *Data){
    switch(Data->LastEvent.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->CloseNow = true;
            break;
    }
}

void request_loading(struct GameSharedData *Data){
    if(!Data->ThreadLoading){
        fprintf(stderr, "Failed to create loading thread.");
        Data->CloseNow = true;
    }
    else{
        Data->RequestChangeState = false;
        Data->GameState = gsLOADING;
        Data->DrawFunction = draw_loading;
        al_start_thread(Data->ThreadLoading);
    }
}
