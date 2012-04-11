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
                    }
                    break;
                case ALLEGRO_KEY_RIGHT:
                    if(Data->Menu.CurrentMenu[Data->Menu.Current].Type == metUPDOWN){
                        arg.CallType = meatUP;
                        (Data->Menu.CurrentMenu[Data->Menu.Current].Activate)((void*)&arg);
                    }
                    break;
                case ALLEGRO_KEY_UP:
                    Data->Menu.Current -= 1;
                    normalize_menu_selection(&(Data->Menu));
                    break;
                case ALLEGRO_KEY_DOWN:
                    Data->Menu.Current += 1;
                    normalize_menu_selection(&(Data->Menu));
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
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    return_menu(&(Data->Menu));
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
