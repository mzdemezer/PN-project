#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "menu.h"
#include "game.h"
#include "loading.h"

void* thread_event_queue_procedure(ALLEGRO_THREAD *thread, void *arg){

    /**
        crap
        */
    int sec = 0;

    #define Data ((struct GameSharedData*)arg)
    /**
        Main loop
        */
    while(1){
    al_wait_for_event(Data->MainEventQueue, &Data->LastEvent);
        //printf("event type#%d\n", Data->LastEvent.type);

        if(Data->LastEvent.type == ALLEGRO_EVENT_TIMER){
            if(Data->GameState == gsGAME){
                al_lock_mutex(Data->MutexMainIteration);
                    if(Data->IterationFinished){
                        Data->IterationFinished = false;
                        al_broadcast_cond(Data->CondMainIteration);
                    }
                al_unlock_mutex(Data->MutexMainIteration);
            }else{
                al_lock_mutex(Data->MutexDrawCall);
                    Data->DrawCall = true;
                al_unlock_mutex(Data->MutexDrawCall);
            }

             /**
                crap
                */
            ++sec;
            if(sec == 60){
                al_lock_mutex(Data->MutexFPS);
                    printf("\n\nSecond passed, FPS: %d\n\n", Data->FPS);
                    sec=0;
                    Data->FPS = 0;
                al_unlock_mutex(Data->MutexFPS);
            }
        }
        switch(Data->GameState){
            case gsGAME: handle_event_game(Data); break;
            case gsMENU: handle_event_menu(Data); break;
            case gsPAUSE: break;
            case gsLOADING: handle_event_loading(Data); break;
        }

        if(Data->CloseNow){
            printf("Closing\n");
            Data->CloseLevel = true;
            break;
        }

        al_lock_mutex(Data->MutexChangeState);
            if(Data->RequestChangeState){
                switch(Data->NewState){
                    case gsPAUSE: break;
                    case gsLOADING: request_loading(Data); break;
                    case gsGAME: request_game(Data); break;
                    case gsMENU: break;
                }
            }
        al_unlock_mutex(Data->MutexChangeState);

        al_lock_mutex(Data->MutexDrawCall);
            if(Data->DrawCall){
                Data->DrawCall = false;
                al_lock_mutex(Data->MutexThreadDraw);
                    if(Data->ThreadDrawWaiting){
                        Data->ThreadDrawWaiting = false;
                        al_broadcast_cond(Data->CondDrawCall);
                    }
                al_unlock_mutex(Data->MutexThreadDraw);
            }
        al_unlock_mutex(Data->MutexDrawCall);
    }
    /**
        Cleaning-up threads
        */

    terminate_iteration(Data);

    /**
        Sending shut-down signal to the main thread
        */
    al_lock_mutex(Data->MutexThreadDraw);
        Data->ThreadDrawWaiting = false;
        al_broadcast_cond(Data->CondDrawCall);printf("Event-queue thread: signal sent\n");
    al_unlock_mutex(Data->MutexThreadDraw);
    printf("Event queue thread closed\n");
    return NULL;
    #undef Data
}

/**
    Call for  function_to_call  to be executed
    in main thread and wait for the results
    */

void special_call(void (*function_to_call)(struct GameSharedData*), struct GameSharedData *Data){
    al_lock_mutex(Data->MutexSpecialMainCall);
        al_lock_mutex(Data->MutexThreadDraw);
            printf("Special call made\n");
            Data->special_main_call_procedure = function_to_call;
            Data->SpecialMainCall = true;

            if(Data->ThreadDrawWaiting){
                Data->ThreadDrawWaiting = false;
                al_broadcast_cond(Data->CondDrawCall);
            }
        al_unlock_mutex(Data->MutexThreadDraw);

        while(Data->SpecialMainCall){
            al_wait_cond(Data->CondSpecialMainCall, Data->MutexSpecialMainCall);
        }
    al_unlock_mutex(Data->MutexSpecialMainCall);
}

