#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    Data->MenuConfigFont = al_load_ttf_font("pirulen.ttf", (int)(Data->MenuRegularFont->height * 0.45), 0);
    Data->MenuConfigSelectedFont = al_load_ttf_font("pirulen.ttf", (int)(Data->MenuSelectedFont->height * 0.45), 0);
}

int rzad(int a){
    int res = 1, lim = 10;
    a = abs(a);
    while(a > lim){
        res += 1;
        lim *= 10;
    }
    return res;
}

char int_to_char(int digit){
    return (char)(digit | 0x30);
}

char* int_to_str(int a){
    bool sign = a < 0;
    int length = rzad(a) + (int)sign, i, digit;
    char* result = (char*)malloc(sizeof(char) * (length + 1) );
    result[length] = '\0';
    a = abs(a);
    for(i = length - 1; i >= (int)sign; --i){
        digit = a % 10;
        a /= 10;
        result[i] = int_to_char(digit);
    }
    if(sign){
        result[0] = '-';
    }
    return result;
}

char* stringify_resolution(const ALLEGRO_DISPLAY_MODE *DispData){
    char* str = (char*)malloc(sizeof(char) * 20 ); //na zapas
    strcpy(str, int_to_str(DispData->width));
    strcpy(str + strlen(str) + 1, int_to_str(DispData->height));
    str[strlen(str)] = 'x';
    return str;
}

void resolution_activate(void*argument){
    ALLEGRO_FONT *Font;
    ALLEGRO_COLOR Color;
    char *CurrentResolution;

    printf("RESOLUTION CHANGE\n");

    struct activation_argument *arg = (struct activation_argument*) argument;
    switch(arg->CallType){
        case meatACCEPT:
            if(arg->Data->ChosenResolution != arg->Data->ChosenInMenu){
                arg->Data->ChosenResolution = arg->Data->ChosenInMenu;
                arg->Data->DisplayData = arg->Data->InMenuDisplayData;
                al_destroy_display(arg->Data->Display);
                arg->Data->Display = al_create_display(arg->Data->DisplayData.width, arg->Data->DisplayData.height);
                scale_fonts(arg->Data);
            }
            break;
        case meatUP:
            arg->Data->ChosenInMenu += 1;
            normalize_resolution_selection(&arg->Data->ChosenInMenu, arg->Data->MaxResolutionIndex);
            al_get_display_mode(arg->Data->ChosenInMenu, &arg->Data->InMenuDisplayData);
            break;
        case meatDOWN:
            arg->Data->ChosenInMenu -= 1;
            normalize_resolution_selection(&arg->Data->ChosenInMenu, arg->Data->MaxResolutionIndex);
            al_get_display_mode(arg->Data->ChosenInMenu, &arg->Data->InMenuDisplayData);
            break;
        case meatRESTORE_CURRENT: printf("\nyes\n\n");
            arg->Data->ChosenInMenu = arg->Data->ChosenResolution;
            arg->Data->InMenuDisplayData = arg->Data->DisplayData;
            break;
        default:
                if(arg->CallType - (int) meatDRAW == arg->Data->Menu.Current){
                    Font = arg->Data->MenuConfigSelectedFont;
                    Color = al_map_rgb(255, 255, 0);
                }
                else{
                    Font = arg->Data->MenuConfigFont;
                    Color = al_map_rgb(255, 255, 255);
                }
                CurrentResolution = stringify_resolution(&arg->Data->InMenuDisplayData);
                al_draw_text(Font, Color, arg->Data->DisplayData.width * 0.8 , (arg->CallType - meatDRAW + 1.5) * (arg->Data->MenuBigFont->height * 1.11), ALLEGRO_ALIGN_RIGHT, CurrentResolution);
    }
}

