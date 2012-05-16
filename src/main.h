/**
    This file includes code that is used by many other files
*/

#ifndef _INCLUDE_MAIN_H
#define _INCLUDE_MAIN_H


//#define TESTS

#ifdef TESTS
#include "Tests/CuTest.h"
#endif

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define MAIN_BUFFER_WIDTH 1000
#define MAIN_BUFFER_HEIGHT 750

/**
    Negatives values are for configuration menus
*/

#define MAIN_MENU_SIZE 4
#define OPTIONS_MENU_SIZE 4
#define GRAPHIC_MENU_SIZE -2
#define HIGH_SCORES_MENU_SIZE 1
#define SOUND_MENU_SIZE -1
#define CONTROLS_MENU_SIZE -1

#define NUMBER_OF_SIGNIFICANT_KEYS 4

#define INITIAL_BOUNDRY_MOVABLE 300
#define INITIAL_BOUNDRY_FIXED 1000

#define ERROR_COLOR al_map_rgb(128, 128, 128)

#define dwaPI 6.28318531
#define PI74 5.49778714
#define PI32 4.71238898
#define PI54 3.92699081
#define PI 3.14159265
#define PI34 2.35619449
#define PIpol 1.57079633
#define PI4 0.78539816

#define SQRT2 1.41421356

#define eps 0.000001

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


enum fixed_object_type{
    fotRECTANGLE,
    fotCIRCLE,
    fotSQUARE,
    fotENTRANCE,
    fotEXIT
};

enum movable_object_type{
    motPLAYER,
    motSWITCH,
    motDOOR,
    motPARTICLE
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
    void (*draw)(void*);
    float (*r)(void*, float);
};

struct movable_object_structure{
    enum movable_object_type Type;
    void* ObjectData;
    void (*draw)(void*);
    float (*r)(void*, float);
};

struct ObjectWorkshop{
    struct playerData *NewPlayer;
    struct rectangleData *NewRectangle;
    struct circleData *NewCircle;
    struct squareData *NewSquare;
    struct entranceData *NewEntrance;
    struct exitData *NewExit;
    struct switchData *NewSwitch;
    struct doorData *NewDoor;
    struct particleData *NewParticle;
};

struct point{
    float x, y;
};

struct playerData{
    struct point center;
    float ang;
};

struct rectangleData{
    struct point center, v1, v2, v3, v4;
    float a, b, ang, r0, fi0, fi02, wsp1, wsp2;
    ALLEGRO_COLOR color;
};

struct circleData{
    struct point center;
    float r0, ang;
    ALLEGRO_COLOR color;
};

float squareEquation(float f0, float fi);

struct squareData{
    struct point center, v1, v2, v3, v4;
    float bok, ang, r0;
    ALLEGRO_COLOR color;
};

struct entranceData{
    struct point center, v1, v2, v3, v4;
    float a, b, ang, r0, fi0, fi02, wsp1, wsp2;
    ALLEGRO_COLOR color;
};

struct exitData{
    struct point center, v1, v2, v3, v4;
    float a, b, ang, r0, fi0, fi02, wsp1, wsp2;
    ALLEGRO_COLOR color;
};

enum switch_type{
    stSLIDE

};

struct connectedData{
    int number_of_doors,
        number_of_switches,
        *doors,
        *switches;
};

struct switchData{
    struct point center, v1, v2, v3, v4;
    float a, b, ang, r0, fi0, fi02, wsp1, wsp2;
    ALLEGRO_COLOR color;

    int pos;
    float mass;
    enum switch_type swType;
    struct connectedData connected;
};

enum door_type{
    dt1WINGSLIDE
};

struct doorData{
    struct point center, v1, v2, v3, v4;
    float a, b, ang, r0, fi0, fi02, wsp1, wsp2;
    ALLEGRO_COLOR color;

    int pos;
    float openingTime, mass;
    enum door_type doorType;
};

struct particleData{
    struct point center;
    float r0, mass, charge;
    ALLEGRO_COLOR color;
};

enum enum_keys{
    ekKEY_UP = 0,
    ekKEY_DOWN,
    ekKEY_LEFT,
    ekKEY_RIGHT
};

struct keyboard_structure{
    int KeyUp,
        KeyDown,
        KeyLeft,
        KeyRight;
    bool Flags[NUMBER_OF_SIGNIFICANT_KEYS];
    ALLEGRO_MUTEX *MutexKeyboard;
};