int int_abs(int a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

float float_abs(float a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

float float_min(float a, float b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

void menu_elem_init(struct menu_elem*Item,
                    enum menu_elem_type NewType,
                    char *NewName,
                    void *NewActivate){
    Item->Type = NewType;
    Item->Name = NewName;
    Item->Activate = NewActivate;
}

int rzad(int a){
    int res = 1, lim = 10;
    a = int_abs(a);
    while(a >= lim){
        res += 1;
        lim *= 10;
    }
    return res;
}

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

/**
    Maths
    */

int sign(float a){
    if(a == 0){
        return 0;
    }else if(a > 0){
        return 1;
    }else{
        return -1;
    }
}

/**
    Ensures that  fi  angle is in [0, dwaPI)
    */

float norm(float fi){
	if(fi > 0){
		while(fi - dwaPI > eps){
			fi -= dwaPI;
		}
	}else
		while(fi < 0){
			fi += dwaPI;
		}
    if(float_abs(fi - dwaPI) < eps){
        fi = 0;
    }
	return fi;
}

float squareEquation(float r0, float fi){
	fi -= PI4;
	float s = sin(fi),
          c = cos(fi);
	s = (s + sign(s * c) * c);
	if(s == 0){
		s = 1;
	}
	return float_abs(r0 / s);
}

/**
    Returns   true   if   the first factor (wsp1)   should be used,
              false  if   the second one   (wsp2).
    Used to say which pair of sides is pointed by angle  fi
    fi0 - angle between two closer verticies and center of the rectangle
    */

bool rectangleWsp(float fi, float fi0){
    fi = norm(fi);
    if( ((fi >= PIpol) && (fi <= PIpol + fi0)) ||
        ((fi >= PI32)  && (fi <= PI32  + fi0)) ){
        return false;
    }else{
        return true;
    }
}

float rectangleEquation(float r0, float fi, float fi0, float fi02, float wsp1, float wsp2){
    fi += fi02;
    float res = sin(fi);

    if(rectangleWsp(fi, fi0)){
        res -= wsp1 * cos(fi);
    }else{
        res -= wsp2 * cos(fi);
    }
    if(res != 0){
        res = float_abs(r0 / res);
    }

    return res;
}

float rSquare(void *ObjectData, float fi){
    #define Data ((struct squareData*)ObjectData)
    return squareEquation(Data->r0, fi - Data->ang);
    #undef Data
}

float rCircle(void *ObjectData, float fi){
    return ((struct circleData*)ObjectData)->r0;
}

float rPlayer(void *ObjectData, float fi){
    return 40;
}

float rRectangle(void *ObjectData, float fi){
    #define Data ((struct rectangleData*)ObjectData)
    return rectangleEquation(Data->r0, fi - Data->ang, Data->fi0, Data->fi02, Data->wsp1, Data->wsp2);
    #undef Data
}

/**
    Draw functions
    */

void draw_tetragon(struct point *v1, struct point *v2, struct point *v3, struct point *v4, ALLEGRO_COLOR color){
    al_draw_filled_triangle(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y, color);
    al_draw_filled_triangle(v1->x, v1->y, v4->x, v4->y, v3->x, v3->y, color);
}

void draw_square(void *ObjectData){
    #define Data ((struct squareData*)ObjectData)
    draw_tetragon(&Data->v1, &Data->v2, &Data->v3, &Data->v4, Data->color);
    #undef Data
}

void draw_circle(void *ObjectData){
    #define Data ((struct circleData*)ObjectData)
    al_draw_filled_circle(Data->center.x, Data->center.y, Data->r0, Data->color);
    #undef Data
}

void draw_rectangle(void *ObjectData){
    #define Data ((struct rectangleData*)ObjectData)
    draw_tetragon(&Data->v1, &Data->v2, &Data->v3, &Data->v4, Data->color);
    #undef Data
}

void draw_all_fixed_objects(struct GameSharedData *Data){
    int i;
    for(i = 0; i < Data->Level.number_of_fixed_objects; ++i){
        DRAW_FIXED(Data->Level.FixedObjects[i]);
    }
}

void draw_player(void *ObjectData, float dx, float dy){
    #define Data ((struct playerData*)ObjectData)
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, 40, al_map_rgb(255, 180, 128));
    al_draw_filled_circle(Data->center.x + dx + 20 * cos(Data->ang), Data->center.y + dy + 20 * sin(Data->ang), 8, al_map_rgb(255, 80, 80));
    #undef Data
}

void draw_particle(void *ObjectData, float dx, float dy){
    #define Data ((struct particleData*)ObjectData)
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, Data->r0, al_map_rgb(255, 0, 255));
    #undef Data
}

void draw_door(void *ObjectData, float dx, float dy){
    #define Data ((struct doorData*)ObjectData)
    struct point dv1 = Data->v1,
                 dv2 = Data->v2,
                 dv3 = Data->v3,
                 dv4 = Data->v4;
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

void draw_switch(void *ObjectData, float dx, float dy){
    #define Data ((struct switchData*)ObjectData)
    struct point dv1 = Data->v1,
                 dv2 = Data->v2,
                 dv3 = Data->v3,
                 dv4 = Data->v4;
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

/**
    Constructors
    */

void construct_circle(struct fixed_object_structure *Object){
    Object->draw = draw_circle;
    Object->r = rCircle;
}

void construct_square(struct fixed_object_structure *Object){
    #define Data ((struct squareData*)Object->ObjectData)
    Object->draw = draw_square;
    Object->r = rSquare;
    Data->r0 = Data->bok * SQRT2 / 2;
    float fi = PI4 + Data->ang;
    Data->v1.x = Data->center.x + Data->r0 * cos(fi);
    Data->v1.y = Data->center.y + Data->r0 * sin(fi);
    fi += PIpol;
    Data->v2.x = Data->center.x + Data->r0 * cos(fi);
    Data->v2.y = Data->center.y + Data->r0 * sin(fi);
    fi += PIpol;
    Data->v3.x = Data->center.x + Data->r0 * cos(fi);
    Data->v3.y = Data->center.y + Data->r0 * sin(fi);
    fi += PIpol;
    Data->v4.x = Data->center.x + Data->r0 * cos(fi);
    Data->v4.y = Data->center.y + Data->r0 * sin(fi);
    #undef Data
}

/**
    This sets the values for a rectangle,
    but due to order of variables it works also for
    entrances, exits, switches and doors
    (which of course need further initialization, btw);
    SO DON'T ANYONE DARE CHANGE THE ORDER!!!
    */

void construct_rectangle(struct fixed_object_structure *Object){
    #define Data ((struct rectangleData*)Object->ObjectData)
    float fi;
    Object->draw = draw_rectangle;
    Object->r = rRectangle;

    if(Data->b > Data->a){
        fi = Data->a;
        Data->a = Data->b;
        Data->b = fi;
        Data->ang = norm(Data->ang + PIpol);
    }

    Data->fi0 = float_abs(2 * atan(Data->b / Data->a));
    Data->fi02 = Data->fi0 * 0.5;
    Data->wsp1 = tan(PIpol + Data->fi02);
    Data->wsp2 = tan(Data->fi02);

    Data->r0 = Data->b / (sin(Data->fi02) * 2);

    fi = PIpol - Data->ang + Data->fi02;
    Data->v1.x = Data->center.x + Data->r0 * cos(fi);
    Data->v1.y = Data->center.y + Data->r0 * sin(fi);
    fi += PI;
    Data->v3.x = Data->center.x + Data->r0 * cos(fi);
    Data->v3.y = Data->center.y + Data->r0 * sin(fi);
    fi = PIpol - Data->ang - Data->fi02;
    Data->v2.x = Data->center.x + Data->r0 * cos(fi);
    Data->v2.y = Data->center.y + Data->r0 * sin(fi);
    fi += PI;
    Data->v4.x = Data->center.x + Data->r0 * cos(fi);
    Data->v4.y = Data->center.y + Data->r0 * sin(fi);
    #undef Data
}

void construct_door(struct movable_object_structure *Object){
    #define Data ((struct doorData*)(Object->ObjectData))
    construct_rectangle((struct fixed_object_structure*)Object);

    Object->draw = draw_door;
    Data->vx = 0;
    Data->vy = 0;
    #undef Data
}

void construct_switch(struct movable_object_structure *Object){
    #define Data ((struct switchData*)(Object->ObjectData))
    construct_rectangle((struct fixed_object_structure*)Object);

    Object->draw = draw_switch;
    Data->vx = 0;
    Data->vy = 0;
    #undef Data
}

void construct_player(struct movable_object_structure *Object){
    #define Data ((struct playerData*)(Object->ObjectData))
    Object->draw = draw_player;
    Object->r = rPlayer;
    Data->vx = 0;
    Data->vy = 0;
    Data->engine_state = 0;
    Data->mass = PLAYER_MASS;
    #undef Data
}

void construct_particle(struct movable_object_structure *Object){
    #define Data ((struct particleData*)(Object->ObjectData))
    Object->draw = draw_particle;
    Object->r = rCircle;
    Data->vx = 0;
    Data->vy = 0;
    #undef Data
}

/**
    Arrays interface
    */

void add_movable_object(struct GameSharedData *Data, enum movable_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.number_of_movable_objects == Data->Level.boundry_movable){
        Data->Level.boundry_movable *= 2;
        Data->Level.MovableObjects = (struct movable_object_structure*)realloc((void*)Data->Level.MovableObjects, sizeof(struct movable_object_structure) * Data->Level.boundry_movable);
    }
    Data->Level.MovableObjects[Data->Level.number_of_movable_objects].Type = NewObjectType;
    Data->Level.MovableObjects[Data->Level.number_of_movable_objects].ObjectData = NewObjectData;
    Data->Level.number_of_movable_objects += 1;
}

void add_fixed_object(struct GameSharedData *Data, enum fixed_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.number_of_fixed_objects == Data->Level.boundry_fixed){
        Data->Level.boundry_fixed *= 2;
        Data->Level.FixedObjects = (struct fixed_object_structure*)realloc((void*)Data->Level.FixedObjects, sizeof(struct fixed_object_structure) * Data->Level.boundry_fixed);
    }
    Data->Level.FixedObjects[Data->Level.number_of_fixed_objects].Type = NewObjectType;
    Data->Level.FixedObjects[Data->Level.number_of_fixed_objects].ObjectData = NewObjectData;
    Data->Level.number_of_fixed_objects += 1;
}

void delete_fixed_object(struct fixed_object_structure *Object){
    switch(Object->Type){
        case fotSQUARE:
            free((struct squareData*)Object->ObjectData);
            break;
        case fotRECTANGLE:
            free((struct rectangleData*)Object->ObjectData);
            break;
        case fotCIRCLE:
            free((struct circleData*)Object->ObjectData);
            break;
        case fotENTRANCE:
            free((struct entranceData*)Object->ObjectData);
            break;
        case fotEXIT:
            free((struct exitData*)Object->ObjectData);
            break;
    }
}

void clear_fixed_object_list(struct GameSharedData *Data){
    int i;
    for(i = 0; i < Data->Level.number_of_fixed_objects; ++i){
        delete_fixed_object(&Data->Level.FixedObjects[i]);
    }
    Data->Level.number_of_fixed_objects = 0;
}

void delete_movable_object(struct movable_object_structure *Object){
    switch(Object->Type){
        case motPLAYER:
            free((struct playerData*)Object->ObjectData);
            break;
        case motPARTICLE:
            free((struct particleData*)Object->ObjectData);
            break;
        case motDOOR:
            free((struct doorData*)Object->ObjectData);
            break;
        case motSWITCH:
            free((struct switchData*)Object->ObjectData);
            break;
    }
}

void clear_movable_object_list(struct GameSharedData *Data){
    int i;
    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
        delete_movable_object(&Data->Level.MovableObjects[i]);
    }
    Data->Level.number_of_movable_objects = 0;
}

