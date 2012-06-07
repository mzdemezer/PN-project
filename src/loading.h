#ifndef _INCLUDE_LOADING_H
#define _INCLUDE_LOADING_H

#define ERROR_COLOR al_map_rgb(128, 128, 128)
#define DEFAULT_BACKGROUND_COLOR al_map_rgb(0, 0, 80)

#define DEFAULT_FLUID_DENSITY 0.001

void* load_level(ALLEGRO_THREAD *, void *);
void draw_level_background(game_shared_data *);
void load_level_from_file(game_shared_data *);
void draw_loading(game_shared_data *);
void load_and_initialize_level(game_shared_data *Data);

#endif
