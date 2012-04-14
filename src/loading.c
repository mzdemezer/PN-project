#include "main.h"
#include "loading.h"

void draw_loading(struct GameSharedData *Data){
    printf("Drawing the loading-screen\n");

    Data->DrawCall = false;

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
