#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "main.h"
#include "draw.h"
#include "structures.h"
#include "stringprocessing.h"
#include "mathematics.h"

/**
    Private methods
    */
void clear_menu();
void clear_paused_menu(game_shared_data *Data);
void draw_menu_content(game_shared_data *Data);
void clear_stat_bar(game_shared_data *Data);
void draw_time(game_shared_data *Data);
void draw_HP(game_shared_data *Data);
void draw_stat_bar(game_shared_data *);
void highlight_zone(game_shared_data *Data, short int x, short int y, ALLEGRO_COLOR color);
void draw_zones(game_shared_data *Data, movable_object *obj, ALLEGRO_COLOR color);
void draw_grid(game_shared_data *Data);
void draw_arrow(game_shared_data *Data, double cx, double cy, double ang, int size, ALLEGRO_COLOR color);

ALLEGRO_COLOR interpolate(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, double frac);
void draw_tetragon(point *v1, point *v2, point *v3, point *v4, ALLEGRO_COLOR color);

/**
    Code
    */

/**
    The wrapper:
    Clean, draw, flip
    */
void draw(void (*func)(game_shared_data *), game_shared_data *Data){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    func(Data);
    al_flip_display();
}

/**
    If some game-state function NEEDS to be
    called from outside draw thread, then this
    wrapper is the right choice.
    */
void synchronized_draw(void (*func)(game_shared_data *), game_shared_data *Data){
    void sp_func(game_shared_data *arg){
        draw(func, arg);
    };
    special_call(sp_func, Data);
}

/**
    Draw menu
    */
void draw_menu(game_shared_data* Data){
    clear_menu();
    draw_menu_content(Data);
}

void draw_pause(game_shared_data *Data){
    clear_paused_menu(Data);
    draw_menu_content(Data);
}

void clear_menu(){
    ;
}

void clear_paused_menu(game_shared_data *Data){
    ALLEGRO_TRANSFORM tempT;

    al_identity_transform(&tempT);
    al_use_transform(&tempT);
        al_draw_bitmap(Data->level.background, Data->scales.scale_x, Data->scales.scale_y, 0);
        al_draw_filled_rectangle(0, 0, Data->display_data.width, Data->display_data.height,
                                 al_map_rgba(0, 0, 0, 170));
    al_use_transform(&Data->transformation);
}

void draw_menu_content(game_shared_data *Data){
    int i, num_of_elems, flags;
    double align_x;
    activation_argument arg;
    ALLEGRO_FONT *regular_font, *select_font;
    ALLEGRO_TRANSFORM tempT;
    al_identity_transform(&tempT);
    al_use_transform(&tempT);

    //Normal menu
    if((int)Data->menu.current_menu[0].type > 0){
        regular_font = Data->font_menu_regular;
        select_font = Data->font_menu_selected;
        flags = ALLEGRO_ALIGN_CENTRE;
        align_x = Data->display_data.width / 2;
    }
    else{//Configuration menu
        regular_font = Data->font_menu_config;
        select_font = Data->font_menu_config_selected;
        flags = ALLEGRO_ALIGN_LEFT;
        align_x = Data->display_data.width / 10 + Data->scales.scale_x;
    }

    num_of_elems = int_abs(Data->menu.current_menu[0].type);
    al_draw_text(Data->font_menu_big, al_map_rgb(255, 255, 255), Data->display_data.width/2,
                 Data->display_data.height/10 + Data->scales.scale_y, ALLEGRO_ALIGN_CENTRE, Data->menu.current_menu[0].name);
    for(i = 1; i < Data->menu.current_elem; ++i){
        al_draw_text(regular_font, al_map_rgb(255, 255, 255), align_x,
                     (i + 1.5) * (Data->font_menu_big->height * 1.11) + Data->scales.scale_y,
                     flags, Data->menu.current_menu[i].name);
        if(Data->menu.current_menu[i].type == metUPDOWN){
            arg.call_type = meatDRAW + i;
            arg.Data = Data;
            Data->menu.current_menu[i].activate((void*)&arg);
        }
    }
    al_draw_text(select_font, al_map_rgb(255, 255, 0), align_x,
                 (i + 1.5) * (Data->font_menu_big->height * 1.11) + Data->scales.scale_y,
                 flags, Data->menu.current_menu[i].name);
    if(Data->menu.current_menu[i].type == metUPDOWN){
            arg.call_type = meatDRAW + i;
            arg.Data = Data;
            Data->menu.current_menu[i].activate((void*)&arg);
        }
    for(++i; i <= num_of_elems; ++i){
        al_draw_text(regular_font, al_map_rgb(255, 255, 255), align_x,
                     (i + 1.5) * (Data->font_menu_big->height * 1.11) + Data->scales.scale_y,
                     flags, Data->menu.current_menu[i].name);
        if(Data->menu.current_menu[i].type == metUPDOWN){
            arg.call_type = meatDRAW + i;
            arg.Data = Data;
            Data->menu.current_menu[i].activate((void*)&arg);
        }
    }

    al_use_transform(&Data->transformation);
}

