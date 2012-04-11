/**This file includes code that is used by many other files

*/

#ifndef _INCLUDE_MAIN_H
#define _INCLUDE_MAIN_H

#include <allegro5/allegro.h>

#define MAIN_MENU_SIZE 4
#define HIGH_SCORES_MENU_SIZE 1
#define OPTIONS_MENU_SIZE 1

enum game_state_enum{
    gsMENU,
    gsPAUSE,
    gsGAME
};

enum menu_elem_type{
    metSUBMENU,
    metACTIVATE,
    metUPDOWN
};

enum menu_elem_activation_type{
    meatACCEPT,
    meatUP,
    meatDOWN
};

struct activation_argument{
    enum menu_elem_activation_type CallType;
    struct GameSharedData *Data;
};

struct menu_elem{
    enum menu_elem_type Type;
    char *Name;
    void (*Activate)(void*);
};

enum main_menu_elems{
    mmeDESCRIPTOR,
    mmeNEWGAME,
    mmeOPTIONS,
    mmeHIGHSCORES,
    mmeEXIT
};

enum options_menu_elems{
    omeDESCRIPTOR,
    omeRETURN
};

enum high_scores_menu_elems{
    hsmeDESCRIPTOR,
    hsmeRETURN
};

void menu_elem_init(struct menu_elem*Item,
                    enum menu_elem_type NewType,
                    char *NewName,
                    void *NewActivate);

struct menu_structure{
    struct menu_elem *CurrentMenu;
    int Current;
};

/*enum DecodeKeys{LEFT, RIGHT, UP, DOWN, ENTER};
struct keyboard_structure{
    bool Keys[5];
};*/

struct GameSharedData{
    enum game_state_enum GameState;
    bool MouseWorking;

    struct menu_structure Menu;
    //struct keyboard_structure Keyboard;

    bool CloseNow;
};







#endif
