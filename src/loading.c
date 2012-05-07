#include "main.h"
#include "loading.h"
#include <stdio.h>
#include <string.h>

void* load_level(ALLEGRO_THREAD *thread, void *argument){
    printf("Loading... level\n");

    struct GameSharedData *Data = (struct GameSharedData*) argument;

    // Actual loading
    load_level_from_file(Data);

    printf("Loading finished\n");
    al_lock_mutex(Data->MutexChangeState);
        Data->NewState = gsGAME;
        Data->RequestChangeState = true;
    al_unlock_mutex(Data->MutexChangeState);

    return NULL;
};

void load_level_from_file(struct GameSharedData *Data){
    FILE *level;
    char buffer[255];
    int n, i, op0, op1, op2, op3, op4;
    struct ObjectWorkshop Factory;

    strcpy(buffer, "Data/Levels/level");
    strcat(buffer, int_to_str(Data->Level.LevelNumber));
    strcat(buffer, ".lev");

    level = fopen(buffer, "r");

    if (level == NULL) {
        fprintf(stderr, "Failed to open %s\n", buffer);
        Data->CloseNow = true;
        return;
    }

    /**
        Loading level Background
        */
    strcpy(buffer, "Data/Levels/");
    fscanf(level, "%s", buffer+12);

    if(strcmp(buffer, "0") == 0){
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
    fscanf(level, "%d", &n);
    for(i = 0; i < n; ++i){
        Data->Level.Player = (struct playerData*)malloc(sizeof(struct playerData));
        add_movable_object(Data, motPLAYER, (void*)Data->Level.Player);

        fscanf(level, "%f %f %f", &Data->Level.Player->x,
                                  &Data->Level.Player->y,
                                  &Data->Level.Player->ang);
        /**
            Firing player constructor, that happens not to be present in the code at the moment
            */
    }

    /**
        Rectangles
        */
    fscanf(level, "%d", &n);
    for(i = 0; i < n; ++i){
        Factory.NewRectangle = (struct rectangleData*)malloc(sizeof(struct rectangleData));
        add_fixed_object(Data, fotRECTANGLE, (void*)Factory.NewRectangle);

        fscanf(level, "%f %f %f %f %f", &Factory.NewRectangle->x,
                                        &Factory.NewRectangle->y,
                                        &Factory.NewRectangle->a,
                                        &Factory.NewRectangle->b,
                                        &Factory.NewRectangle->ang);
        fscanf(level, "%d", &op0);
        switch(op0){
            case 3:
                fscanf(level, "%d %d %d", &op1, &op2, &op3);
                Factory.NewRectangle->color = al_map_rgb(op1, op2, op3);
                break;
            case 4:
                fscanf(level, "%d %d %d %d", &op1, &op2, &op3, &op4);
                Factory.NewRectangle->color = al_map_rgba(op1, op2, op3, op4);
                break;
            default:
                fprintf(stderr, "Invalid level input: rectangle#%d color", i);
                Factory.NewRectangle->color = al_map_rgb(128, 128, 128);
        /**
            Firing temporarily non-existant rectangle constructor
            */
        }
    }
    fclose(level);
}

void draw_loading(struct GameSharedData *Data){
    al_clear_to_color(al_map_rgb(0,0,0));

    al_draw_text(Data->MenuBigFont, al_map_rgb(255,255,255), Data->DisplayData.width / 2, Data->DisplayData.height / 2, ALLEGRO_ALIGN_CENTRE, "LOADING");

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
