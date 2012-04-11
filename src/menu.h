#ifndef _INCLUDE_MENU_H
#define _INCLUDE_MENU_H

void normalize_menu_selection(struct menu_structure *);
void handle_event_menu(const ALLEGRO_EVENT*, struct GameSharedData*);

void new_game_activate(void*);
void exit_activate(void*);

void scale_fonts(struct GameSharedData*);
void resolution_activate(void*);

void change_menu(struct menu_structure *, struct menu_elem *, int);
void return_menu(struct menu_structure *);
void clear_menu();
void draw_menu(struct GameSharedData* Data);


#endif
