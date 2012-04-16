#include "main.h"
#include "loading.h"
#include <stdio.h>

void draw_loading(struct GameSharedData *Data){
    //printf("Drawing the loading-screen\n");

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