struct level_structure{
    int LevelNumber,
        NumberOfMovableObjects,
        NumberOfFixedObjects,
        BoundryMovable,
        BoundryFixed;
    struct fixed_object_structure *FixedObjects;
    struct movable_object_structure *MovableObjects;

    struct playerData *Player;

    ALLEGRO_BITMAP *Background;
    char filename[256];
};

struct scale_structure{
    float scale_x, scale_y,
          scale_w, scale_h,
          scale;
};

/**
    Auxilary threads
    */

struct iteration_thread_structure{
    ALLEGRO_THREAD *Thread;
    bool Finished;
    void* (*Job)(ALLEGRO_THREAD *, void*);
};

struct GameSharedData{
    enum game_state GameState;
    bool RequestChangeState;
    enum game_state NewState;
    ALLEGRO_MUTEX *MutexChangeState;

    ALLEGRO_MUTEX *MutexFPS;
    unsigned char FPS;
    ALLEGRO_MUTEX *DrawMutex;
    ALLEGRO_TIMER *DrawTimer;
    void (*DrawFunction)(struct GameSharedData *);

    ALLEGRO_THREAD *ThreadLoading;
    ALLEGRO_THREAD *ThreadEventQueue;
    ALLEGRO_THREAD *ThreadMainIteration;
    ALLEGRO_COND *CondMainIteration;
    ALLEGRO_MUTEX *MutexMainIteration;
    ALLEGRO_MUTEX *MutexIterations;
    ALLEGRO_COND *CondIterations;
    #define NumOfThreads 3
    struct iteration_thread_structure IterationThreads[NumOfThreads];
    bool IterationFinished;
    bool MainIterationThreadsIsWaiting;
    bool CloseLevel;

    struct scale_structure scales;
    ALLEGRO_BITMAP *main_buffer;
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

    struct keyboard_structure Keyboard;

    struct level_structure Level;

    /**
        Flags
        */
    ALLEGRO_MUTEX *MutexDrawCall;
    ALLEGRO_COND *CondDrawCall;
    ALLEGRO_MUTEX *MutexThreadDraw;
    bool ThreadDrawWaiting;
    bool DrawCall;
    ALLEGRO_MUTEX *MutexSpecialMainCall;
    ALLEGRO_COND *CondSpecialMainCall;
    bool SpecialMainCall;
    void (*special_main_call_procedure)(struct GameSharedData*);

    bool CloseNow;

    /**
        */
    bool MouseWorking;
};

void special_call(void (*function_to_call)(struct GameSharedData *), struct GameSharedData*);
void scale_display_buffers(struct GameSharedData *);
void* main_iteration(ALLEGRO_THREAD *, void *);
void* thread_event_queue_procedure(ALLEGRO_THREAD *, void *);
void* load_level(ALLEGRO_THREAD *, void *);

void add_movable_object(struct GameSharedData*, enum movable_object_type, void*);
void add_fixed_object(struct GameSharedData*, enum fixed_object_type, void*);

//Draw

#define DRAW(OBJECT) OBJECT.draw(OBJECT.ObjectData)

void draw_tetragon(struct point *v1, struct point *v2, struct point *v3, struct point *v4, ALLEGRO_COLOR);
void draw_square(void *ObjectData);
void draw_circle(void *ObjectData);
void draw_rectangle(void *ObjectData);

void draw_player(void *ObjectData);

//Constructors

void construct_circle(struct fixed_object_structure *);
void construct_square(struct fixed_object_structure *);
void construct_rectangle(struct fixed_object_structure *);

void construct_player(struct movable_object_structure *);
void construct_particle(struct movable_object_structure *);

//Math
int int_abs(int);
float float_abs(float);
int int_min(int, int);
int sign(float);
float norm(float fi);

float squareEquation(float r0, float fi);
float rectangleEquation(float r0, float fi, float fi0, float fi02, float wsp1, float wsp2);
float rSquare(void *ObjectData, float fi);
float rCircle(void *ObjectData, float fi);
float rRectangle(void *ObjectData, float fi);
float rPlayer(void *ObjectData, float fi);


int rzad(int);
char int_to_char(int);
void int_to_str(int, char*);


#endif
