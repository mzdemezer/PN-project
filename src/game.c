#include "main.h"
#include "game.h"

void handle_event_game(struct GameSharedData *Data){
    //printf("event handled in game\n");
    switch(Data->LastEvent.type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->CloseNow = true;
            break;
    }
}

void draw_game(struct GameSharedData *Data){
    al_clear_to_color(al_map_rgb(0,0,0));

    al_draw_text(Data->MenuBigFont, al_map_rgb(255,255,255), Data->DisplayData.width / 2, Data->DisplayData.height / 2, ALLEGRO_ALIGN_CENTRE, "GAME");

    al_flip_display();
}

void request_game(struct GameSharedData *Data){
    al_lock_mutex(Data->MutexChangeState);
        Data->RequestChangeState = false;
        Data->GameState = gsGAME;
    al_unlock_mutex(Data->MutexChangeState);
    al_start_thread(Data->ThreadMainIteration);
}