/**
    Draw high scores
    */
void draw_high_scores(game_shared_data *Data){
    int i;
    char buf[20];
    ALLEGRO_TRANSFORM tempT;
    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255),
                  yellow = al_map_rgb(255, 255, 0),
                  *color;

    al_draw_text(Data->font_menu_big, white, Data->display_data.width / 2,
                 Data->display_data.height * 0.07 + Data->scales.scale_y, ALLEGRO_ALIGN_CENTRE, "HIGH SCORES");
    al_draw_text(Data->font_menu_selected, yellow, Data->display_data.width / 2,
                 Data->display_data.height * 0.8 + Data->scales.scale_y, ALLEGRO_ALIGN_CENTRE, "RETURN");
    strcpy(buf, "name");
    for(i = 0; i < MAX_HIGH_SCORES; ++i){
        if(i == Data->name_length){
            color = &yellow;
        }else{
            color = &white;
        }
        int_to_str(i + 1, buf + 4);
        al_draw_text(Data->font_menu_config, *color, Data->scales.scale_w * 0.1 + Data->scales.scale_x,
                     Data->display_data.height * (0.06 * (i + 3.5)) + Data->scales.scale_y, ALLEGRO_ALIGN_LEFT, buf + 4);
        al_draw_text(Data->font_menu_config, *color, Data->scales.scale_w * 0.15 + Data->scales.scale_x,
                     Data->display_data.height * (0.06 * (i + 3.5)) + Data->scales.scale_y, ALLEGRO_ALIGN_LEFT,
                     al_get_config_value(Data->config, "High_scores", buf));
        al_draw_text(Data->font_menu_config, *color, Data->scales.scale_w * 0.9 + Data->scales.scale_x,
                     Data->display_data.height * (0.06 * (i + 3.5)) + Data->scales.scale_y, ALLEGRO_ALIGN_RIGHT,
                     al_get_config_value(Data->config, "High_scores", buf + 4));
    }

    al_use_transform(&Data->transformation);
}

/**
    Draw score
    */
void draw_score(game_shared_data *Data){
    ALLEGRO_TRANSFORM tempT;
    al_identity_transform(&tempT);
    al_use_transform(&tempT);

    al_draw_text(Data->font_menu_big,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width / 2,
                 Data->display_data.height * 0.3,
                 ALLEGRO_ALIGN_CENTRE,
                 "SCORE");

    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width / 2,
                 Data->display_data.height * 0.6,
                 ALLEGRO_ALIGN_CENTRE,
                 Data->buffer);

    al_use_transform(&Data->transformation);
}

/**
    Draw: game over, victory,
    score for level, etc.
    */
