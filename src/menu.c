#include <stdio.h>

#include "main.h"
#include "menu.h"

void normalize_menu_selection(struct menu_structure *Menu){
    int NumberOfElems = (int)(Menu->CurrentMenu->Type);
    if(Menu->Current > NumberOfElems){
        Menu->Current = 1;
    }
    else if(Menu->Current <= 0){
        Menu->Current = NumberOfElems;
    }
    printf("\nCurrent: %d\n"
           "Name: %s\n"
           "Type: %d\n", Menu->Current, Menu->CurrentMenu[Menu->Current].Name,
                                      Menu->CurrentMenu[Menu->Current].Type);
}

void new_game_activate(void *argument){
    printf("NEW GAME ACTIVATED\n");
};

void exit_activate(void *argument){
    printf("EXIT ACTIVATED\n");

    struct activation_argument *arg = (struct activation_argument*) argument;
    arg->Data->CloseNow = true;
};

void change_menu(struct menu_structure *Menu, struct menu_elem *NewMenu, int NewCurrent){
    Menu->CurrentMenu = NewMenu;
    Menu->Current = NewCurrent;
    printf("MENU CHANGED\n");
}

void return_menu(struct menu_structure *Menu){
    change_menu(Menu, (struct menu_elem*)Menu->CurrentMenu[0].Activate, 1);
    normalize_menu_selection(Menu);
}


void handle_event_menu(const ALLEGRO_EVENT *ev, struct GameSharedData *Data){
    struct activation_argument arg;
    arg.Data = Data;
    switch(ev->type){
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            Data->CloseNow = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            printf("keycode down: %d\n", ev->keyboard.keycode);
            switch(ev->keyboard.keycode){
                case ALLEGRO_KEY_LEFT:
                    if(Data->Menu.CurrentMenu[Data->Menu.Current].Type == metUPDOWN){
                        arg.CallType = meatDOWN;
                        (Data->Menu.CurrentMenu[Data->Menu.Current].Activate)((void*)&arg);
                        Data->DrawCall = true;
                    }
                    break;
                case ALLEGRO_KEY_RIGHT:
                    if(Data->Menu.CurrentMenu[Data->Menu.Current].Type == metUPDOWN){
                        arg.CallType = meatUP;
                        (Data->Menu.CurrentMenu[Data->Menu.Current].Activate)((void*)&arg);
                        Data->DrawCall = true;
                    }
                    break;
                case ALLEGRO_KEY_UP:
                    Data->Menu.Current -= 1;
                    normalize_menu_selection(&(Data->Menu));
                    Data->DrawCall = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    Data->Menu.Current += 1;
                    normalize_menu_selection(&(Data->Menu));
                    Data->DrawCall = true;
                    break;
                case ALLEGRO_KEY_ENTER:
                    arg.CallType = meatACCEPT;
                    switch (Data->Menu.CurrentMenu[Data->Menu.Current].Type){
                        case metSUBMENU:
                            change_menu(&(Data->Menu), (struct menu_elem*) Data->Menu.CurrentMenu[Data->Menu.Current].Activate, 1);
                            break;
                        case metACTIVATE:

                            (Data->Menu.CurrentMenu[Data->Menu.Current].Activate)((void*)&arg);
                            break;
                        case metUPDOWN:
                            (Data->Menu.CurrentMenu[Data->Menu.Current].Activate)((void*)&arg);
                            break;
                    }
                    Data->DrawCall = true;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    return_menu(&(Data->Menu));
                    Data->DrawCall = true;
                    break;
            }
            break;
        case ALLEGRO_EVENT_KEY_UP:
            printf("keycode up: %d\n", ev->keyboard.keycode);
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            printf("mouse moved in menu\n");
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            printf("mouse up: %d\n", ev->mouse.button);
            break;
    }
    printf("event handled in menu\n");
};

void clear_menu(){
    al_clear_to_color(al_map_rgb(0,0,0));
}

void draw_menu(struct GameSharedData* Data){
    int i, NumberOfElems;

    clear_menu();

    NumberOfElems = Data->Menu.CurrentMenu[0].Type;
    al_draw_text(Data->MenuBigFont, al_map_rgb(255,255,255), 400, 120, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[0].Name);
    for(i = 1; i < Data->Menu.Current; ++i){
        al_draw_text(Data->MenuRegularFont, al_map_rgb(255,255,255), 400, (i + 1.5) * 80, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    }
    al_draw_text(Data->MenuSelectedFont, al_map_rgb(255,255,0), 400, (i + 1.5) * 80, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    for(++i; i <= NumberOfElems; ++i){
        al_draw_text(Data->MenuRegularFont, al_map_rgb(255,255,255), 400, (i + 1.5) * 80, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    }

    al_flip_display();

    Data->DrawCall = false;
}
