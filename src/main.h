/**
    This file includes code that is used by many other files
*/

#ifndef _INCLUDE_MAIN_H
#define _INCLUDE_MAIN_H

//#define TESTS

/**
    Display
    */
#define MAX_FPS 60

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "structures.h"
#include "structures/gameshareddata.h"
#include "draw.h"
#include "mathematics.h"
#include "stringprocessing.h"
#include "collisions.h"
#include "eventthread.h"

/**
    Functions
    */

void special_call(void (*function_to_call)(game_shared_data *), game_shared_data*);
bool initialize_all_threads(game_shared_data *);
void initialize_loading_thread(game_shared_data *, void* (*)(ALLEGRO_THREAD *, void *));
void start_game(game_shared_data *Data);

#endif
