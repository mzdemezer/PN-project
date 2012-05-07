#ifndef _INCLUDE_LOADING_H
#define _INCLUDE_LOADING_H

void* load_level(ALLEGRO_THREAD *, void *);
void load_level_from_file(struct GameSharedData *);
void draw_loading(struct GameSharedData *);
void handle_event_loading(struct GameSharedData *);
void request_loading(struct GameSharedData *);

#endif
