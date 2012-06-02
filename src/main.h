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

#define SCREEN_BUFFER_WIDTH 1000
#define SCREEN_BUFFER_HEIGHT 750
#define MAX_FPS 60

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
#define INITIAL_BOUNDRY_FIXED 1024
#define INITIAL_BOUNDRY_PRIMITIVE 8192

#define ERROR_COLOR al_map_rgb(128, 128, 128)
#define DEFAULT_BACKGROUND_COLOR al_map_rgb(0, 0, 80)

/**
    Physics
    */

#define COULOMB 0.1
#define GRAV 600

#define DEFAULT_FLUID_DENSITY 0.001
#define MACH_SPEED 300
#define SPHERE_DRAG_COEFFICENT 0.45

#define MAX_COLLISIONS_PER_TURN 1600
#define WALL_COLLISION_COEFFICIENT 0.8

#define PLAYER_TO_PLAYER_RESTITUTION 1
#define PLAYER_TO_PARTICLE_RESTITUTION 1
#define PARTICLE_TO_PARTICLE_RESTITUTION 1
/**
    Maths
    */

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

enum primitive_object_type{
    potSEGMENT,
    potPOINT,
    potCIRCLE
};

/**
    These structures enable objectivization
    by casting on different objects in
    relation to Type. They also enable
    creating lists of objects that are
    logically connected with each other
    */

struct primitive_object_structure{
    enum primitive_object_type Type;
    void *ObjectData;
};

struct point{
    float x, y;
};

struct segment{
    struct point A, B;
    float ang; // direction of AB vector; to make collisions faster
};

struct circle{
    struct point center;
    float r;
};

struct fixed_object_structure{
    enum fixed_object_type Type;
    void* ObjectData;
    void (*draw)(void*);
    float (*r)(void*, float);
    short int zones[4];
};

struct collision_data{
    float time;
    short int who, with;
    bool with_movable;
};

struct collision_heap{
    int allocated, length;
    struct collision_data *heap;
};

#define RED true
#define BLACK false
typedef struct RB_node{
    short int key;
    bool color;
    struct RB_node *left, *right, *parent;
}RB_node;

typedef struct RB_tree{
    RB_node *root, *nil;
}RB_tree;

typedef struct coll_node{
    struct collision_data key;
    bool color;
    short int counter;
    struct coll_node *left, *right, *parent;
}coll_node;

typedef struct coll_tree{
    coll_node *root, *nil;
}coll_tree;

struct movable_object_structure{
    enum movable_object_type Type;
    void* ObjectData;
    void (*draw)(void*, float dx, float dy);
    float (*r)(void*, float);
    float dx, dy;
    short int zones[4];
    struct collision_data *next_collision,
                           coll_with_fixed;
    coll_tree colls_with_mov;
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

    struct point NewPoint1;
    struct point NewPoint2;
    short int new_zones[4];
};


/**
    Order of first few fields in objects is
    deliberetly the same. This order simplifies
    getting access to them.
    */

#define MAX_ACCELERATE 40
#define MAX_DECELERATE -20
#define THROTTLE 3
#define dANG 0.06981 //4 degrees
#define PLAYER_MASS 600
#define PLAYER_RADIUS 30
struct playerData{
    struct point center;
    float r, vx, vy;
    float ang;
    int engine_state;
    float mass,
          charge,
          r0;
};

struct rectangleData{
    struct point center;
    float r, a, b, ang, fi0, fi02, wsp1, wsp2;
    struct point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
};

struct circleData{
    struct point center;
    float r, ang;
    ALLEGRO_COLOR color;
};

float squareEquation(float f0, float fi);

struct squareData{
    struct point center;
    float r, bok, ang;
    struct point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
};

struct entranceData{
    struct point center;
    float r, a, b, ang, fi0, fi02, wsp1, wsp2;
    struct point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;
};

struct exitData{
    struct point center;
    float r, a, b, ang, fi0, fi02, wsp1, wsp2;
    struct point *v1, *v2, *v3, *v4;
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
    struct point center;
    float r, a, b, ang, fi0, fi02, wsp1, wsp2;
    struct point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;

    int pos;
    float mass;
    enum switch_type swType;
    struct connectedData connected;

    float vx, vy;
};

enum door_type{
    dt1WINGSLIDE
};

struct doorData{
    struct point center;
    float r, a, b, ang, fi0, fi02, wsp1, wsp2;
    struct point *v1, *v2, *v3, *v4;
    ALLEGRO_COLOR color;

    int pos;
    float openingTime, mass;
    enum door_type doorType;

    float vx, vy;
};

struct particleData{
    struct point center;
    float r, mass, charge;
    ALLEGRO_COLOR color;

