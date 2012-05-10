#include "main.h"
#include "loading.h"
#include <stdio.h>
#include <string.h>

void* load_level(ALLEGRO_THREAD *thread, void *argument){
    printf("Loading... level\n");
    #define Data ((struct GameSharedData*)argument)

    // Actual loading
    load_level_from_file(Data);

    initialize_level(Data);

    printf("Loading finished\n");
    al_lock_mutex(Data->MutexChangeState);
        Data->NewState = gsGAME;
        Data->RequestChangeState = true;
    al_unlock_mutex(Data->MutexChangeState);

    return NULL;
    #undef Data
};

void initialize_level(struct GameSharedData *Data){
    int i;
    al_lock_mutex(Data->DrawMutex);
        al_set_target_bitmap(al_get_backbuffer(Data->Display));
        al_clear_to_color(al_map_rgb(170, 0, 0));
        if(Data->Level.Background){
            al_draw_bitmap(Data->Level.Background, 0, 0, 0);
        }else{
            Data->Level.Background = al_create_bitmap(800, 800);
        }

        for(i = 0; i < Data->Level.NumberOfFixedObjects; ++i){
            DRAW(Data->Level.FixedObjects[i]);
        }

        //This loop will be moved to game section, when it will be executed over and over again
        for(i = 0; i < Data->Level.NumberOfMovableObjects; ++i){
           DRAW(Data->Level.MovableObjects[i]);
        }

        al_set_target_bitmap(Data->Level.Background);
        al_draw_bitmap(al_get_backbuffer(Data->Display), 0, 0, 0);
        al_set_target_bitmap(al_get_backbuffer(Data->Display));

    al_unlock_mutex(Data->DrawMutex);
}