void restore_current_settings(struct GameSharedData *Data){
    int i;
    struct activation_argument arg;
    arg.CallType = meatRESTORE_CURRENT;
    arg.Data = Data;
    if((int)Data->Menu.CurrentMenu[0].Type < 0){
        for(i = 1; i <= abs((int)Data->Menu.CurrentMenu[0].Type); ++i){
            printf("I'm trying, master\n");
            if(Data->Menu.CurrentMenu[i].Type == metUPDOWN){
                Data->Menu.CurrentMenu[i].Activate((void*)&arg);
                printf("I'm working, master\n");
            }
        }
    }
}

void change_menu(struct GameSharedData *Data, struct menu_elem *NewMenu, int NewCurrent){
    restore_current_settings(Data);
    Data->Menu.CurrentMenu = NewMenu;
    Data->Menu.Current = NewCurrent;
    printf("MENU CHANGED\n");
}

void return_menu(struct GameSharedData *Data){
    change_menu(Data, (struct menu_elem*)Data->Menu.CurrentMenu[0].Activate, 1);
    normalize_menu_selection(&(Data->Menu));
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
                            change_menu(Data, (struct menu_elem*) Data->Menu.CurrentMenu[Data->Menu.Current].Activate, 1);
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
                    return_menu(Data);
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
};

void clear_menu(){
    al_clear_to_color(al_map_rgb(0,0,0));
}

void draw_menu(struct GameSharedData* Data){
    int i, NumberOfElems, flags;
    float align_x;
    struct activation_argument arg;
    ALLEGRO_FONT *RegFont, *SelectFont;

    clear_menu();

    //Normal menu
    if((int)Data->Menu.CurrentMenu[0].Type > 0){
        RegFont = Data->MenuRegularFont;
        SelectFont = Data->MenuSelectedFont;
        flags = ALLEGRO_ALIGN_CENTRE;
        align_x = Data->DisplayData.width/2;
    }
    else{//Configuration menu
        RegFont = Data->MenuConfigFont;
        SelectFont = Data->MenuConfigSelectedFont;
        flags = ALLEGRO_ALIGN_LEFT;
        align_x = Data->DisplayData.width/10;
    }

    NumberOfElems = abs(Data->Menu.CurrentMenu[0].Type);
    al_draw_text(Data->MenuBigFont, al_map_rgb(255,255,255), Data->DisplayData.width/2, Data->DisplayData.height/10, ALLEGRO_ALIGN_CENTRE, Data->Menu.CurrentMenu[0].Name);
    for(i = 1; i < Data->Menu.Current; ++i){
        al_draw_text(RegFont, al_map_rgb(255,255,255), align_x, (i + 1.5) * (Data->MenuBigFont->height * 1.11), flags, Data->Menu.CurrentMenu[i].Name);
        if(Data->Menu.CurrentMenu[i].Type == metUPDOWN){
            arg.CallType = meatDRAW + i;
            arg.Data = Data;
            Data->Menu.CurrentMenu[i].Activate((void*)&arg);
        }
    }
    al_draw_text(SelectFont, al_map_rgb(255,255,0), align_x, (i + 1.5) * (Data->MenuBigFont->height * 1.11), flags, Data->Menu.CurrentMenu[i].Name);
    if(Data->Menu.CurrentMenu[i].Type == metUPDOWN){
            arg.CallType = meatDRAW + i;
            arg.Data = Data;
            Data->Menu.CurrentMenu[i].Activate((void*)&arg);
        }
    for(++i; i <= NumberOfElems; ++i){
        al_draw_text(RegFont, al_map_rgb(255,255,255), align_x, (i + 1.5) * (Data->MenuBigFont->height * 1.11), flags, Data->Menu.CurrentMenu[i].Name);
        if(Data->Menu.CurrentMenu[i].Type == metUPDOWN){
            arg.CallType = meatDRAW + i;
            arg.Data = Data;
            Data->Menu.CurrentMenu[i].Activate((void*)&arg);
        }
    }

    al_flip_display();

    Data->DrawCall = false;
}