void calculate_transformation(struct GameSharedData *Data){
    al_identity_transform(&Data->Transformation);
    al_scale_transform(&Data->Transformation, Data->scales.scale, Data->scales.scale);
    al_use_transform(&Data->Transformation);
}

void calculate_scales(struct GameSharedData *Data){
    Data->scales.scale = float_min(Data->DisplayData.width / (float)SCREEN_BUFFER_WIDTH, Data->DisplayData.height / (float)SCREEN_BUFFER_HEIGHT);

    Data->scales.scale_w = SCREEN_BUFFER_WIDTH * Data->scales.scale;//?
    Data->scales.scale_h = SCREEN_BUFFER_HEIGHT * Data->scales.scale;//?
    Data->scales.scale_x = (Data->DisplayData.width - Data->scales.scale_w) / 2;
    Data->scales.scale_y = (Data->DisplayData.height - Data->scales.scale_h) / 2;
    Data->scales.trans_x = Data->scales.scale_x / Data->scales.scale;
    Data->scales.trans_y = Data->scales.scale_y / Data->scales.scale;
}

ALLEGRO_COLOR interpolate(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, float frac){
    return al_map_rgba_f(c1.r + frac * (c2.r - c1.r),
                         c1.g + frac * (c2.g - c1.g),
                         c1.b + frac * (c2.b - c1.b),
                         c1.a + frac * (c2.a - c1.a));
}

