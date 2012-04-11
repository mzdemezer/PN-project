#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "menu.h"
#include "game.h"

void menu_elem_init(struct menu_elem*Item,
                    enum menu_elem_type NewType,
                    char *NewName,
                    void *NewActivate){
    Item->Type = NewType;
    Item->Name = NewName;
    Item->Activate = NewActivate;
}

int main(){
    ALLEGRO_EVENT_QUEUE *main_event_queue = NULL;

    if(!al_init()){
        fprintf(stderr, "Problems when initilizing Allegro");
        return -1;
    }

    struct GameSharedData Data;
    Data.Display = NULL;
    Data.GameState = gsMENU;
    Data.CloseNow = false;
    Data.DrawCall = false;

    /**
        Menu initialization
        */

    struct menu_elem MainMenu[1 + MAIN_MENU_SIZE],
                     OptionsMenu[1 + OPTIONS_MENU_SIZE],
                     HighScoresMenu[1 + HIGH_SCORES_MENU_SIZE];

    menu_elem_init(&MainMenu[mmeDESCRIPTOR],
                   MAIN_MENU_SIZE,
                   "MAIN MENU",
                   &MainMenu);
    menu_elem_init(&MainMenu[mmeNEWGAME],
                   metACTIVATE,
                   "NEW GAME",
                   (void*) new_game_activate);
    menu_elem_init(&MainMenu[mmeOPTIONS],
                   metSUBMENU,
                   "OPTIONS",
                   (void*) &OptionsMenu);
    menu_elem_init(&MainMenu[mmeHIGHSCORES],
                   metSUBMENU,
                   "HIGH SCORES",
                   (void*) &HighScoresMenu);
    menu_elem_init(&MainMenu[mmeEXIT],
                   metACTIVATE,
                   "EXIT",
                   (void*) exit_activate);

    menu_elem_init(&OptionsMenu[omeDESCRIPTOR],
                   OPTIONS_MENU_SIZE,
                   "OPTIONS",
                   &MainMenu);
    menu_elem_init(&OptionsMenu[omeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);

    menu_elem_init(&HighScoresMenu[hsmeDESCRIPTOR],
                   HIGH_SCORES_MENU_SIZE,
                   "HIGH SCORES",
                   &MainMenu);
    menu_elem_init(&HighScoresMenu[hsmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);


    Data.Menu.CurrentMenu = &MainMenu;
    Data.Menu.Current = 1;

    al_init_font_addon();
    al_init_ttf_addon();

    /**
        Getting resolution
        */
    Data.DisplayWidth = 800;
    Data.DisplayHeight = 600;

    Data.Display = al_create_display(Data.DisplayWidth, Data.DisplayHeight);

    if(!Data.Display){
        fprintf(stderr, "Problems when creating the display");
        return -1;
    }


    /**
        Setting correct font size
        */
    Data.MenuRegularFont = al_load_ttf_font("pirulen.ttf",44,0 );
    Data.MenuBigFont = al_load_ttf_font("pirulen.ttf",72,0 );
    Data.MenuSelectedFont = al_load_ttf_font("pirulen.ttf",58,0 );

    if (!Data.MenuRegularFont){
        fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
        return -1;
    }

    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        printf("Nie możesz grać w tą grę bez klawiatury gościu...");
        return -1;
    }

    if(!al_install_mouse()) {
        fprintf(stderr, "failed to initialize the mouse!\n");
        Data.MouseWorking = false;
    }
    else{
        Data.MouseWorking = true;
    }

    main_event_queue = al_create_event_queue();
    if(!main_event_queue) {
        fprintf(stderr, "failed to create main_event_queue!\n");
        al_destroy_display(Data.Display);
        return -1;
    }
    al_register_event_source(main_event_queue, al_get_keyboard_event_source());
    al_register_event_source(main_event_queue, al_get_display_event_source(Data.Display));
    al_register_event_source(main_event_queue, al_get_mouse_event_source());

    draw_menu(&Data);

    ALLEGRO_EVENT ev;
    while(1){
        al_wait_for_event(main_event_queue, &ev);
        printf("event type#%d\n", ev.type);

        switch(Data.GameState){
            case gsMENU: handle_event_menu(&ev, &Data); break;
            case gsPAUSE: break;
            case gsGAME: handle_event_game(&ev, &Data); break;
        }

        if(Data.CloseNow){
            break;
        }

        if(Data.DrawCall){
                switch(Data.GameState){
                case gsMENU: draw_menu(&Data); break;
                case gsPAUSE: break;
                case gsGAME: break;
            }
        }
    }




    al_destroy_display(Data.Display);
    al_destroy_event_queue(main_event_queue);

    return 0;
}
