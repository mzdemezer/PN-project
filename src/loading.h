#ifndef _INCLUDE_LOADING_H
#define _INCLUDE_LOADING_H

#define ERROR_COLOR al_map_rgb(128, 128, 128)

void* load_level(ALLEGRO_THREAD *, void *);
bool load_level_from_file(game_shared_data *);
void draw_loading(game_shared_data *);
bool load_and_initialize_level(game_shared_data *Data);

#endif