void scale_bitmap(ALLEGRO_BITMAP* source, int width, int height) {
    float source_x = al_get_bitmap_width(source),
          source_y = al_get_bitmap_height(source);
	if (((int)source_x == width) && ((int)source_y == height)) {
		al_draw_bitmap(source, 0, 0, 0);
		return;
	}
	int x, y;
	al_lock_bitmap(al_get_target_bitmap(), ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	al_lock_bitmap(source, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	/* linear filtering code written by SiegeLord */

    float pixy, pixy_f,
          pixx, pixx_f;
    ALLEGRO_COLOR a, b, c, d,
                  ab, cd, result;

    source_x = (source_x - 1) / width;
    source_y = (source_y - 1) / height;

	for (y = 0; y < height; y++) {
		pixy = (float)y * source_y;
		pixy_f = floor(pixy);
		for (x = 0; x < width; x++) {
			pixx = (float)x * source_x;
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

extern void RunAllTests(void);

int main(){

#ifdef TESTS

    printf("Running in test mode\n\n");
    RunAllTests();

#else
    int i;

    if(!al_init()){
        fprintf(stderr, "Problems when initilizing Allegro");
        return -1;
    }

    struct GameSharedData Data;
    Data.Display = NULL;
    Data.GameState = gsMENU;
    Data.CloseNow = false;
    Data.DrawCall = false;

    /**
        Menu initialization
        */
    //this needs to be somehow compressed... macroes?

    struct menu_elem MainMenu[1 + int_abs(MAIN_MENU_SIZE)],
                     OptionsMenu[1 + int_abs(OPTIONS_MENU_SIZE)],
                     HighScoresMenu[1 + int_abs(HIGH_SCORES_MENU_SIZE)],
                     GraphicMenu[1 + int_abs(GRAPHIC_MENU_SIZE)],
                     SoundMenu[1 + int_abs(SOUND_MENU_SIZE)],
                     ControlsMenu[1 + int_abs(CONTROLS_MENU_SIZE)];

    menu_elem_init(&MainMenu[mmeDESCRIPTOR],
                   MAIN_MENU_SIZE,
                   "MAIN MENU",
                   &MainMenu);
    menu_elem_init(&MainMenu[mmeNEWGAME],
                   metACTIVATE,
                   "NEW GAME",
                   (void*) new_game_activate);
    menu_elem_init(&MainMenu[mmeOPTIONS],
                   metSUBMENU,
                   "OPTIONS",
                   (void*) &OptionsMenu);
    menu_elem_init(&MainMenu[mmeHIGHSCORES],
                   metSUBMENU,
                   "HIGH SCORES",
                   (void*) &HighScoresMenu);
    menu_elem_init(&MainMenu[mmeEXIT],
                   metACTIVATE,
                   "EXIT",
                   (void*) exit_activate);

    menu_elem_init(&OptionsMenu[omeDESCRIPTOR],
                   OPTIONS_MENU_SIZE,
                   "OPTIONS",
                   &MainMenu);
    menu_elem_init(&OptionsMenu[omeGRAPHIC],
                   metSUBMENU,
                   "GRAPHIC",
                   &GraphicMenu);
    menu_elem_init(&OptionsMenu[omeSOUND],
                   metSUBMENU,
                   "SOUND",
                   &SoundMenu);
    menu_elem_init(&OptionsMenu[omeCONTROLS],
                   metSUBMENU,
                   "CONTROLS",
                   &ControlsMenu);
    menu_elem_init(&OptionsMenu[omeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);

    menu_elem_init(&GraphicMenu[gmeDESCRIPTOR],
                   GRAPHIC_MENU_SIZE,
                   "GRAPHIC",
                   &OptionsMenu);
    menu_elem_init(&GraphicMenu[gmeRESOLUTION],
                   metUPDOWN,
                   "RESOLUTION",
                   &resolution_activate);
    menu_elem_init(&GraphicMenu[gmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);

    menu_elem_init(&SoundMenu[smeDESCRIPTOR],
                   SOUND_MENU_SIZE,
                   "SOUND",
                   &OptionsMenu);
    menu_elem_init(&SoundMenu[smeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);

    menu_elem_init(&ControlsMenu[cmeDESCRIPTOR],
                   CONTROLS_MENU_SIZE,
                   "CONTROLS",
                   &OptionsMenu);
    menu_elem_init(&ControlsMenu[cmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);


    menu_elem_init(&HighScoresMenu[hsmeDESCRIPTOR],
                   HIGH_SCORES_MENU_SIZE,
                   "HIGH SCORES",
                   &MainMenu);
    menu_elem_init(&HighScoresMenu[hsmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);


    Data.Menu.CurrentMenu = &MainMenu[0];
    Data.Menu.Current = 1;

    /**
        Initializing addons
        */

    al_init_font_addon();
    al_init_ttf_addon();

    if(!al_init_image_addon()){
        fprintf(stderr, "Failed to initialize al_init_image_addon!");
        return -1;
    }

    if(!al_init_primitives_addon()){
        fprintf(stderr, "Failed to initialize al_init_primitives_addon!");
        return -1;
    }

    /**
        Getting resolution
        */

    Data.MaxResolutionIndex = al_get_num_display_modes() - 1;
    Data.ChosenResolution = Data.MaxResolutionIndex;

    al_get_display_mode(Data.ChosenResolution, &Data.DisplayData);
    Data.InMenuDisplayData = Data.DisplayData;
    Data.ChosenInMenu = Data.ChosenResolution;
    calculate_scales(&Data);

    /**
        Creating display
        */

    al_set_new_display_flags(ALLEGRO_FULLSCREEN); //ALLEGRO_WINDOWED
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    Data.Display = al_create_display(Data.DisplayData.width, Data.DisplayData.height);
    if (al_get_display_option(Data.Display, ALLEGRO_SAMPLE_BUFFERS)) {
        printf("With multisampling, level %i\n", al_get_display_option(Data.Display, ALLEGRO_SAMPLES));
    }
    else {
        printf("Without multisampling.\n");
    }

    if(!Data.Display){
        fprintf(stderr, "Problems when creating the display");
        return -1;
    }
    calculate_transformation(&Data);
    /**
        Setting font size accordingly to resolution
        */
    scale_fonts(&Data);

    /**
        Initializing keyboard
        */
    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        printf("Nie możesz grać w tą grę bez klawiatury gościu...");
        return -1;
    }

    /**
        Initializing mouse
        */
    if(!al_install_mouse()) {
        fprintf(stderr, "failed to initialize the mouse!\n");
        Data.MouseWorking = false;
    }
    else{
        Data.MouseWorking = true;
    }
    al_hide_mouse_cursor(Data.Display);
    /**
        Initializing event_queue
        */
    Data.MainEventQueue = NULL;
    Data.MainEventQueue = al_create_event_queue();
    if(!Data.MainEventQueue) {
        fprintf(stderr, "failed to create main_event_queue!\n");
        al_destroy_display(Data.Display);
        return -1;
    }
    Data.DrawTimer = NULL;
    Data.DrawTimer = al_create_timer(1.0 / 60);
    if(!Data.DrawTimer) {
        fprintf(stderr, "failed to create timer!\n");
        al_destroy_display(Data.Display);
        al_destroy_event_queue(Data.MainEventQueue);
        return -1;
    }

    al_register_event_source(Data.MainEventQueue, al_get_timer_event_source(Data.DrawTimer));
    al_register_event_source(Data.MainEventQueue, al_get_keyboard_event_source());
    al_register_event_source(Data.MainEventQueue, al_get_display_event_source(Data.Display));
    al_register_event_source(Data.MainEventQueue, al_get_mouse_event_source());
    /**
        Initializing threads
        */
    Data.ThreadEventQueue = NULL;
    Data.ThreadEventQueue = al_create_thread(thread_event_queue_procedure, (void*)&Data);
    if(!Data.ThreadEventQueue){
        fprintf(stderr, "Failed to initialize thread_event_queue");
        al_destroy_display(Data.Display);
        al_destroy_event_queue(Data.MainEventQueue);
        al_destroy_timer(Data.DrawTimer);
        return -1;
    }
    Data.ThreadMainIteration = NULL;
    Data.ThreadMainIteration = al_create_thread(main_iteration, (void*)&Data);
    if(!Data.ThreadMainIteration){
        fprintf(stderr, "Failed to initialize main_iteration thread");
        al_destroy_display(Data.Display);
        al_destroy_event_queue(Data.MainEventQueue);
        al_destroy_timer(Data.DrawTimer);
        return -1;
    }
    Data.IterationThreads[0].Job = iteration_0;
    Data.IterationThreads[1].Job = iteration_1;
    Data.IterationThreads[2].Job = iteration_2;

    for(i = 0; i < NumOfThreads; ++i){
        Data.IterationThreads[i].Thread = al_create_thread(Data.IterationThreads[i].Job, (void*)&Data);
    }
    /**
        Initializing data
        */

    Data.IterationFinished = true;
    Data.CloseLevel = false;
    Data.SpecialMainCall = false;

    Data.Level.LevelNumber = 0;
    Data.Level.number_of_movable_objects = 0;
    Data.Level.number_of_fixed_objects = 0;
    Data.Level.boundry_movable = INITIAL_BOUNDRY_MOVABLE;
    Data.Level.boundry_fixed = INITIAL_BOUNDRY_FIXED;
    Data.Level.MovableObjects = (struct movable_object_structure*)malloc(sizeof(struct movable_object_structure) * Data.Level.boundry_movable);
    Data.Level.FixedObjects =   (struct fixed_object_structure*)malloc(sizeof(struct fixed_object_structure)   * Data.Level.boundry_fixed);
    Data.Level.Background = NULL;
    Data.Level.ScaledBackground = NULL;

    Data.Keyboard.KeyUp = ALLEGRO_KEY_UP;
    Data.Keyboard.KeyDown = ALLEGRO_KEY_DOWN;
    Data.Keyboard.KeyLeft = ALLEGRO_KEY_LEFT;
    Data.Keyboard.KeyRight = ALLEGRO_KEY_RIGHT;

    Data.RequestChangeState = false;
    Data.MutexChangeState = al_create_mutex();
    Data.DrawMutex = al_create_mutex();
    Data.MutexMainIteration = al_create_mutex();
    Data.MutexIterations = al_create_mutex();
    Data.CondIterations = al_create_cond();
    Data.CondMainIteration = al_create_cond();
    Data.MutexDrawCall = al_create_mutex();
    Data.CondDrawCall = al_create_cond();
    Data.MutexThreadDraw = al_create_mutex();
    Data.Keyboard.MutexKeyboard = al_create_mutex();
    Data.MutexSpecialMainCall = al_create_mutex();
    Data.CondSpecialMainCall = al_create_cond();
    Data.MutexFPS = al_create_mutex();
    /**
        First draw
        */

    Data.DrawFunction = draw_menu;
    draw_menu(&Data);
    al_flip_display();
    Data.FPS = 0;

    /**
        Draw thread
        */
    printf("Starting The Game\n\n\n");
    al_start_thread(Data.ThreadEventQueue);
    al_start_timer(Data.DrawTimer);
    while(!Data.CloseNow){
        al_lock_mutex(Data.MutexSpecialMainCall);
        al_lock_mutex(Data.MutexThreadDraw);
            if(Data.SpecialMainCall){
                al_unlock_mutex(Data.MutexThreadDraw);
                /**
                    Receiving special call to do (generally draw)
                    something in the main thread. During this
                    operation the calling thread is synchronized
                    with the main thread.
                    */
                    Data.special_main_call_procedure(&Data);
                    Data.SpecialMainCall = false;
                    al_broadcast_cond(Data.CondSpecialMainCall);
                al_unlock_mutex(Data.MutexSpecialMainCall);
            }
            else{
                al_unlock_mutex(Data.MutexSpecialMainCall);
                /**
                    Normal boring drawing routines
                    */

                    Data.ThreadDrawWaiting = true;
                    while(Data.ThreadDrawWaiting){
                        al_wait_cond(Data.CondDrawCall, Data.MutexThreadDraw);
                    }
                al_unlock_mutex(Data.MutexThreadDraw);

                al_lock_mutex(Data.DrawMutex);
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    Data.DrawFunction(&Data);
                    al_flip_display();
                al_unlock_mutex(Data.DrawMutex);
                al_lock_mutex(Data.MutexFPS);
                    Data.FPS += 1;
                al_unlock_mutex(Data.MutexFPS);
            }
    }
    /**
        End draw thread
        */
    /**
        Clean-up
        */
    printf("Main thread at clean-up, waiting for EventQueue Thread\n");
    al_destroy_thread(Data.ThreadEventQueue);

    printf("Main thread at last clean-up, all threads safely closed\n");
    al_destroy_display(Data.Display);
    al_destroy_event_queue(Data.MainEventQueue);
    al_destroy_timer(Data.DrawTimer);

    clear_movable_object_list(&Data);
    free(Data.Level.MovableObjects);
    clear_fixed_object_list(&Data);
    free(Data.Level.FixedObjects);

#endif

    return 0;
}
