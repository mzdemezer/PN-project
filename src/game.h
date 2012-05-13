#ifndef _INCLUDE_GAME_H
#define _INCLUDE_GAME_H

void handle_event_game(struct GameSharedData *);
void draw_game(struct GameSharedData *);
void request_game(struct GameSharedData *);
float VectorAngle(float, float);
void* main_iteration(ALLEGRO_THREAD *, void *);
void* iteration_0(ALLEGRO_THREAD *, void *);
void* iteration_1(ALLEGRO_THREAD *, void *);
void* iteration_2(ALLEGRO_THREAD *, void *);
void terminate_iteration(struct GameSharedData *);

#endif
