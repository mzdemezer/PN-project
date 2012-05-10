#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "menu.h"
#include "game.h"
#include "loading.h"

void* main_iteration(ALLEGRO_THREAD *thread, void *argument){
    #define Data ((struct GameSharedData*)argument)
    while(!Data->CloseNow){
        al_rest(2.0);
        printf("Iterated\n");
    }
    return NULL;
    #undef Data
}

int int_abs(int a){
    return a < 0 ? -a : a;
}

float float_abs(float a){
    return a < 0 ? -a : a;
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

char* int_to_str(int a){
    bool sign = a < 0;
    int length = rzad(a) + (int)sign, i, digit;
    char* result = (char*)malloc(sizeof(char) * (length + 1) );
    result[length] = '\0';
    a = int_abs(a);
    for(i = length - 1; i >= (int)sign; --i){
        digit = a % 10;
        a /= 10;
        result[i] = int_to_char(digit);
    }
    if(sign){
        result[0] = '-';
    }
    return result;
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

void draw_player(void *ObjectData){
    #define Data ((struct playerData*)ObjectData)
    al_draw_filled_circle(Data->center.x, Data->center.y, 40, al_map_rgb(255, 255, 255));
    #undef Data
}

void draw_particle(void *ObjectData){
    #define Data ((struct particleData*)ObjectData)
    al_draw_filled_circle(Data->center.x, Data->center.y, Data->r0, al_map_rgb(255, 0, 255));
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
    Data->r0 = Data->bok * SQRT2;
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

void construct_player(struct movable_object_structure *Object){
    Object->draw = draw_player;
    Object->r = rPlayer;
}

void construct_particle(struct movable_object_structure *Object){
    Object->draw = draw_particle;
    Object->r = rCircle;
}

/**
    Arrays interface
    */

void add_movable_object(struct GameSharedData *Data, enum movable_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.NumberOfMovableObjects == Data->Level.BoundryMovable){
        Data->Level.BoundryMovable *= 2;
        Data->Level.MovableObjects = (struct movable_object_structure*)realloc((void*)Data->Level.MovableObjects, sizeof(struct movable_object_structure) * Data->Level.BoundryMovable);
    }
    Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects].Type = NewObjectType;
    Data->Level.MovableObjects[Data->Level.NumberOfMovableObjects].ObjectData = NewObjectData;
    Data->Level.NumberOfMovableObjects += 1;
}

void add_fixed_object(struct GameSharedData *Data, enum fixed_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.NumberOfFixedObjects == Data->Level.BoundryFixed){
        Data->Level.BoundryFixed *= 2;
        Data->Level.FixedObjects = (struct fixed_object_structure*)realloc((void*)Data->Level.FixedObjects, sizeof(struct fixed_object_structure) * Data->Level.BoundryFixed);
    }
    Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects].Type = NewObjectType;
    Data->Level.FixedObjects[Data->Level.NumberOfFixedObjects].ObjectData = NewObjectData;
    Data->Level.NumberOfFixedObjects += 1;
}

void clear_fixed_object_list(struct GameSharedData *Data){
    ;
}

void delete_fixed_object(enum fixed_object_type ObjectType, void* ObjectData){
    ;
}

extern void RunAllTests(void);

int main(){

#ifdef TESTS

    printf("Running in test mode\n\n");
    RunAllTests();

#else

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

    /**
        Creating display
        */

    al_set_new_display_flags(ALLEGRO_FULLSCREEN); //ALLEGRO_WINDOWED
    //al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    //al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

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
    Data.ThreadMainIteration = NULL;
    Data.ThreadMainIteration = al_create_thread(main_iteration, (void*)&Data);
    if(!Data.ThreadMainIteration){
        fprintf(stderr, "Failed to initialize main_iteration thread, sorry");
        al_destroy_display(Data.Display);
        al_destroy_event_queue(Data.MainEventQueue);
        al_destroy_timer(Data.DrawTimer);
        return -1;
    }
    /**
        Initializing data
        */



    Data.Level.LevelNumber = 0;
    Data.Level.NumberOfMovableObjects = 0;
    Data.Level.NumberOfFixedObjects = 0;
    Data.Level.BoundryMovable = INITIAL_BOUNDRY_MOVABLE;
    Data.Level.BoundryFixed = INITIAL_BOUNDRY_FIXED;
    Data.Level.MovableObjects = (struct movable_object_structure*)malloc(sizeof(struct movable_object_structure) * Data.Level.BoundryMovable);
    Data.Level.FixedObjects =   (struct fixed_object_structure*)malloc(sizeof(struct fixed_object_structure)   * Data.Level.BoundryFixed);

    Data.Keyboard.KeyUp = ALLEGRO_KEY_UP;
    Data.Keyboard.KeyDown = ALLEGRO_KEY_DOWN;
    Data.Keyboard.KeyLeft = ALLEGRO_KEY_LEFT;
    Data.Keyboard.KeyRight = ALLEGRO_KEY_RIGHT;

    Data.RequestChangeState = false;
    Data.MutexChangeState = al_create_mutex();
    Data.DrawMutex = al_create_mutex();

    /**
        crap
        */
    int sec = 0;

    /**
        First draw
        */
    draw_menu(&Data);
    Data.FPS = 0;

    /**
        Main loop
        */
    al_start_timer(Data.DrawTimer);
    while(1){
        al_wait_for_event(Data.MainEventQueue, &Data.LastEvent);
        //printf("event type#%d\n", Data.LastEvent.type);

        if(Data.LastEvent.type == ALLEGRO_EVENT_TIMER){
            Data.DrawCall = true;
             /**
                crap
                */
            ++sec; if(sec == 60){printf("Second passed, FPS: %d\n", Data.FPS); sec=0; Data.FPS = 0;}
        }
        switch(Data.GameState){
            case gsGAME: handle_event_game(&Data); break;
            case gsMENU: handle_event_menu(&Data); break;
            case gsPAUSE: break;
            case gsLOADING: handle_event_loading(&Data); break;
        }

        if(Data.CloseNow){
            break;
        }

        if(Data.RequestChangeState){
            switch(Data.NewState){
                case gsPAUSE: break;
                case gsLOADING: request_loading(&Data); break;
                case gsGAME: request_game(&Data); break;
                case gsMENU: break;
            }
        }

        if(Data.DrawCall && al_is_event_queue_empty(Data.MainEventQueue)){
            al_lock_mutex(Data.DrawMutex);
                ++Data.FPS;
                Data.DrawCall = false;

                switch(Data.GameState){
                    case gsGAME: draw_game(&Data); break;
                    case gsLOADING: draw_loading(&Data); break;
                    case gsMENU: draw_menu(&Data); break;
                    case gsPAUSE: break;
                }
            al_unlock_mutex(Data.DrawMutex);
        }

    }



    /**
        Clean-up
        */
    al_destroy_display(Data.Display);
    al_destroy_event_queue(Data.MainEventQueue);
    al_destroy_timer(Data.DrawTimer);

#endif

    return 0;
}
