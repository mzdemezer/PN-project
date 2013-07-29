#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stringprocessing.h"
#include "mathematics.h"

char int_to_char(int digit){
    return (char)(digit | 0x30);
}

void int_to_str(int a, char *target){
    bool sign = a < 0;
    int length = rzad(a) + (int)sign, i, digit;
    target[length] = '\0';
    a = int_abs(a);
    for(i = length - 1; i >= (int)sign; --i){
        digit = a % 10;
        a /= 10;
        target[i] = int_to_char(digit);
    }
    if(sign){
        target[0] = '-';
    }
}

void stringify_time(char *buffer, long double time){
    int help = ((int)time) / 3600;

    time -= help * 3600;
    sprintf(buffer, "%02d:", help % 100);

    help = ((int)time) / 60;
    time -= help * 60;
    sprintf(buffer + 3, "%02d:%04.1Lf", help, time);
    buffer[8] = '`'; //depending on the font
}

void stringify_resolution(const ALLEGRO_DISPLAY_MODE *display_data, char *target){
    int_to_str(display_data->width, target);
    int len = strlen(target);
    int_to_str(display_data->height, target + len + 1);
    target[len] = 'x';
}

void add_char_to_name(char *buffer, char new_char, int *length, int max_length){
    if(*length < max_length){
        buffer[*length] = new_char;
        *length += 1;
        buffer[*length] = '\0';
    }
}

void capitalize(char *a){
    *a &= 0xdf;
}

void to_lower_case(char *a){
    *a |= 0x20;
}