void draw_end_level(game_shared_data *Data){
    char buffer[20];
    if(Data->level.victory){
        strcpy(buffer, "VICTORY");
    }else{
        strcpy(buffer, "GAME OVER");
    }
    ALLEGRO_TRANSFORM tempT;
    al_identity_transform(&tempT);
    al_use_transform(&tempT);


    if(Data->loading_state < 32){
        al_draw_bitmap(Data->level.background, Data->scales.scale_x, Data->scales.scale_y, 0);
        al_draw_filled_rectangle(0, 0, Data->display_data.width, Data->display_data.height,
                             al_map_rgba(0, 0, 0, Data->loading_state * 8));
    }


    al_draw_text(Data->font_menu_big,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width / 2,
                 Data->display_data.height * 0.20,
                 ALLEGRO_ALIGN_CENTRE,
                 buffer);


    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.1,
                 Data->display_data.height * 0.45,
                 ALLEGRO_ALIGN_LEFT,
                 "Top level");
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.1,
                 Data->display_data.height * 0.55,
                 ALLEGRO_ALIGN_LEFT,
                 "Time");
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.1,
                 Data->display_data.height * 0.65,
                 ALLEGRO_ALIGN_LEFT,
                 "Score");
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.1,
                 Data->display_data.height * 0.75,
                 ALLEGRO_ALIGN_LEFT,
                 "Total score");

    int_to_str(Data->last_score.level_number - 1, buffer);
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.9,
                 Data->display_data.height * 0.45,
                 ALLEGRO_ALIGN_RIGHT,
                 buffer);
    stringify_time(buffer, Data->level.sum_time);
    buffer[8] = ',';
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.9,
                 Data->display_data.height * 0.55,
                 ALLEGRO_ALIGN_RIGHT,
                 buffer);
    int_to_str(Data->last_score.score, buffer);
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.9,
                 Data->display_data.height * 0.65,
                 ALLEGRO_ALIGN_RIGHT,
                 buffer);
    int_to_str(Data->last_score.total_score, buffer);
    al_draw_text(Data->font_menu_config,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width  * 0.9,
                 Data->display_data.height * 0.75,
                 ALLEGRO_ALIGN_RIGHT,
                 buffer);

    al_use_transform(&Data->transformation);
}

/**
    Draw loading screen
    */
void draw_loading(game_shared_data *Data){
    ALLEGRO_TRANSFORM tempT;
    char buffer[14];
    strcpy(buffer, "...LOADING...");
    int i;
    for(i = 0; i < Data->loading_state % 4; ++i){
        buffer[i] = ' ';
        buffer[12 - i] = ' ';
    }
    al_identity_transform(&tempT);
    al_use_transform(&tempT);

    al_draw_text(Data->font_menu_big,
                 al_map_rgb(255, 255, 255),
                 Data->display_data.width / 2,
                 Data->display_data.height / 2,
                 ALLEGRO_ALIGN_CENTRE,
                 buffer);

    al_use_transform(&Data->transformation);
}

/**
    Draw game
    */
void draw_game(game_shared_data *Data){
    int i, j, k;
    ALLEGRO_TRANSFORM tempT;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    al_draw_bitmap(Data->level.background, Data->scales.scale_x, Data->scales.scale_y, 0);
    al_use_transform(&Data->transformation);

    if(Data->debug){
        for(i = 0; i < ZONE_FACTOR; ++i){
            for(j = 0; j < ZONE_FACTOR; ++j){
                for(k = 0; k < Data->level.zones[i][j].number_of_primitives; ++k){
                    highlight_zone(Data, i, j, al_map_rgba(30, 3, 0, 0.01));
                }
            }
        }
        for(i = 0; i < Data->level.number_of_movable_objects; ++i){
            draw_zones(Data, &Data->level.movable_objects[i], al_map_rgba(150, 90, 30, 0.01));
        }
        draw_grid(Data);
    }

    for(i = 0; i < Data->level.number_of_movable_objects; ++i){
       DRAW_MOVABLE(Data->level.movable_objects[i]);
    }


    draw_stat_bar(Data);
}

