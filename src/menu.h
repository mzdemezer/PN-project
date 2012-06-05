#ifndef _INCLUDE_MENU_H
#define _INCLUDE_MENU_H

void normalize_menu_selection(struct menu_structure *);
void handle_event_menu(struct GameSharedData*);

void new_game_activate(void *argument);
void exit_activate(void *argument);
void return_to_game_activate(void *argument);

void scale_fonts(struct GameSharedData*);
void rescale_bitmaps(struct GameSharedData *);
void change_resolution(struct GameSharedData *);
void resolution_activate(void*);
void stringify_resolution(const ALLEGRO_DISPLAY_MODE *, char*);

void restore_current_settings(struct GameSharedData *);
void change_menu(struct GameSharedData *, struct menu_elem *, int);
void return_menu(struct GameSharedData *);
void clear_menu();
void draw_menu_content(struct GameSharedData *Data);
void draw_menu(struct GameSharedData* Data);
void clear_paused_menu(struct GameSharedData *Data);
void draw_pause(struct GameSharedData *Data);

#endif
