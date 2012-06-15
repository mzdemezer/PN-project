#ifndef _INCLUDE_MENU_H
#define _INCLUDE_MENU_H

/**
    Negatives values are for configuration menus
    */

#define DEEPEST_MENU_LEVEL 2

#define MAIN_MENU_SIZE 4
#define OPTIONS_MENU_SIZE 4
#define GRAPHIC_MENU_SIZE -2
#define SOUND_MENU_SIZE -1
#define CONTROLS_MENU_SIZE -9

typedef enum menu_elem_type{
    metSUBMENU,
    metACTIVATE,
    metINPUT,
    metUPDOWN
}menu_elem_type;

/**
    For descriptor the first byte - type -
    is the number of elements in array,
    for the rest is the information how to
    cast activate pointer - on function or
    on other menu for submenu.
    */
typedef struct menu_elem{
    menu_elem_type type;
    char *name;
    void (*activate)(void*);
}menu_elem;

enum main_menu_elems{
    mmeDESCRIPTOR,
    mmeNEWGAME,
    mmeOPTIONS,
    mmeHIGHSCORES,
    mmeEXIT,
    mmeRETURN
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
    cmeUP,
    cmeDOWN,
    cmeLEFT,
    cmeRIGHT,
    cmeSHIELD,
    cmeNEG,
    cmePOS,
    cmeGRAV,
    cmeRETURN
};

typedef struct menu_structure{
    menu_elem *current_menu;
    int current_elem;
}menu_structure;

/**
    Public methods
    */
menu_elem *create_menu();
void destroy_menu(menu_structure *menu);
void make_main_menu_unpause(menu_structure *menu);
void make_main_menu_pause_menu(menu_structure *menu);

void normalize_menu_selection(menu_structure *);

#endif