    float vx, vy;
    float surface_field;
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

struct zone{
    RB_tree movable;
    int number_of_primitives, allocated;
    short int *primitives;
};

#define NumOfThreads 3
#define ACC_2nd_DIM NumOfThreads + 2
struct move_arrays{
    float x, y,
          vx, vy,
          ax[ACC_2nd_DIM],
          ay[ACC_2nd_DIM];
};

#define ZONE_FACTOR 50
#define ZONE_SIZE (SCREEN_BUFFER_HEIGHT / ZONE_FACTOR)
#define INITIAL_PRIMITIVES_PER_ZONE 64
#define INITIAL_OBJECT_COLLISION_QUEUE_SIZE 1024

struct level_structure{
    int LevelNumber,
        number_of_movable_objects,
        number_of_fixed_objects,
        number_of_primitive_objects,
        boundry_movable,
        boundry_fixed,
        boundry_primitive;
    struct primitive_object_structure *PrimitiveObjects;
    struct fixed_object_structure *FixedObjects;
    struct movable_object_structure *MovableObjects;

    struct playerData *Player;

    struct zone zones[ZONE_FACTOR][ZONE_FACTOR];
    struct collision_heap collision_queue;
    coll_tree dirty_tree;

    ALLEGRO_BITMAP *Background;
    ALLEGRO_BITMAP *ScaledBackground;
    char filename[256];
    double last_time, dt;
    struct move_arrays *Acc;
    float dens, wind_vx, wind_vy;
};

struct scale_structure{
    float scale_x, scale_y,
          scale_w, scale_h,
          trans_x, trans_y,
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
    struct iteration_thread_structure IterationThreads[NumOfThreads];
    bool IterationFinished;
    bool MainIterationThreadsIsWaiting;
    bool CloseLevel;

    struct scale_structure scales;
    ALLEGRO_TRANSFORM Transformation;
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

    bool Debug;
    char DeBuffer[1024];
    ALLEGRO_FONT *DeFont;
    float DeCollAngs[4];

    bool CloseNow;

