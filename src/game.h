#ifndef _INCLUDE_GAME_H
#define _INCLUDE_GAME_H

#define MAX_DT 0.1
#define MAX_DISPLACEMENT 10
/**
    Physics
    */

#define COULOMB 0.1
#define GRAV 60

#define SPHERE_DRAG_COEFFICENT 0.45

#define COST_SHIELD 5
#define COST_ELECTROSTATIC 4
#define COST_GRAVITY 4
#define ENERGY_REPLENISHMENT_SPEED 2

void* main_iteration(ALLEGRO_THREAD *, void *);
void* compute_gravity(ALLEGRO_THREAD *, void *);
void* compute_electrostatics(ALLEGRO_THREAD *, void *);
void* compute_collisions(ALLEGRO_THREAD *, void *);
bool initialize_iteration_threads(game_shared_data*);
void terminate_iteration(game_shared_data *);

#endif
