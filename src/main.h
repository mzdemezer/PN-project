/**
    This file includes code that is used by many other files
*/

#ifndef _INCLUDE_MAIN_H
#define _INCLUDE_MAIN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/**
    Negatives values are for configuration menus
*/

#define MAIN_MENU_SIZE 4
#define OPTIONS_MENU_SIZE 4
#define GRAPHIC_MENU_SIZE -2
#define HIGH_SCORES_MENU_SIZE 1
#define SOUND_MENU_SIZE -1
#define CONTROLS_MENU_SIZE -1

enum game_state{
    gsMENU,
    gsLOADING,
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
    meatDOWN,
    meatRESTORE_CURRENT,
    meatDRAW
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
    omeGRAPHIC,
    omeSOUND,
    omeCONTROLS,
    omeRETURN
};

enum graphic_menu_elems{
    gmeDESCRIPTOR,
    gmeRESOLUTION,
    gmeRETURN
};

enum sound_menu_elems{
    smeDESCRIPTOR,
    smeRETURN
};

enum controls_menu_elems{
    cmeDESCRIPTOR,
    cmeRETURN
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

enum fixed_object_type{//e.g.
    fotSQUARE,
    fotRECTANGLE
};

enum movable_object_type{//e.g.
    motPLAYER,
    motTURRET,
    motBULLET
};

/**
    This structure enables objectivization
    by casting on different objects in
    relation to Type. It anables also
    creating lists of objects that are
    logically connected with each other
    */

struct fixed_object_structure{
    enum fixed_object_type Type;
    void* ObjectData;
};

struct movable_object_structure{
    enum fixed_object_type Type;
    void* ObjectData;
};

struct level_structure{
    int LevelNumber;

    int NumberOfMovableObjects;
    int NumberOfFixedObjects;
    struct fixed_object_structure *Fixed;
    struct fixed_object_structure *Movable;
};

struct GameSharedData{
    enum game_state GameState;
    bool MouseWorking;

    ALLEGRO_THREAD *LoadingThread;

    ALLEGRO_DISPLAY *Display;
    ALLEGRO_DISPLAY_MODE DisplayData;
    ALLEGRO_DISPLAY_MODE InMenuDisplayData;
    int MaxResolutionIndex;
    int ChosenResolution;
    int ChosenInMenu;

    ALLEGRO_EVENT_QUEUE *MainEventQueue;
    ALLEGRO_EVENT LastEvent;

    struct menu_structure Menu;
    ALLEGRO_FONT *MenuRegularFont;
    ALLEGRO_FONT *MenuBigFont;
    ALLEGRO_FONT *MenuSelectedFont;
    ALLEGRO_FONT *MenuConfigFont;
    ALLEGRO_FONT *MenuConfigSelectedFont;

    //struct keyboard_structure Keyboard;

    struct level_structure Level;

    /**
        Flags
        */
    bool DrawCall;
    bool CloseNow;
};

void* load_level(ALLEGRO_THREAD *, void *);

//Math

int abs(int);



#endif