void load_level_from_file(struct GameSharedData *Data){
    ALLEGRO_FILE *level;
    char buffer[1024];
    int n, i, op0, op1;
    struct ObjectWorkshop Factory;

    strcpy(buffer, "Data/Levels/level");
    strcat(buffer, int_to_str(Data->Level.LevelNumber));
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
    strcpy(buffer, "Data/Levels/");
    read_line(buffer + 12, level);

    if(strcmp(buffer + 12, "0") == 0){
        Data->Level.Background = NULL;
    }else{
        Data->Level.Background = al_load_bitmap(buffer);
    }
    /**
        Scaling Background
        */

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
        add_movable_object(Data, motPLAYER, (void*)Data->Level.Player);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f", &Data->Level.Player->center.x,
                                   &Data->Level.Player->center.y,
                                   &Data->Level.Player->ang);
        /**
            Firing player constructor, that happens not to be present in the code at the moment
            */
        construct_player(&Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects - 1]);
    }

    /**
        Rectangles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewRectangle = (struct rectangleData*)malloc(sizeof(struct rectangleData));
        add_fixed_object(Data, fotRECTANGLE, (void*)Factory.NewRectangle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewRectangle->center.x,
                                            &Factory.NewRectangle->center.y,
                                            &Factory.NewRectangle->a,
                                            &Factory.NewRectangle->b,
                                            &Factory.NewRectangle->ang,
                                            &op0);

        read_color(buffer, level, &Factory.NewRectangle->color, op0, "Invalid level input: rectangle#%d color\n", i);
        /**
            Firing temporarily non-existant rectangle constructor
            */
        construct_rectangle(&Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects - 1]);
    }

    /**
        Circles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewCircle = (struct circleData*)malloc(sizeof(struct circleData));
        add_fixed_object(Data, fotCIRCLE, (void*)Factory.NewCircle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %d",   &Factory.NewCircle->center.x,
                                        &Factory.NewCircle->center.y,
                                        &Factory.NewCircle->r0,
                                        &op0);
        read_color(buffer, level, &Factory.NewCircle->color, op0, "Invalid level input: circle#%d color\n", i);
        /**
            Firing circle constructor... oh, it's not here! I wonder why...
            */
        construct_circle(&Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects - 1]);
    }

    /**
        Squares
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewSquare = (struct squareData*)malloc(sizeof(struct squareData));
        add_fixed_object(Data, fotSQUARE, (void*)Factory.NewSquare);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %d",    &Factory.NewSquare->center.x,
                                            &Factory.NewSquare->center.y,
                                            &Factory.NewSquare->bok,
                                            &Factory.NewSquare->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewSquare->color, op0, "Invalid level input: square#%d color\n", i);
        /**
            They say this is a perfect place to run something they call "Square constructor"
            */
        construct_square(&Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects - 1]);
    }

    /**
        Entrances
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewEntrance = (struct entranceData*)malloc(sizeof(struct entranceData));
        add_fixed_object(Data, fotENTRANCE, (void*)Factory.NewEntrance);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewEntrance->center.x,
                                            &Factory.NewEntrance->center.y,
                                            &Factory.NewEntrance->a,
                                            &Factory.NewEntrance->b,
                                            &Factory.NewEntrance->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewEntrance->color, op0, "Invalid level input: entrance#%d color\n", i);
        /**
            Enter the Matrix with my constructor that does not exist
            */
        construct_rectangle(&Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects - 1]);//for the meantime
    }

    /**
        Exits
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewExit = (struct exitData*)malloc(sizeof(struct exitData));
        add_fixed_object(Data, fotEXIT, (void*)Factory.NewExit);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f %d", &Factory.NewExit->center.x,
                                            &Factory.NewExit->center.y,
                                            &Factory.NewExit->a,
                                            &Factory.NewExit->b,
                                            &Factory.NewExit->ang,
                                            &op0);
        read_color(buffer, level, &Factory.NewExit->color, op0, "Invalid level input: rectangle#%d color\n", i);

        /**
            Calling saws and hammers for constructing an exit
            */
        construct_rectangle(&Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects - 1]);//destroy this after righting the function!
    }

    /**
        Switches
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewSwitch = (struct switchData*)malloc(sizeof(struct switchData));
        add_movable_object(Data, motSWITCH, (void*)Factory.NewSwitch);

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
        /**
            This is the right place, yes Luke
            */
        construct_rectangle((struct fixed_object_structure*)&Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects - 1]);//for the meantime also
    }

    /**
        Doors
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewDoor = (struct doorData*)malloc(sizeof(struct doorData));
        add_movable_object(Data, motDOOR, (void*)Factory.NewDoor);

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

         /**
            The door should be now built
            */
        construct_rectangle((struct fixed_object_structure*)&Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects - 1]);//the last one to be DESTROYED!
    }

    /**
        Particles
        */
    read_line(buffer, level);
    sscanf(buffer, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewParticle = (struct particleData*)malloc(sizeof(struct particleData));
        add_movable_object(Data, motPARTICLE, (void*)Factory.NewParticle);

        read_line(buffer, level);
        sscanf(buffer, "%f %f %f %f %f",    &Factory.NewParticle->center.x,
                                            &Factory.NewParticle->center.y,
                                            &Factory.NewParticle->r0,
                                            &Factory.NewParticle->mass,
                                            &Factory.NewParticle->charge);
        /**
            I heard news, that here will be particle constructor
            */
        construct_particle(&Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects - 1]);
    }

    al_fclose(level);
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
    al_clear_to_color(al_map_rgb(0,0,0));

    al_draw_text(Data->MenuBigFont,
                 al_map_rgb(255,255,255),
                 Data->DisplayData.width / 2,
                 Data->DisplayData.height / 2,
                 ALLEGRO_ALIGN_CENTRE,
                 "LOADING");

    al_flip_display();
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
        al_lock_mutex(Data->MutexChangeState);
            Data->RequestChangeState = false;
            Data->GameState = gsLOADING;
        al_unlock_mutex(Data->MutexChangeState);
        al_start_thread(Data->ThreadLoading);
    }
}
