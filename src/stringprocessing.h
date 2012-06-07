#ifndef _INCLUDE_STRINGPROCESSING_H
#define _INCLUDE_STRINGPROCESSING_H

#include <allegro5/allegro.h>

char int_to_char(int);
void int_to_str(int, char*);
void stringify_time(char *buffer, double time);
void stringify_resolution(const ALLEGRO_DISPLAY_MODE *, char*);

#endif
