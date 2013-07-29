#ifndef _INCLUDE_EVENTTHREAD_H
#define _INCLUDE_EVENTTHREAD_H

#include <allegro5/allegro.h>
#define TIMER_TICKS_FOR_END_LEVEL 900

void* thread_event_queue_procedure(ALLEGRO_THREAD *thread, void *arg);
void new_game_activate(void *argument);
void exit_activate(void *argument);
void high_scores_activate(void *argument);
void return_to_game_activate(void *argument);
void high_scores_return(game_shared_data *);

#endif
