#ifndef _INCLUDE_LOADING_H
#define _INCLUDE_LOADING_H

void read_line(char *, ALLEGRO_FILE *);
void read_color(char *, ALLEGRO_FILE *, ALLEGRO_COLOR *, int, const char *, int);
void* load_level(ALLEGRO_THREAD *, void *);
void load_level_from_file(struct GameSharedData *);
void draw_loading(struct GameSharedData *);
void handle_event_loading(struct GameSharedData *);
void request_loading(struct GameSharedData *);

#endif
