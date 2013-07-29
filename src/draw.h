#ifndef _INCLUDE_DRAW_H
#define _INCLUDE_DRAW_H

#include "structures/gameshareddata.h"

#define DEFAULT_BACKGROUND_COLOR al_map_rgb(0, 0, 80)

/**
    Drawing game states
    */
/**
    A general draw-game-state wrappers
    */
void draw(void (*func)(game_shared_data *), game_shared_data *Data);
void synchronized_draw(void (*func)(game_shared_data *), game_shared_data *Data);

/**
    Draw functions in varius game states
    */
void draw_menu(game_shared_data* Data);
void draw_pause(game_shared_data *Data);
void draw_high_scores(game_shared_data *Data);
void draw_score(game_shared_data *Data);
void draw_end_level(game_shared_data *Data);
void draw_loading(game_shared_data *Data);
void draw_game(game_shared_data *Data);

void draw_content_to_background(void (*draw_func)(game_shared_data *), game_shared_data *Data);

void draw_level_background(game_shared_data *Data);
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

void draw_shield(long double x, long double y, long double radius, int r, int g, int b);
void draw_player(void *object_data, long double dx, long double dy);
void draw_particle(void *object_data, long double dx, long double dy);
void draw_door(void *object_data, long double dx, long double dy);
void draw_switch(void *object_data, long double dx, long double dy);

void draw_ship(void *object_data, game_shared_data *Data);


#endif
