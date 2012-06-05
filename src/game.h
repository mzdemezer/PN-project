#ifndef _INCLUDE_GAME_H
#define _INCLUDE_GAME_H

void handle_event_game(struct GameSharedData *);
void draw_game(struct GameSharedData *);
void draw_stat_bar(struct GameSharedData *);
void request_game(struct GameSharedData *);
void request_pause(struct GameSharedData *);
float VectorAngle(float, float);
bool get_drag_data(struct movable_object_structure *Obj, double *vx, double *vy, double *Cx, double *S);
double coefficient_multiplier(double v);
void* main_iteration(ALLEGRO_THREAD *, void *);
void* iteration_0(ALLEGRO_THREAD *, void *);
void* iteration_1(ALLEGRO_THREAD *, void *);
void* iteration_2(ALLEGRO_THREAD *, void *);
void terminate_iteration(struct GameSharedData *);

#endif
