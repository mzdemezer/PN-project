#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "menu.h"

void normalize_menu_selection(struct menu_structure *Menu){
    int NumberOfElems = abs(Menu->CurrentMenu->Type);
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

void normalize_resolution_selection(int*current, const int max){
    if(*current<0){
        *current = 0;
    }
    else if(*current > max){
        *current = max;
    }
};

void new_game_activate(void *argument){
    printf("NEW GAME ACTIVATED\n");
};

void exit_activate(void *argument){
    printf("EXIT ACTIVATED\n");

    struct activation_argument *arg = (struct activation_argument*) argument;
    arg->Data->CloseNow = true;
};

void scale_fonts(struct GameSharedData *Data){
    Data->MenuBigFont = NULL;
    Data->MenuBigFont = al_load_ttf_font("pirulen.ttf", (int)(Data->DisplayData.height / 10), 0);

    if (!Data->MenuRegularFont){
        fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
        exit(-1);
    }

    Data->MenuRegularFont = al_load_ttf_font("pirulen.ttf", (int)(Data->MenuBigFont->height * 0.62), 0);
    Data->MenuSelectedFont = al_load_ttf_font("pirulen.ttf", (int)((Data->MenuBigFont->height +  Data->MenuRegularFont->height) / 2), 0);
    Data->MenuConfigFont = al_load_ttf_font("pirulen.ttf", (int)(Data->MenuRegularFont->height * 0.62), 0);
    Data->MenuConfigSelectedFont = al_load_ttf_font("pirulen.ttf", (int)(Data->MenuSelectedFont->height * 0.62), 0);
}

void resolution_activate(void*argument){
    printf("RESOLUTION CHANGE\n");

    struct activation_argument *arg = (struct activation_argument*) argument;
    switch(arg->CallType){
        case meatACCEPT:
            arg->Data->ChosenResolution = arg->Data->ChosenInMenu;
            arg->Data->DisplayData = arg->Data->InMenuDisplayData;
            al_destroy_display(arg->Data->Display);
            arg->Data->Display = al_create_display(arg->Data->DisplayData.width, arg->Data->DisplayData.height);
            scale_fonts(arg->Data);
            break;
        case meatUP:
            arg->Data->ChosenInMenu += 1;
            normalize_resolution_selection(&arg->Data->ChosenInMenu, arg->Data->MaxResolutionIndex);
            al_get_display_mode(arg->Data->ChosenResolution, &arg->Data->InMenuDisplayData);
            break;
        case meatDOWN:
            arg->Data->ChosenInMenu -= 1;
            normalize_resolution_selection(&arg->Data->ChosenInMenu, arg->Data->MaxResolutionIndex);
            al_get_display_mode(arg->Data->ChosenResolution, &arg->Data->InMenuDisplayData);
            break;
        case meatDRAW:

        default: return;
    }
}

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
    printf("event handled in menu #%d\n", ev->type);
};

void clear_menu(){
    al_clear_to_color(al_map_rgb(0,0,0));
}

void draw_menu(struct GameSharedData* Data){
    int i, NumberOfElems;
    ALLEGRO_FONT *RegFont, *SelectFont;

    clear_menu();

    //Normal menu
    if((int)Data->Menu.CurrentMenu[0].Type > 0){
        RegFont = Data->MenuRegularFont;
        SelectFont = Data->MenuSelectedFont;
    }
    else{//Configuration menu
        RegFont = Data->MenuConfigFont;
        SelectFont = Data->MenuConfigSelectedFont;
    }

    NumberOfElems = abs(Data->Menu.CurrentMenu[0].Type);
    al_draw_text(Data->MenuBigFont, al_map_rgb(255,255,255), Data->DisplayData.width/2, Data->DisplayData.height/10, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[0].Name);
    for(i = 1; i < Data->Menu.Current; ++i){
        al_draw_text(RegFont, al_map_rgb(255,255,255), Data->DisplayData.width/2, (i + 1.5) * (Data->MenuBigFont->height * 1.11), ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    }
    al_draw_text(SelectFont, al_map_rgb(255,255,0),Data->DisplayData.width/2, (i + 1.5) * (Data->MenuBigFont->height * 1.11), ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    for(++i; i <= NumberOfElems; ++i){
        al_draw_text(RegFont, al_map_rgb(255,255,255), Data->DisplayData.width/2, (i + 1.5) * (Data->MenuBigFont->height * 1.11), ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[i].Name);
    }

    al_flip_display();

    Data->DrawCall = false;
}