void highlight_zone(game_shared_data *Data, short int x, short int y, ALLEGRO_COLOR color){
    #define OFFSET 0.5
    al_draw_filled_rectangle(x * ZONE_SIZE + OFFSET + Data->scales.trans_x, y * ZONE_SIZE + OFFSET + Data->scales.trans_y,
                        (x + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_x, (y + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_y,
                        color);
    #undef OFFSET
}

void draw_zones(game_shared_data *Data, movable_object *obj, ALLEGRO_COLOR color){
    #define OFFSET 0.5
    al_draw_filled_rectangle(obj->zones[0] * ZONE_SIZE + OFFSET + Data->scales.trans_x, obj->zones[1] * ZONE_SIZE + OFFSET + Data->scales.trans_y,
                        (obj->zones[2] + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_x, (obj->zones[3] + 1) * ZONE_SIZE - OFFSET + Data->scales.trans_y,
                        color);
    #undef OFFSET
}

void draw_grid(game_shared_data *Data){
    #define OFFSET 0.5
    short int i;
    for(i = 1; i < ZONE_FACTOR; ++i){
        al_draw_line(OFFSET + Data->scales.trans_x + i * ZONE_SIZE, OFFSET + Data->scales.trans_y,
                     OFFSET + Data->scales.trans_x + i * ZONE_SIZE, OFFSET + Data->scales.trans_y + SCREEN_BUFFER_HEIGHT,
                     al_map_rgba(0, 0, 0, 70), 1);
        al_draw_line(OFFSET + Data->scales.trans_x, OFFSET + Data->scales.trans_y + i * ZONE_SIZE,
                     OFFSET + Data->scales.trans_x + SCREEN_BUFFER_WIDTH, OFFSET + Data->scales.trans_y + i * ZONE_SIZE,
                     al_map_rgba(0, 0, 0, 70), 1);
    }
    #undef OFFSET
}

void draw_arrow(game_shared_data *Data, double cx, double cy, double ang, int size, ALLEGRO_COLOR color){
    double fx = cx + size * 0.5 * cos(ang),
          fy = cy + size * 0.5 * sin(ang);

    size *= 0.5;
    al_draw_line(fx, fy,
                 cx - size * cos(ang), cy - size * sin(ang),
                 color, 1.5);
    size *= 0.7;
    al_draw_line(fx, fy,
                 cx + size * cos(ang + dANG), cy + size * sin(ang + dANG),
                 color, 1.5);
    al_draw_line(fx, fy,
                 cx + size * cos(ang - dANG), cy + size * sin(ang - dANG),
                 color, 1.5);
}


void clear_stat_bar(game_shared_data *Data){
    al_draw_filled_rectangle(SCREEN_BUFFER_HEIGHT + Data->scales.trans_x, Data->scales.trans_y,
                             SCREEN_BUFFER_WIDTH + Data->scales.trans_x, SCREEN_BUFFER_HEIGHT + Data->scales.trans_y,
                             al_map_rgb(45, 0, 0));
}

void draw_time(game_shared_data *Data){
    char buffer[11];
    double actual_time = al_get_time() - Data->level.start_time + Data->level.sum_time;
    ALLEGRO_TRANSFORM tempT;

    stringify_time(buffer, actual_time);
    al_identity_transform(&tempT);
    al_use_transform(&tempT);
    al_draw_text(Data->font_time, al_map_rgb(255, 255, 255),
                 Data->scales.scale_w * 0.99 + Data->scales.scale_x,
                 Data->scales.scale_h / 25 + Data->scales.scale_y,
                 ALLEGRO_ALIGN_RIGHT, buffer);
    al_use_transform(&Data->transformation);
}

void draw_HP(game_shared_data *Data){
    al_draw_filled_rectangle(SCREEN_BUFFER_HEIGHT + Data->scales.trans_x + 10, Data->scales.trans_y + 80,
                             SCREEN_BUFFER_WIDTH + Data->scales.trans_x - 10,  Data->scales.trans_y + 90,
                             al_map_rgba(0, 160, 0, 0.3));
    float pos = Data->level.player->HP / PLAYER_HP;
    if(pos < 0){
        pos = 0;
    }
    pos *= SCREEN_BUFFER_WIDTH - SCREEN_BUFFER_HEIGHT - 20;
    al_draw_filled_rectangle(SCREEN_BUFFER_HEIGHT + Data->scales.trans_x + 10,       Data->scales.trans_y + 80,
                             SCREEN_BUFFER_HEIGHT + Data->scales.trans_x + 10 + pos, Data->scales.trans_y + 90,
                             al_map_rgb(0, 255, 0));
}

void draw_stat_bar(game_shared_data *Data){
    clear_stat_bar(Data);
    draw_time(Data);
    draw_HP(Data);
}

/**
    Scalling with linear filtering
    */
ALLEGRO_COLOR interpolate(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, double frac){
    return al_map_rgba_f(c1.r + frac * (c2.r - c1.r),
                         c1.g + frac * (c2.g - c1.g),
                         c1.b + frac * (c2.b - c1.b),
                         c1.a + frac * (c2.a - c1.a));
}

void scale_bitmap(ALLEGRO_BITMAP* source, int width, int height) {
    double source_x = al_get_bitmap_width(source),
           source_y = al_get_bitmap_height(source);
	if (((int)source_x == width) && ((int)source_y == height)) {
		al_draw_bitmap(source, 0, 0, 0);
		return;
	}
	int x, y;
	al_lock_bitmap(al_get_target_bitmap(), ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	al_lock_bitmap(source, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	/* linear filtering code written by SiegeLord */

    double pixy, pixy_f,
           pixx, pixx_f;
    ALLEGRO_COLOR a, b, c, d,
                  ab, cd, result;
    width -= 1;
    height -= 1;
    source_x = (source_x - 1) / width;
    source_y = (source_y - 1) / height;

	for (y = 0; y <= height; ++y) {
		pixy = y * source_y;
		pixy_f = floor(pixy);
		for (x = 0; x <= width; ++x) {
			pixx = x * source_x;
			pixx_f = floor(pixx);

			a = al_get_pixel(source, pixx_f, pixy_f);
			b = al_get_pixel(source, pixx_f + 1, pixy_f);
			c = al_get_pixel(source, pixx_f, pixy_f + 1);
			d = al_get_pixel(source, pixx_f + 1, pixy_f + 1);

			ab = interpolate(a, b, pixx - pixx_f);
			cd = interpolate(c, d, pixx - pixx_f);
			result = interpolate(ab, cd, pixy - pixy_f);

			al_put_pixel(x, y, result);
		}
	}
	al_unlock_bitmap(al_get_target_bitmap());
	al_unlock_bitmap(source);
}

/**
    Print screen
    */
void draw_content_to_background(void (*draw_func)(game_shared_data *), game_shared_data *Data){
    ALLEGRO_TRANSFORM tempT;
    ALLEGRO_BITMAP *tempBitmap = al_get_backbuffer(Data->display);
    al_set_target_bitmap(tempBitmap);
    al_use_transform(&Data->transformation);
    draw_func(Data);

    al_set_target_bitmap(Data->level.background);
    al_identity_transform(&tempT);
    al_use_transform(&tempT);
        al_draw_bitmap_region(tempBitmap, Data->scales.scale_x, Data->scales.scale_y,
                              Data->scales.scale_h + Data->scales.scale_x,
                              Data->scales.scale_h + Data->scales.scale_y, 0, 0, 0);
    al_set_target_bitmap(tempBitmap);
    al_use_transform(&Data->transformation);
}

/**
    Load background, scale it, draw static objects
    */
void draw_level_background(game_shared_data *Data){
    ALLEGRO_BITMAP *tempBitmap;
    ALLEGRO_TRANSFORM tempT;

    /**
        Preparing bitmaps and loading the file
        This is the place to destroy bitmaps - swap them
        quickly for new ones when DrawThread is
        busy... helping to do this :D
        */
    al_destroy_bitmap(Data->level.background);
    Data->level.background = al_create_bitmap(Data->scales.scale_h, Data->scales.scale_h);
    if(strcmp(Data->level.filename + 12, "0") == 0){
        tempBitmap = NULL;
    }else{
        tempBitmap = al_load_bitmap(Data->level.filename);
    }

    /**
        Scaling and drawing
        */
    al_lock_mutex(Data->mutex_draw);
            Data->loading_state = 11;
            draw(draw_loading, Data);
        al_identity_transform(&tempT);
        al_use_transform(&tempT);
            al_set_target_bitmap(Data->level.background);
            if(tempBitmap){
                scale_bitmap(tempBitmap, Data->scales.scale_h, Data->scales.scale_h);
                al_destroy_bitmap(tempBitmap);
            }else{
                al_clear_to_color(DEFAULT_BACKGROUND_COLOR);
            }

        al_use_transform(&Data->transformation);
            al_set_target_backbuffer(Data->display);
            Data->loading_state = 12;
            draw(draw_loading, Data);
        al_use_transform(&tempT);

            tempBitmap = al_get_backbuffer(Data->display);
            al_set_target_bitmap(tempBitmap);
            al_draw_bitmap(Data->level.background, 0, 0, 0);

        al_use_transform(&Data->transformation);
            draw_all_fixed_objects(&Data->level);


            al_set_target_bitmap(Data->level.background);
        al_use_transform(&tempT);
            al_draw_bitmap_region(tempBitmap, 0, 0, Data->scales.scale_h, Data->scales.scale_h, 0, 0, 0);

            al_set_target_bitmap(tempBitmap);
        al_use_transform(&Data->transformation);
            Data->loading_state = 13;
            draw(draw_loading, Data);
    al_unlock_mutex(Data->mutex_draw);
}

/**
    Draw functions for game objects
    */

void draw_tetragon(point *v1, point *v2, point *v3, point *v4, ALLEGRO_COLOR color){
    al_draw_filled_triangle(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y, color);
    al_draw_filled_triangle(v1->x, v1->y, v4->x, v4->y, v3->x, v3->y, color);
}

void draw_square(void *object_data){
    #define Data ((fixed_square*)object_data)
    draw_tetragon(Data->v1, Data->v2, Data->v3, Data->v4, Data->color);
    #undef Data
}

void draw_circle(void *object_data){
    #define Data ((fixed_circle*)object_data)
    al_draw_filled_circle(Data->center.x, Data->center.y, Data->r, Data->color);
    #undef Data
}

void draw_rectangle(void *object_data){
    #define Data ((fixed_rectangle*)object_data)
    draw_tetragon(Data->v1, Data->v2, Data->v3, Data->v4, Data->color);
    #undef Data
}

void draw_all_fixed_objects(level_data *level){
    int i;
    for(i = 0; i < level->number_of_fixed_objects; ++i){
        DRAW_FIXED(level->fixed_objects[i]);
    }
}

void draw_player(void *object_data, double dx, double dy){
    #define Data ((movable_player*)object_data)
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, PLAYER_RADIUS, al_map_rgb(255, 255, 255));
    al_draw_filled_circle(Data->center.x + dx + PLAYER_RADIUS * 0.5 * cos(Data->ang),
                          Data->center.y + dy + PLAYER_RADIUS * 0.5 * sin(Data->ang),
                          PLAYER_RADIUS * 0.2, al_map_rgb(0, 0, 0));
    #undef Data
}

void draw_particle(void *object_data, double dx, double dy){
    #define Data ((movable_particle*)object_data)
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, Data->r, al_map_rgb(255, 0, 255));
    #undef Data
}

void draw_door(void *object_data, double dx, double dy){
    #define Data ((movable_door*)object_data)
    point dv1 = *Data->v1,
                 dv2 = *Data->v2,
                 dv3 = *Data->v3,
                 dv4 = *Data->v4;
    dv1.x += dx;
    dv1.y += dy;
    dv2.x += dx;
    dv2.y += dy;
    dv3.x += dx;
    dv3.y += dy;
    dv4.x += dx;
    dv4.y += dy;
    draw_tetragon(&dv1, &dv2, &dv3, &dv4, Data->color);
    #undef Data
}

void draw_switch(void *object_data, double dx, double dy){
    #define Data ((movable_switch*)object_data)
    point dv1 = *Data->v1,
                 dv2 = *Data->v2,
                 dv3 = *Data->v3,
                 dv4 = *Data->v4;
    dv1.x += dx;
    dv1.y += dy;
    dv2.x += dx;
    dv2.y += dy;
    dv3.x += dx;
    dv3.y += dy;
    dv4.x += dx;
    dv4.y += dy;
    draw_tetragon(&dv1, &dv2, &dv3, &dv4, Data->color);
    #undef Data
}
