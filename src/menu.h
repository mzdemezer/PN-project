#ifndef _INCLUDE_MENU_H
#define _INCLUDE_MENU_H

void normalize_menu_selection(struct menu_structure *);
void handle_event_menu(const ALLEGRO_EVENT*, struct GameSharedData*);

void new_game_activate(void*);
void exit_activate(void*);

void scale_fonts(struct GameSharedData*);
void resolution_activate(void*);
char* stringify_resolution(const ALLEGRO_DISPLAY_MODE *);
int rzad(int);
char int_to_char(int);
char* int_to_str(int);


void restore_current_settings(struct GameSharedData *);
void change_menu(struct GameSharedData *, struct menu_elem *, int);
void return_menu(struct GameSharedData *);
void clear_menu();
void draw_menu(struct GameSharedData* Data);


#endif