    /**
        */
    bool MouseWorking;
};

struct line{
    float A, B, C;
};

/**
    Functions
    */

void special_call(void (*function_to_call)(struct GameSharedData *), struct GameSharedData*);
void calculate_scales(struct GameSharedData *);
void calculate_transformation(struct GameSharedData *);

void* main_iteration(ALLEGRO_THREAD *, void *);
void* thread_event_queue_procedure(ALLEGRO_THREAD *, void *);
void* load_level(ALLEGRO_THREAD *, void *);

ALLEGRO_COLOR interpolate(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, float frac);
void scale_bitmap(ALLEGRO_BITMAP* source, int width, int height);

//Arrays for objects
void add_primitive_object(struct GameSharedData *, enum primitive_object_type, void* NewObjectData);
void delete_primitive_object(struct primitive_object_structure *);
void clear_primitive_object_list(struct GameSharedData *);
void add_fixed_object(struct GameSharedData *, enum fixed_object_type, void* NewObjectData);
void delete_fixed_object(struct fixed_object_structure *);
void clear_fixed_object_list(struct GameSharedData *);
void add_movable_object(struct GameSharedData *Data, enum movable_object_type NewObjectType, void* NewObjectData);
void delete_movable_object(struct movable_object_structure *);
void clear_movable_object_list(struct GameSharedData *);

//Red-Black Tree for zones
RB_node* get_node(RB_tree *tree, short int key);
RB_node* get_minimum(RB_node *node, RB_node *nil);
RB_node* get_successor(RB_node *node, RB_node *nil);
void insert_node(RB_tree *tree, short int key);
void delete_node(RB_tree *tree, short int key);
void RB_delete_fixup(RB_tree *tree, RB_node *node);
void clear_nodes(RB_node *node, RB_node *nil);
void clear_tree(RB_tree *tree);
inline bool is_left(RB_node *node);
void rotate_left(RB_tree *tree, RB_node *node);
void rotate_right(RB_tree *tree, RB_node *node);

void in_order(RB_node *root, RB_node *nil);
void for_each_higher_check_collision(struct GameSharedData *Data, bool *movable_done, short int who, RB_node *node, RB_node *nil);
void in_order_check_collision(struct GameSharedData *Data, bool *movable_done, short int who, RB_node *node, RB_node *nil);
void in_order_find_new_collision(struct GameSharedData *Data, bool *movable_done, short int who, short int ommit, RB_node *node, RB_node *nil, float time_passed);

//Red-Black Tree for collisions
#define LESS -1
#define MORE 1
#define EQUAL 0
short int coll_comp(struct collision_data *a, struct collision_data *b);
short int coll_rev_comp(struct collision_data *a, struct collision_data *b);
coll_node* coll_get_node(coll_tree *tree, struct collision_data *key);
coll_node* coll_get_minimum(coll_node *node, coll_node *nil);
coll_node* coll_get_successor(coll_node *node, coll_node *nil);
void coll_insert_node(coll_tree *tree, struct collision_data *key);
bool coll_delete_node(coll_tree *tree, struct collision_data *key);
void coll_delete_fixup(coll_tree *tree, coll_node *node);
void coll_clear_nodes(coll_node *node, coll_node *nil);
void coll_clear_tree(coll_tree *tree);
void coll_clear_trash(struct GameSharedData *Data, coll_node *node, coll_node *nil);
inline bool coll_is_left(coll_node *node);
void coll_rotate_left(coll_tree *tree, coll_node *node);
void coll_rotate_right(coll_tree *tree, coll_node *node);
void coll_in_order(coll_node *root, coll_node *nil);

//Heap for collision
void construct_heap(struct collision_heap* heap, int size);
#define heap_left(i) (i << 1)
#define heap_right(i) ((i << 1 ) | 1)
#define heap_parent(i) (i >> 1)
void heapify(struct collision_heap* heap, short int i);
void build_heap(struct collision_heap* heap);
struct collision_data pop_min(struct collision_heap* heap);
void heap_insert(struct collision_heap* heap, struct collision_data *collision);
void clear_heap(struct collision_heap* heap);

//Zones
void get_zone(float x, float y, short int *zone);
void get_zone_for_object(float x, float y, float dx, float dy, float r0, short int *zone);
void add_primitive_to_zone(struct zone* zone, short int key);
void add_segment(struct GameSharedData *, const struct point *A, const struct point *B);
void add_point(struct GameSharedData *Data, struct point *A);
void add_circle(struct GameSharedData *Data, float r, struct point center);
void add_square(struct GameSharedData *Data, struct squareData *square);
void add_rectangle(struct GameSharedData *Data, struct rectangleData *rectangle);
void initialize_zones_with_movable(struct GameSharedData *Data, short int *zones, short int index);
void change_zones_for_movable(struct GameSharedData *Data, short int index, float t);

//Colisions
#define EMPTY_COLLISION_TIME 10
float check_collision_between_two_balls(double x, double y, float dx, float dy, double d);
void move_objects(struct GameSharedData *Data, float t);
struct collision_data get_collision_with_primitive(struct movable_object_structure *who, struct primitive_object_structure *with_what);
struct collision_data get_collision_with_movable(struct movable_object_structure *who, struct movable_object_structure *with_whom);
void get_and_check_mov_coll_if_valid(struct GameSharedData *Data, short int who, short int with, float time_passed);
void collision_min_for_object(struct GameSharedData *Data, short int who);
void find_next_collision(struct GameSharedData *Data, short int index, short int ommit, bool *fixed_done, bool *movable_done, float time_passed);

float vector_product(float x1, float y1, float x2, float y2);
bool vectors_on_two_sides(float vector_pr1, float vector_pr2);
bool do_segments_intersect(const struct point *A1, const struct point *A2,
                           const struct point *B1, const struct point *B2);
bool get_segment_intersection(const struct point *A1, const struct point *A2,
                              const struct point *B1, const struct point *B2,
                              struct point *I);
bool get_outer_zones_of_segment(const struct point *A, const struct point *B, short int *zones);

void get_line_from_points(float x1, float y1, float x2, float y2, struct line *);
void get_line_from_point_and_vector(float x, float y, float vx, float vy, struct line *);
void common_point(const struct line* L1, const struct line* L2, float *x, float *y);
void collide(struct GameSharedData *Data, short int who, short int with, bool with_movable, float dt);
void get_velocities_after_two_balls_collision(float *v1x, float *v1y, float *v2x, float *v2y,
                                              double dx, double dy, float m1, float m2, float restitution);
//Draw

#define DRAW_FIXED(OBJECT) OBJECT.draw(OBJECT.ObjectData)
#define DRAW_MOVABLE(OBJECT) OBJECT.draw(OBJECT.ObjectData, Data->scales.trans_x, Data->scales.trans_y)

void draw_tetragon(struct point *v1, struct point *v2, struct point *v3, struct point *v4, ALLEGRO_COLOR);
void draw_square(void *ObjectData);
void draw_circle(void *ObjectData);
void draw_rectangle(void *ObjectData);
void draw_all_fixed_objects(struct GameSharedData*);

void draw_player(void *ObjectData, float dx, float dy);
void draw_particle(void *ObjectData, float dx, float dy);
void draw_door(void *ObjectData, float dx, float dy);
void draw_switch(void *ObjectData, float dx, float dy);

//Constructors

void construct_circle(struct fixed_object_structure *);
void construct_square(struct fixed_object_structure *);
void construct_rectangle(struct fixed_object_structure *);

void construct_player(struct movable_object_structure *);
void construct_particle(struct movable_object_structure *);
void construct_door(struct movable_object_structure *);
void construct_switch(struct movable_object_structure *);
void construct_movable(struct GameSharedData *Data, struct movable_object_structure *Object);

//Math
int int_abs(int);
float float_abs(float);
double double_abs(double);
float float_min(float, float);
inline double double_min(double a, double b);
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
