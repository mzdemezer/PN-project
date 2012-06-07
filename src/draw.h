#ifndef _INCLUDE_DRAW_H
#define _INCLUDE_DRAW_H

#include "structures/gameshareddata.h"

/**
    Drawing game states
    */
/**
    A general draw-game-state wrappers
    */
void draw(void (*func)(game_shared_data *), game_shared_data *Data);
void synchronized_draw(void (*func)(game_shared_data *), game_shared_data *Data);

/**
    Draw menu
    */
void draw_menu(game_shared_data* Data);
void draw_pause(game_shared_data *Data);
void clear_menu();
void clear_paused_menu(game_shared_data *Data);
void draw_menu_content(game_shared_data *Data);

/**
    Draw game
    */
void draw_game(game_shared_data *);


/**
    Scalling with linear filtering
    */
void scale_bitmap(ALLEGRO_BITMAP* source, int width, int height);

/**
    Drawing game objects
    */
#define DRAW_FIXED(OBJECT) OBJECT.draw(OBJECT.object_data)
#define DRAW_MOVABLE(OBJECT) OBJECT.draw(OBJECT.object_data, Data->scales.trans_x, Data->scales.trans_y)

void draw_square(void *object_data);
void draw_circle(void *object_data);
void draw_rectangle(void *object_data);
void draw_all_fixed_objects(level_data *level);

void draw_player(void *object_data, double dx, double dy);
void draw_particle(void *object_data, double dx, double dy);
void draw_door(void *object_data, double dx, double dy);
void draw_switch(void *object_data, double dx, double dy);



#endif
