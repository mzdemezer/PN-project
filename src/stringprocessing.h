#ifndef _INCLUDE_STRINGPROCESSING_H
#define _INCLUDE_STRINGPROCESSING_H

#include <allegro5/allegro.h>

char int_to_char(int);
void int_to_str(int, char*);
void stringify_time(char *buffer, long double time);
void stringify_resolution(const ALLEGRO_DISPLAY_MODE *, char*);
void add_char_to_name(char *buffer, char new_char, int *length, int max_length);
void capitalize(char *a);
void to_lower_case(char *a);

#endif
