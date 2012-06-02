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

inline int int_abs(int a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

inline float float_abs(float a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

inline double double_abs(double a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

inline float float_min(float a, float b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline float float_max(float a, float b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

inline short int short_min(short int a, short int b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline short int short_max(short int a, short int b){
    if(a > b){
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
    Heap
    */

void construct_heap(struct collision_heap* heap, int size){
    heap->allocated = size;
    heap->length = 0;
    heap->heap = (struct collision_data*)malloc(sizeof(struct collision_data) * (size + 1));
}

void heapify(struct collision_heap* heap, short int i){
    short int l = heap_left(i),
              r = heap_right(i),
              largest;
    if(l <= heap->length && heap->heap[l].time < heap->heap[i].time){
        largest = l;
    }else{
        largest = i;
    }
    if(r <= heap->length && heap->heap[r].time < heap->heap[largest].time){
        largest = r;
    }
    if(largest != i){
        struct collision_data temp;
        temp = heap->heap[i];
        heap->heap[i] = heap->heap[largest];
        heap->heap[largest] = temp;
        heapify(heap, largest);
    }
}

void build_heap(struct collision_heap* heap){
    int i;
    for(i = heap->length >> 1; i >= 1; --i){
        heapify(heap, i);
    }
}

struct collision_data pop_min(struct collision_heap* heap){
    struct collision_data min;
    if(heap->length < 1){//error
        min.time = 10;
        return min;
    }
    min = heap->heap[1];
    heap->heap[1] = heap->heap[heap->length];
    heap->length -= 1;
    heapify(heap, 1);
    return min;
}

void heap_insert(struct collision_heap* heap, struct collision_data *collision){
    heap->length += 1;
    if(heap->length > heap->allocated){
        heap->allocated *= 2;
        heap->heap = (struct collision_data*)realloc(heap->heap, sizeof(struct collision_data) * (heap->allocated + 1));
    }
    int i = heap->length;
    while(i > 1 && heap->heap[heap_parent(i)].time > collision->time){
        heap->heap[i] = heap->heap[heap_parent(i)];
        i = heap_parent(i);
    }
    heap->heap[i] = *collision;
    /**
        Keeping who > with order for movables
        */
    if(heap->heap[i].with_movable){
        if(heap->heap[i].with < heap->heap[i].who){
            short int temp = heap->heap[i].who;
            heap->heap[i].who = heap->heap[i].with;
            heap->heap[i].with = temp;
        }
    }
}

void clear_heap(struct collision_heap* heap){
    heap->length = 0;
}

/**
    Red-Black Tree for zones
    */
RB_node* get_node(RB_tree *tree, short int key){
    RB_node *node = tree->root;
    while((node != tree->nil) && (node->key != key)){
        if(key < node->key){
            node = node->left;
        }else{
            node = node->right;
        }
    }
    return node;
}

/**
    No anti-NULL protection
    BEWARE
    */
RB_node* get_minimum(RB_node *node, RB_node *nil){
    while(node->left != nil){
        node = node->left;
    }
    return node;
}

RB_node* get_successor(RB_node *node, RB_node *nil){
    if(node->right != nil){
        return get_minimum(node->right, nil);
    }else{
        RB_node *succ = node->parent;
        while(succ != nil && node == succ->right){
            node = succ;
            succ = succ->parent;
        }
        return succ;
    }
}

void insert_node(RB_tree *tree, short int key){
    RB_node *node = tree->root,
            *last = tree->nil;
    while(node != tree->nil){
        last = node;
        if(key < node->key){
            node = node->left;
        }else{
            node = node->right;
        }
    }

    node = (RB_node*)malloc(sizeof(RB_node));
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = last;
    node->key = key;

    if(last == tree->nil){
        tree->root = node;
        node->color = BLACK;
    }else{
        if(key < last->key){
            last->left = node;
        }else{
            last->right = node;
        }

        node->color = RED;
        while(node != tree->root && node->parent->color == RED){
            if(is_left(node->parent)){
                last = node->parent->parent->right;
                if(last->color == RED){//1st CASE
                    node->parent->color = BLACK;
                    last->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }else{
                    if(node == node->parent->right){//2nd CASE --> 3rd
                        node = node->parent;
                        rotate_left(tree, node);
                    }
                    node->parent->color = BLACK;//3rd CASE
                    node->parent->parent->color = RED;
                    rotate_right(tree, node->parent->parent);
                }
            }else{
                last = node->parent->parent->left;
                if(last->color == RED){//1st CASE
                    node->parent->color = BLACK;
                    last->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }else{
                    if(node == node->parent->left){//2nd CASE --> 3rd
                        node = node->parent;
                        rotate_right(tree, node);
                    }
                    node->parent->color = BLACK;//3rd CASE
                    node->parent->parent->color = RED;
                    rotate_left(tree, node->parent->parent);
                }
            }
        }
        tree->root->color = BLACK;
    }
}

void delete_node(RB_tree *tree, short int key){
    RB_node *node = get_node(tree, key);
    if(node != tree->nil){
        RB_node *y, *x;
        if(node->left == tree->nil || node->right == tree->nil){
            y = node;
        }else{
            y = get_successor(node, tree->nil);
        }
        if(y->left != tree->nil){
            x = y->left;
        }else{
            x = y->right;
        }

        x->parent = y->parent;

        if(y->parent == tree->nil){
            tree->root = x;
        }else if(is_left(y)){
            y->parent->left = x;
        }else{
            y->parent->right = x;
        }
        if(y != node){
            node->key = y->key;
        }

        if(y->color == BLACK){
            RB_delete_fixup(tree, x);
        }

        free(y);
    }
}

void RB_delete_fixup(RB_tree *tree, RB_node *node){
    RB_node *sibl;
    while(node != tree->root && node->color == BLACK){
        if(is_left(node)){
            sibl = node->parent->right;
            if(sibl->color == RED){//1st CASE
                sibl->color = BLACK;
                sibl->parent->color = RED;
                rotate_left(tree, node->parent);
                sibl = node->parent->right;
            }
            if(sibl->left->color == BLACK && sibl->right->color == BLACK){
                //2nd CASE
                sibl->color = RED;
                node = node->parent;
            }else{
                if(sibl->right->color == BLACK){//3th CASE --> 4th
                    sibl->left->color = BLACK;
                    sibl->color = RED;
                    rotate_right(tree, sibl);
                    sibl = node->parent->right;
//                    if(sibl == tree->nil){
//                        sibl = node->parent->parent;
//                    }
                }
                sibl->color = node->parent->color;//4th CASE
                node->parent->color = BLACK;
                sibl->right->color = BLACK;
                rotate_left(tree, node->parent);
                node = tree->root;
            }
        }else{
            sibl = node->parent->left;
            if(sibl->color == RED){//1st CASE
                sibl->color = BLACK;
                sibl->parent->color = RED;
                rotate_right(tree, node->parent);
                sibl = node->parent->left;
            }
            if(sibl->left->color == BLACK && sibl->right->color == BLACK){//2nd CASE
                sibl->color = RED;
                node = node->parent;
            }else{
                if(sibl->left->color == BLACK){//3th CASE --> 4th
                    sibl->right->color = BLACK;
                    sibl->color = RED;
                    rotate_left(tree, sibl);
                    sibl = node->parent->left;
//                    if(sibl == tree->nil){
//                        sibl = node->parent->parent;
//                    }
                }
                sibl->color = node->parent->color;//4th CASE
                node->parent->color = BLACK;
                sibl->left->color = BLACK;
                rotate_right(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = BLACK;
}

void clear_nodes(RB_node *node, RB_node *nil){
    if(node != nil){
        clear_nodes(node->left, nil);
        clear_nodes(node->right, nil);
        free(node);
    }
}

void clear_tree(RB_tree *tree){
    if(tree->root != tree->nil){
        clear_nodes(tree->root, tree->nil);
        tree->root = tree->nil;
    }
}

/**
    This does NOT test if node is NULL or
    if its parent is NULL !!!!!!!
    */
inline bool is_left(RB_node *node){
    return node == node->parent->left;
}

/**
    Passing NULL to rotation will crush
    */
void rotate_left(RB_tree *tree, RB_node *node){
    RB_node *temp = node->right;
    if(node->right != tree->nil){
        node->right = temp->left;
        temp->left = node;
        node->right->parent = node;

        if(node->parent == tree->nil){
            tree->root = temp;
        }else{
            if(is_left(node)){
                node->parent->left = temp;
            }else{
                node->parent->right = temp;
            }
        }
        temp->parent = node->parent;
        node->parent = temp;
    }
}

void rotate_right(RB_tree *tree, RB_node *node){
    RB_node *temp = node->left;
    if(node->left != tree->nil){
        node->left = temp->right;
        temp->right = node;
        node->left->parent = node;

        if(node->parent == tree->nil){
            tree->root = temp;
        }else{
            if(is_left(node)){
                node->parent->left = temp;
            }else{
                node->parent->right = temp;
            }
        }
        temp->parent = node->parent;
        node->parent = temp;
    }
}

void in_order(RB_node *root, RB_node *nil){
    if(root != nil){
        in_order(root->left, nil);

        printf("%hd\n", root->key);

        in_order(root->right, nil);
    }
}

/**
    First it's important to get fresh collision data
    with fixed objects - that's the limit if the object
    doesn't collide with other movable one.
    */
void get_and_check_mov_coll_if_valid(struct GameSharedData *Data, short int who, short int with, float time_passed){
    struct collision_data coll = get_collision_with_movable(&Data->Level.MovableObjects[who], &Data->Level.MovableObjects[with]);
    coll.time += time_passed;
    if(coll.time >= 0 && coll.time <= 1){
        if(coll.time < Data->Level.MovableObjects[who].coll_with_fixed.time &&
           coll.time < Data->Level.MovableObjects[with].coll_with_fixed.time){//otherwise it's pointless to store such information
            coll.with_movable = true;
            coll.who = who;
            coll.with = with;
            coll_insert_node(&Data->Level.MovableObjects[who].colls_with_mov, &coll);
            coll.who = with;
            coll.with = who;
            coll_insert_node(&Data->Level.MovableObjects[with].colls_with_mov, &coll);
            if(time_passed > 0){
                struct collision_data *dirty_pointer = &coll_get_minimum(Data->Level.MovableObjects[with].colls_with_mov.root,
                                                                         Data->Level.MovableObjects[with].colls_with_mov.nil)->key;
                if(dirty_pointer != Data->Level.MovableObjects[with].next_collision){//check if DIRTY things happen
                    if(Data->Level.MovableObjects[with].next_collision->time <= 1 &&
                       Data->Level.MovableObjects[with].next_collision->time >= 0){
                        coll = *Data->Level.MovableObjects[with].next_collision;
                        if(coll.with < coll.who){
                            coll.who = coll.with;
                            coll.with = Data->Level.MovableObjects[with].next_collision->who;
                        }
                        coll_insert_node(&Data->Level.dirty_tree, &coll);
                    }
                    Data->Level.MovableObjects[with].next_collision = dirty_pointer;
                }
            }
        }
    }
}

/**
    A DFS that does something like that
    i  <--  some_value
    for(j = i + 1; j < length; ++j){
        DO_STUFF_DO_STUFF_DO_STUFF_DO_STUFF_DO_STUFF
        DO_MORE_STUFF
    }
    */

void for_each_higher_check_collision(struct GameSharedData *Data, bool *movable_done, short int who, RB_node *node, RB_node *nil){
    while(node != nil &&
          node->key < who){
        node = node->right;
    }
    if(node != nil){
        if(who != node->key){
            for_each_higher_check_collision(Data, movable_done, who, node->left, nil);
            if(!movable_done[node->key]){
                movable_done[node->key] = true;
                get_and_check_mov_coll_if_valid(Data, who, node->key, 0);
            }
        }
        in_order_check_collision(Data, movable_done, who, node->right, nil);
    }
}

void in_order_check_collision(struct GameSharedData *Data, bool *movable_done, short int who, RB_node *node, RB_node *nil){
    if(node != nil){
        in_order_check_collision(Data, movable_done, who, node->left, nil);

        if(!movable_done[node->key]){
            movable_done[node->key] = true;
            get_and_check_mov_coll_if_valid(Data, who, node->key, 0);
        }

        in_order_check_collision(Data, movable_done, who, node->right, nil);
    }
}

void in_order_find_new_collision(struct GameSharedData *Data, bool *movable_done, short int who, short int ommit, RB_node *node, RB_node *nil, float time_passed){
    if(node != nil){
        in_order_find_new_collision(Data, movable_done, who, ommit, node->left, nil, time_passed);

        if(!movable_done[node->key]){
            movable_done[node->key] = true;
            if(who != node->key &&
               ommit != node->key){
                get_and_check_mov_coll_if_valid(Data, who, node->key, time_passed);
            }
        }

        in_order_find_new_collision(Data, movable_done, who, ommit, node->right, nil, time_passed);
    }
}
/**
    Red-Black Tree for collisions
    */
short int coll_comp(struct collision_data *a, struct collision_data *b){
    if(a->time < b->time){
        return LESS;
    }else if(a->time > b->time){
        return MORE;
    }else{
        if(a->who < b->who){
            return LESS;
        }else if(a->who > b->who){
            return MORE;
        }else{
            if(a->with < b->with){
                return LESS;
            }else if(a->with > b->with){
                return MORE;
            }else{
                if((short int)a->with_movable < (short int)b->with_movable){
                    return LESS;
                }else if((short int)a->with_movable > (short int)b->with_movable){
                    return MORE;
                }else{
                    return EQUAL;
                }
            }
        }
    }
}

short int coll_rev_comp(struct collision_data *a, struct collision_data *b){
    if(a->time < b->time){
        return LESS;
    }else if(a->time > b->time){
        return MORE;
    }else{
        if(a->who < b->with){
            return LESS;
        }else if(a->who > b->with){
            return MORE;
        }else{
            if(a->with < b->who){
                return LESS;
            }else if(a->with > b->who){
                return MORE;
            }else{
                if((short int)a->with_movable < (short int)b->with_movable){
                    return LESS;
                }else if((short int)a->with_movable > (short int)b->with_movable){
                    return MORE;
                }else{
                    return EQUAL;
                }
            }
        }
    }
}

coll_node* coll_get_node(coll_tree *tree, struct collision_data *key){
    coll_node *node = tree->root;
    short int comp;
    while(node != tree->nil){
        comp = coll_comp(key, &node->key);
        if(comp == EQUAL){
            break;
        }else if(comp == LESS){
            node = node->left;
        }else{
            node = node->right;
        }
    }
    return node;
}

coll_node* coll_get_minimum(coll_node *node, coll_node *nil){
    if(node != nil){
        while(node->left != nil){
            node = node->left;
        }
    }
    return node;
}

coll_node* coll_get_successor(coll_node *node, coll_node *nil){
    if(node->right != nil){
        return coll_get_minimum(node->right, nil);
    }else{
        coll_node *succ = node->parent;
        while(succ != nil && node == succ->right){
            node = succ;
            succ = succ->parent;
        }
        return succ;
    }
}

void coll_insert_node(coll_tree *tree, struct collision_data *key){
    coll_node *node = tree->root,
              *last = tree->nil;
    short int comp = LESS;
    while(node != tree->nil){
        last = node;
        comp = coll_comp(key, &node->key);
        if(comp == EQUAL){
            break;
        }else if(comp == LESS){
            node = node->left;
        }else{
            node = node->right;
        }
    }
    if(comp == EQUAL){
        node->counter += 1;
    }else{
        node = (coll_node*)malloc(sizeof(coll_node));
        node->left = tree->nil;
        node->right = tree->nil;
        node->parent = last;
        node->counter = 1;
        node->key = *key;

        if(last == tree->nil){
            tree->root = node;
            node->color = BLACK;
        }else{
            if(key->time < last->key.time){
                last->left = node;
            }else{
                last->right = node;
            }

            node->color = RED;
            while(node != tree->root && node->parent->color == RED){
                if(coll_is_left(node->parent)){
                    last = node->parent->parent->right;
                    if(last->color == RED){//1st CASE
                        node->parent->color = BLACK;
                        last->color = BLACK;
                        node->parent->parent->color = RED;
                        node = node->parent->parent;
                    }else{
                        if(node == node->parent->right){//2nd CASE --> 3rd
                            node = node->parent;
                            coll_rotate_left(tree, node);
                        }
                        node->parent->color = BLACK;//3rd CASE
                        node->parent->parent->color = RED;
                        coll_rotate_right(tree, node->parent->parent);
                    }
                }else{
                    last = node->parent->parent->left;
                    if(last->color == RED){//1st CASE
                        node->parent->color = BLACK;
                        last->color = BLACK;
                        node->parent->parent->color = RED;
                        node = node->parent->parent;
                    }else{
                        if(node == node->parent->left){//2nd CASE --> 3rd
                            node = node->parent;
                            coll_rotate_right(tree, node);
                        }
                        node->parent->color = BLACK;//3rd CASE
                        node->parent->parent->color = RED;
                        coll_rotate_left(tree, node->parent->parent);
                    }
                }
            }
            tree->root->color = BLACK;
        }
    }
}

bool coll_delete_node(coll_tree *tree, struct collision_data *key){
    coll_node *node = coll_get_node(tree, key);
    if(node == tree->nil){
        return false;
    }else{
        node->counter -= 1;
        if(node->counter == 0){
            coll_node *y, *x;
            if(node->left == tree->nil || node->right == tree->nil){
                y = node;
            }else{
                y = coll_get_successor(node, tree->nil);
            }
            if(y->left != tree->nil){
                x = y->left;
            }else{
                x = y->right;
            }

            x->parent = y->parent;

            if(y->parent == tree->nil){
                tree->root = x;
            }else if(coll_is_left(y)){
                y->parent->left = x;
            }else{
                y->parent->right = x;
            }
            if(y != node){
                node->key = y->key;
                node->counter = y->counter;
            }

            if(y->color == BLACK){
                coll_delete_fixup(tree, x);
            }

            free(y);
        }
        return true;
    }
}

void coll_delete_fixup(coll_tree *tree, coll_node *node){
    coll_node *sibl;
    while(node != tree->root && node->color == BLACK){
        if(coll_is_left(node)){
            sibl = node->parent->right;
            if(sibl->color == RED){//1st CASE
                sibl->color = BLACK;
                sibl->parent->color = RED;
                coll_rotate_left(tree, node->parent);
                sibl = node->parent->right;
            }
            if(sibl->left->color == BLACK && sibl->right->color == BLACK){//2nd CASE
                sibl->color = RED;
                node = node->parent;
            }else{
                if(sibl->right->color == BLACK){//3th CASE --> 4th
                    sibl->left->color = BLACK;
                    sibl->color = RED;
                    coll_rotate_right(tree, sibl);
                    sibl = node->parent->right;
                }
                sibl->color = node->parent->color;//4th CASE
                node->parent->color = BLACK;
                sibl->right->color = BLACK;
                coll_rotate_left(tree, node->parent);
                node = tree->root;
            }
        }else{
            sibl = node->parent->left;
            if(sibl->color == RED){//1st CASE
                sibl->color = BLACK;
                sibl->parent->color = RED;
                coll_rotate_right(tree, node->parent);
                sibl = node->parent->left;
            }
            if(sibl->left->color == BLACK && sibl->right->color == BLACK){//2nd CASE
                sibl->color = RED;
                node = node->parent;
            }else{
                if(sibl->left->color == BLACK){//3th CASE --> 4th
                    sibl->right->color = BLACK;
                    sibl->color = RED;
                    coll_rotate_left(tree, sibl);
                    sibl = node->parent->left;
                }
                sibl->color = node->parent->color;//4th CASE
                node->parent->color = BLACK;
                sibl->left->color = BLACK;
                coll_rotate_right(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = BLACK;
}

void coll_clear_nodes(coll_node *node, coll_node *nil){
    if(node != nil){
        coll_clear_nodes(node->left, nil);
        coll_clear_nodes(node->right, nil);
        free(node);
    }
}

void coll_clear_tree(coll_tree *tree){
    if(tree->root != tree->nil){
        coll_clear_nodes(tree->root, tree->nil);
        tree->root = tree->nil;
    }
}

/**
    This does NOT test if node is NULL or
    if its parent is NULL !!!!!!!
    */
inline bool coll_is_left(coll_node *node){
    return node == node->parent->left;
}

/**
    Passing NULL to rotation will crush
    */
void coll_rotate_left(coll_tree *tree, coll_node *node){
    coll_node *temp = node->right;
    if(node->right != tree->nil){
        node->right = temp->left;
        temp->left = node;
        node->right->parent = node;

        if(node->parent == tree->nil){
            tree->root = temp;
        }else{
            if(coll_is_left(node)){
                node->parent->left = temp;
            }else{
                node->parent->right = temp;
            }
        }
        temp->parent = node->parent;
        node->parent = temp;
    }
}

void coll_rotate_right(coll_tree *tree, coll_node *node){
    coll_node *temp = node->left;
    if(node->left != tree->nil){
        node->left = temp->right;
        temp->right = node;
        node->left->parent = node;

        if(node->parent == tree->nil){
            tree->root = temp;
        }else{
            if(coll_is_left(node)){
                node->parent->left = temp;
            }else{
                node->parent->right = temp;
            }
        }
        temp->parent = node->parent;
        node->parent = temp;
    }
}

void coll_clear_trash(struct GameSharedData *Data, coll_node *node, coll_node *nil){
    if(node != nil){
        coll_clear_trash(Data, node->left, nil);
        coll_clear_trash(Data, node->right, nil);

        short int temp = node->key.with;
        node->key.with = node->key.who;
        node->key.who = temp;

        temp = coll_comp(&node->key, Data->Level.MovableObjects[node->key.who].next_collision);
        coll_delete_node(&Data->Level.MovableObjects[node->key.who].colls_with_mov, &node->key);
        if(temp == EQUAL){
            //mark dirty
            temp = node->key.who;
            if(node->key.with < node->key.who){
                node->key.who = node->key.with;
                node->key.with = temp;
            }
            coll_insert_node(&Data->Level.dirty_tree, &node->key);
            //find new min
            collision_min_for_object(Data, temp);
            //push on queue //how to push so that one collision gets on queue only once?
            if(Data->Level.MovableObjects[node->key.who].next_collision->time >= 0 &&
               Data->Level.MovableObjects[node->key.who].next_collision->time <= 1){
                heap_insert(&Data->Level.collision_queue, Data->Level.MovableObjects[node->key.who].next_collision);
            }
        }
        free(node);
    }
}


void coll_in_order(coll_node *root, coll_node *nil){
    if(root != nil){
        coll_in_order(root->left, nil);

        printf("%f\n", root->key.time);

        coll_in_order(root->right, nil);
    }
}

/**
    Zones
    */

void get_zone(float x, float y, short int *zone){
    /**
        Closure: [0 ; SCREEN_BUFFER_HEIGHT) ---> [0 ; SCREEN_BUFFER_HEIGHT]
        */
    if(float_abs(x - SCREEN_BUFFER_HEIGHT) < eps){
        zone[0] = ZONE_FACTOR - 1;
    }else{
        zone[0] = (short int)((int)x / ZONE_SIZE);
        if(x < 0){
            zone[0] -= 1;
        }
    }
    if(float_abs(y - SCREEN_BUFFER_HEIGHT) < eps){
        zone[1] = ZONE_FACTOR - 1;
    }else{
        zone[1] = (short int)((int)y / ZONE_SIZE);
        if(y < 0){
            zone[1] -= 1;
        }
    }
}

void get_zone_for_object(float x, float y, float dx, float dy, float r0, short int *zone){
    dx = float_abs(dx) + r0;
    dy = float_abs(dy) + r0;
    get_zone(x - dx, y - dy, zone);
    get_zone(x + dx, y + dy, zone + 2);
    if(zone[0] < 0){
        zone[0] = 0;
    }
    if(zone[1] < 0){
        zone[1] = 0;
    }
    if(zone[2] >= ZONE_FACTOR){
        zone[2] = ZONE_FACTOR - 1;
    }
    if(zone[3] >= ZONE_FACTOR){
        zone[3] = ZONE_FACTOR - 1;
    }
}

void add_primitive_to_zone(struct zone* zone, short int key){
    if(zone->number_of_primitives >= zone->allocated){
        zone->allocated *= 2;
        zone->primitives = (short int*)realloc(zone->primitives, sizeof(short int) * zone->allocated);
        printf("REALLOCATION\n");
    }

    zone->primitives[zone->number_of_primitives] = key;
    zone->number_of_primitives += 1;
}

void initialize_zones_with_movable(struct GameSharedData *Data, short int *zones, short int index){
    int i, j;
    for(i = zones[0]; i <= zones[2]; ++i){
        for(j = zones[1]; j <= zones[3]; ++j){
            insert_node(&Data->Level.zones[i][j].movable, index);
        }
    }
}

/**
    Sets zones for new  dx  and  dy  values
    Requires setting new dx and dy before calling
    It only looks bad, but it seems quite optimal
    actually :P
    */
void change_zones_for_movable(struct GameSharedData *Data, short int index, float t){
    short int oldz[4], xleft, xright;
    int i, j;
    struct movable_object_structure *Obj = &Data->Level.MovableObjects[index];
    for(i = 0; i < 4; ++i){
        oldz[i] = Obj->zones[i];
    }
    get_zone_for_object(((struct point*)(Obj->ObjectData))->x,
                        ((struct point*)(Obj->ObjectData))->y,
                        Obj->dx * t, Obj->dy * t, ((struct circleData*)Obj->ObjectData)->r,
                        Obj->zones);
    #define newz(x) (Obj->zones[x])
    #define Zonez(x, y) (Data->Level.zones[x][y])
    if(newz(2) >= oldz[0] &&
       newz(0) <= oldz[2] &&
       newz(3) >= oldz[1] &&
       newz(1) <= oldz[3]){//There are some common zones

        if(newz(0) > oldz[0]){
            for(i = oldz[0]; i < newz(0); ++i){
                for(j = oldz[1]; j <= oldz[3]; ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = newz(0); i < oldz[0]; ++i){
                for(j = newz(1); j <= newz(3); ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }

        if(oldz[2] > newz(2)){
            for(i = newz(2) + 1; i <= oldz[2]; ++i){
                for(j = oldz[1]; j <= oldz[3]; ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = oldz[2] + 1; i <= newz(2); ++i){
                for(j = newz(1); j <= newz(3); ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }

        xleft = short_max(newz(0), oldz[0]);
        xright = short_min(newz(2), oldz[2]);

        if(newz(1) < oldz[1]){
            for(i = xleft; i <= xright; ++i){
                for(j = newz(1); j < oldz[1]; ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = xleft; i <= xright; ++i){
                for(j = oldz[1]; j < newz(1); ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }

        if(newz(3) > oldz[3]){
            for(i = xleft; i <= xright; ++i){
                for(j = oldz[3] + 1; j <= newz(3); ++j){
                    insert_node(&Zonez(i, j).movable, index);
                }
            }
        }else{
            for(i = xleft; i <= xright; ++i){
                for(j = newz(3) + 1; j <= oldz[3]; ++j){
                    delete_node(&Zonez(i, j).movable, index);
                }
            }
        }
    }else{
        for(i = oldz[0]; i <= oldz[2]; ++i){
            for(j = oldz[1]; j <= oldz[3]; ++j){
                delete_node(&Zonez(i, j).movable, index);
            }
        }

        for(i = newz(0); i <= newz(2); ++i){
            for(j = newz(1); j <= newz(3); ++j){
                insert_node(&Zonez(i, j).movable, index);
            }
        }
    }
    #undef Zonez
    #undef newz
}
/**
    Collisions
    */

void move_objects(struct GameSharedData *Data, float t){
    int i;
    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
        ((struct point*)Data->Level.MovableObjects[i].ObjectData)->x += Data->Level.MovableObjects[i].dx * t;
        ((struct point*)Data->Level.MovableObjects[i].ObjectData)->y += Data->Level.MovableObjects[i].dy * t;
    }
}

inline double double_min(double a, double b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

float check_collision_between_two_balls(double x, double y, float dx, float dy, double d){
    double a = dx * dx + dy * dy,
        b = 2 * (x * dx + y * dy);
    if(a == 0){//linear
        if(b == 0){
            return EMPTY_COLLISION_TIME;
        }else{
            b = -(x * x + y * y + d * d) / b;
            if(b <= 0 && b >= 1){
                return b;
            }else{
                return EMPTY_COLLISION_TIME;
            }
        }
    }else{
        d *= d;
        d = b * b - 4 * a * (x * x + y * y - d);//delta
        if(d < 0){
            return EMPTY_COLLISION_TIME;
        }else{
            b = -b;
            a *= 2;
            if(d == 0){
                b /= a;
                if(b >= 0 && b <= 1){
                    return b;
                }
                else{
                    return EMPTY_COLLISION_TIME;
                }
            }else{
                d = sqrt(d);
                x = (b + d) / a;
                y = (b - d) / a;
                if(x >= 0 && x <= 1){
                    if(y >= 0 && y <= 1){
                        return double_min(x, y);
                    }else{
                        return x;
                    }
                }else{
                    if(y >= 0 && y <= 1){
                        return y;
                    }else{
                        return EMPTY_COLLISION_TIME;
                    }
                }
            }
        }
    }
}

struct collision_data get_collision_with_primitive(struct movable_object_structure *who, struct primitive_object_structure *with_what){
    struct collision_data new_coll;
    new_coll.time = EMPTY_COLLISION_TIME;
    //BIG SWITCH
    return new_coll;
}

struct collision_data get_collision_with_movable(struct movable_object_structure *who, struct movable_object_structure *with_whom){
    struct collision_data new_coll;
    new_coll.time = EMPTY_COLLISION_TIME;
    switch(who->Type){
        case motPLAYER:
            #define WHO_PLAYER ((struct playerData*)who->ObjectData)
            switch(with_whom->Type){
                case motPLAYER:
                    #define WITH_PLAYER ((struct playerData*)with_whom->ObjectData)
                    new_coll.time = check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PLAYER->r + WHO_PLAYER->r);
                    #undef WITH_PLAYER
                    break;
                case motPARTICLE:
                    #define WITH_PARTICLE ((struct particleData*)with_whom->ObjectData)
                    new_coll.time = check_collision_between_two_balls(WITH_PARTICLE->center.x - WHO_PLAYER->center.x,
                                                                      WITH_PARTICLE->center.y - WHO_PLAYER->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PARTICLE->r + WHO_PLAYER->r);
                    //printf("%f\n", new_coll.time);
                    #undef WITH_PARTICLE
                    break;
                default:
                    break;
            }
            #undef WHO_PLAYER
            break;
        case motPARTICLE:
            #define WHO_PARTICLE ((struct particleData*)who->ObjectData)
            switch(with_whom->Type){
                case motPLAYER:
                    #define WITH_PLAYER ((struct playerData*)with_whom->ObjectData)
                    new_coll.time = check_collision_between_two_balls(WITH_PLAYER->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_PLAYER->center.y - WHO_PARTICLE->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PLAYER->r + WHO_PARTICLE->r);
                    #undef WITH_PLAYER
                    break;
                case motPARTICLE:
                    #define WITH_PARTICLE ((struct particleData*)with_whom->ObjectData)
                    new_coll.time = check_collision_between_two_balls(WITH_PARTICLE->center.x - WHO_PARTICLE->center.x,
                                                                      WITH_PARTICLE->center.y - WHO_PARTICLE->center.y,
                                                                      with_whom->dx - who->dx, with_whom->dy - who->dy,
                                                                      WITH_PARTICLE->r + WHO_PARTICLE->r);
                    #undef WITH_PARTICLE
                    break;
                default:
                    break;
            }
            #undef WHO_PARTICLE
            break;
        default:
            break;
    }

    return new_coll;
}

void collision_min_for_object(struct GameSharedData *Data, short int who){
    if(Data->Level.MovableObjects[who].colls_with_mov.root == Data->Level.MovableObjects[who].colls_with_mov.nil){
        Data->Level.MovableObjects[who].next_collision = &Data->Level.MovableObjects[who].coll_with_fixed;
    }else{
        Data->Level.MovableObjects[who].next_collision = &(coll_get_minimum(Data->Level.MovableObjects[who].colls_with_mov.root,
                                                                              Data->Level.MovableObjects[who].colls_with_mov.nil)->key);
    }
}

void find_next_collision(struct GameSharedData *Data, short int index, short int ommit, bool *primitive_done, bool *movable_done, float time_passed){
    struct collision_data new_coll;
    int i, j, k;
    for(i = 0; i < Data->Level.number_of_primitive_objects; ++i){
        primitive_done[i] = false;
    }
    for(i = 0; i < Data->Level.number_of_movable_objects; ++i){
        movable_done[i] = false;
    }
    new_coll.who = index;
    new_coll.with_movable = false;
    Data->Level.MovableObjects[index].coll_with_fixed.time = EMPTY_COLLISION_TIME;
    for(i = Data->Level.MovableObjects[index].zones[0]; i <= Data->Level.MovableObjects[index].zones[2]; ++i){
        for(j = Data->Level.MovableObjects[index].zones[1]; j <= Data->Level.MovableObjects[index].zones[3]; ++j){
            for(k = 0; k < Data->Level.zones[i][j].number_of_primitives; ++k){
                if(!primitive_done[Data->Level.zones[i][j].primitives[k]]){
                    primitive_done[Data->Level.zones[i][j].primitives[k]] = true;
                    new_coll = get_collision_with_primitive(&Data->Level.MovableObjects[index],
                                                            &Data->Level.PrimitiveObjects[Data->Level.zones[i][j].primitives[k]]);
                    new_coll.time += time_passed;
                    if(new_coll.time >= 0 && new_coll.time <= 1){
                        if(new_coll.time < Data->Level.MovableObjects[index].coll_with_fixed.time){
                            new_coll.with = Data->Level.zones[i][j].primitives[k];
                            Data->Level.MovableObjects[index].coll_with_fixed = new_coll;
                        }
                    }
                }

            }
        }
    }
    for(i = Data->Level.MovableObjects[index].zones[0]; i <= Data->Level.MovableObjects[index].zones[2]; ++i){
        for(j = Data->Level.MovableObjects[index].zones[1]; j <= Data->Level.MovableObjects[index].zones[3]; ++j){
            in_order_find_new_collision(Data, movable_done,
                                            index, ommit,
                                            Data->Level.zones[i][j].movable.root,
                                            Data->Level.zones[i][j].movable.nil,
                                            time_passed);
        }
    }

    collision_min_for_object(Data, index);

    //push on queue  //heap checks if who < with and does necessary exchanges
    if(Data->Level.MovableObjects[index].next_collision->time <= 1 &&
       Data->Level.MovableObjects[index].next_collision->time >= 0){
        heap_insert(&Data->Level.collision_queue, Data->Level.MovableObjects[index].next_collision);
    }
}


void get_line_from_points(float x1, float y1, float x2, float y2, struct line *L){
    L->A = y1 - y2;
    L->B = x2 - x1;
    L->C = -x1 * L->A - y1 * L->B;
}

void get_line_from_point_and_vector(float x, float y, float vx, float vy, struct line *L){
    L->A = -vy;
    L->B = vx;
    L->C = x * vy - y * vx;
}

void common_point(const struct line* L1, const struct line* L2, float *x, float *y){
    *y = (L1->C * L2->A - L1->A * L2->C) / (L2->B * L1->A - L1->B * L2->A);
    *x = -(L1->C + L1->B * *y) / L1->A;
}

/**
    for vectors [x1, y1] and [x2, y2]
    */
float vector_product(float x1, float y1, float x2, float y2){
    return x1 * y2 - x2 * y1;
}

bool vectors_on_two_sides(float vector_pr1, float vector_pr2){
    if(vector_pr1 == 0 || vector_pr2 == 0){
        return true;
    }else if((vector_pr1 > 0 && vector_pr2 < 0) ||
             (vector_pr1 < 0 && vector_pr2 > 0) ){
        return true;
    }else{
        return false;
    }
}

bool do_segments_intersect(const struct point *A1, const struct point *A2,
                           const struct point *B1, const struct point *B2){
    float v_x = A2->x - A1->x,
          v_y = A2->y - A1->y,
          b_x = B1->x - A1->x,
          b_y = B1->y - A1->y;

    if(vectors_on_two_sides(vector_product(v_x, v_y, B2->x - A1->x, B2->y - A1->y), vector_product(v_x, v_y, b_x, b_y))){
        b_x = -b_x;
        b_y = -b_y;
        v_x = B2->x - B1->x;
        v_y = B2->y - B1->y;
        if(vectors_on_two_sides(vector_product(v_x, v_y, b_x, b_y), vector_product(v_x, v_y, A2->x - B1->x, A2->y - B1->y))){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

/**
    This function uses do_segments_intersect
    */
bool get_segment_intersection(const struct point *A1, const struct point *A2,
                              const struct point *B1, const struct point *B2,
                              struct point *I){
    if(do_segments_intersect(A1, A2, B1, B2)){
        double dxA = (double)A1->x - A2->x,
               dxB = (double)B1->x - B2->x,
               dyA = (double)A1->y - A2->y,
               dyB = (double)B1->y - B2->y,
               denom = dxA * dyB - dyA * dxB,
               dxyA,
               dxyB;
        /**
            If segments cover each other
            function does nothing
            */
        if(double_abs(denom) < eps){
            return false;
        }else{
            dxyA = (double)A1->x * A2->y - (double)A1->y * A2->x;
            dxyB = (double)B1->x * B2->y - (double)B1->y * B2->x;

            I->x = (dxyA * dxB - dxA * dxyB) / denom;
            I->y = (dxyA * dyB - dyA * dxyB) / denom;
            return true;
        }
    }else{
        return false;
    }
}

/**
    A lot of freakin' cases!
    This function should be fired
    after checking rectangular
    limits of the segment and the map
    */
bool get_intersection_within_borders(const struct point *A, const struct point *B, short int *zones){
    struct point correct, Bord1, Bord2;
    if(zones[0] < 0){
            Bord1.x = 0;
            Bord1.y = 0;
            Bord2.x = 0;
            Bord2.y = SCREEN_BUFFER_HEIGHT;
            if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                get_zone(correct.x, correct.y, zones);
                return true;
            }else{
                if(zones[1] < 0){
                    Bord2.x = SCREEN_BUFFER_HEIGHT;
                    Bord2.y = 0;
                    if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                        get_zone(correct.x, correct.y, zones);
                        return true;
                    }else{
                        return false;
                    }
                }else if(zones[1] >= ZONE_FACTOR){
                    Bord1.x = SCREEN_BUFFER_HEIGHT;
                    Bord1.y = SCREEN_BUFFER_HEIGHT;
                    if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                        get_zone(correct.x, correct.y, zones);
                        return true;
                    }else{
                        return false;
                    }
                }else{
                    return false;
                }
            }
        }else if(zones[0] >= ZONE_FACTOR){
            Bord1.x = SCREEN_BUFFER_HEIGHT;
            Bord1.y = 0;
            Bord2.x = SCREEN_BUFFER_HEIGHT;
            Bord2.y = SCREEN_BUFFER_HEIGHT;
            if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                get_zone(correct.x, correct.y, zones);
                return true;
            }else{
                if(zones[1] < 0){
                    Bord2.x = 0;
                    Bord2.y = 0;
                    if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                        get_zone(correct.x, correct.y, zones);
                        return true;
                    }else{
                        return false;
                    }
                }else if(zones[1] >= ZONE_FACTOR){
                    Bord1.x = 0;
                    Bord1.y = SCREEN_BUFFER_HEIGHT;
                    if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                        get_zone(correct.x, correct.y, zones);
                        return true;
                    }else{
                        return false;
                    }
                }else{
                    return false;
                }
            }
        }else{
            if(zones[1] < 0){
                Bord1.x = 0;
                Bord1.y = 0;
                Bord2.x = SCREEN_BUFFER_HEIGHT;
                Bord2.y = 0;
                if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else if(zones[1] >= ZONE_FACTOR){
                Bord1.x = 0;
                Bord1.y = SCREEN_BUFFER_HEIGHT;
                Bord2.x = SCREEN_BUFFER_HEIGHT;
                Bord2.y = SCREEN_BUFFER_HEIGHT;
                if(get_segment_intersection(A, B, &Bord1, &Bord2, &correct)){
                    get_zone(correct.x, correct.y, zones);
                    return true;
                }else{
                    return false;
                }
            }else{
                /**
                The point is fully inside map
                and doesn't need any correction
                */
                return true;
            }
        }
}

bool get_outer_zones_of_segment(const struct point *A, const struct point *B, short int *zones){
    get_zone(A->x, A->y, zones);
    get_zone(B->x, B->y, zones + 2);
    /**
        Rule out the cases when the rectangular limits
        of the segments and the map don't collide
        */
          if(zones[0] < 0 && zones[2] < 0){
        return false;
    }else if(zones[1] < 0 && zones[3] < 0){
        return false;
    }else if(zones[0] >= ZONE_FACTOR && zones[2] >= ZONE_FACTOR){
        return false;
    }else if(zones[1] >= ZONE_FACTOR && zones[3] >= ZONE_FACTOR){
        return false;
    }else{
        /**
            Now, if any point is outside the bonds
            it is either possible to find intersection
            with the border and continue from there
            or segment is still outside the map
            */
        if(!get_intersection_within_borders(A, B, zones)){
            return false;
        }else if(!get_intersection_within_borders(B, A, zones + 2)){
            return false;
        }else{
            return true;
        }
    }
}

void normalize_segment_zones(short int *zones){
    short int temp;
    if(zones[0] > zones[2]){
        temp = zones[2];
        zones[2] = zones[0];
        zones[0] = temp;
        temp = zones[3];
        zones[3] = zones[1];
        zones[1] = temp;
    }
}

/**
    This function does all the allocating
    and counting. If segment is outside the map
    it is not added anywhere
    */
void add_segment(struct GameSharedData *Data, const struct point *A, const struct point *B){
    short int zones[4];
    if(get_outer_zones_of_segment(A, B, zones)){
        /**
            When correct zones are set it's time to work
            */
        normalize_segment_zones(zones);
        short int i = sign(zones[3] - zones[1]),
                  key = Data->Level.number_of_primitive_objects;
        struct segment *seg = (struct segment*)malloc(sizeof(struct segment));
        seg->ang = VectorAngle(B->x - A->x, B->y - A->y);
        seg->A = *A;
        seg->B = *B;
        add_primitive_object(Data, potSEGMENT, &seg);
        if(zones[0] == zones[2]){//to simplify: vertical
            short int j;
            for(j = zones[1]; j != zones[3]; j += i){
                add_primitive_to_zone(&Data->Level.zones[zones[0]][j], key);
            }
            add_primitive_to_zone(&Data->Level.zones[zones[0]][zones[3]], key);
        }else if(zones[1] == zones[3]){//horizontal
            short int j;
            for(j = zones[0]; j != zones[2]; ++j){
                add_primitive_to_zone(&Data->Level.zones[j][zones[1]], key);
            }
            add_primitive_to_zone(&Data->Level.zones[zones[2]][zones[1]], key);
        }else{
            struct point Bord1, Bord2;
            while(zones[0] != zones[2] &&
                  zones[1] != zones[3]){
                add_primitive_to_zone(&Data->Level.zones[zones[0]][zones[1]], key);

                Bord1.x = (zones[0] + 1) * ZONE_SIZE;
                Bord1.y = zones[1] * ZONE_SIZE;
                Bord2.x = Bord1.x;
                Bord2.y = Bord1.y + ZONE_SIZE;
                if(do_segments_intersect(&Bord1, &Bord2, A, B)){
                    zones[0] += 1;
                }else{
                    zones[1] += i;
                }
            }
            add_primitive_to_zone(&Data->Level.zones[zones[2]][zones[3]], key);
        }
    }
}

/**
    This function does not allocate
    the object data - it needs to
    get newly allocated point.
    */
void add_point(struct GameSharedData *Data, struct point *A){
    short int zone[2];
    get_zone(A->x, A->y, zone);
    if(zone[0] >= 0 && zone[0] < ZONE_FACTOR &&
       zone[1] >= 0 && zone[1] < ZONE_FACTOR){
        add_primitive_object(Data, potPOINT, (void*)A);
        add_primitive_to_zone(&Data->Level.zones[zone[0]][zone[1]], Data->Level.number_of_primitive_objects - 1);
    }
}

void add_circle(struct GameSharedData *Data, float r, struct point center){
    short int zones[4];
    get_zone(center.x - r, center.y - r, zones);
    get_zone(center.x + r, center.y + r, zones + 2);
    if(zones[0] < ZONE_FACTOR && zones[1] < ZONE_FACTOR &&
       zones[2] >= 0 && zones[3] >= 0){
        if(zones[0] < 0){
            zones[0] = 0;
        }
        if(zones[1] < 0){
            zones[1] = 0;
        }
        if(zones[2] >= ZONE_FACTOR){
            zones[2] = ZONE_FACTOR - 1;
        }
        if(zones[3] >= ZONE_FACTOR){
            zones[3] = ZONE_FACTOR - 1;
        }
        struct circle *C = (struct circle*)malloc(sizeof(struct circle));
        C->center = center;
        C->r = r;
        short int i, j, key = Data->Level.number_of_primitive_objects;
        add_primitive_object(Data, potCIRCLE, C);
        float r0 = (SQRT2 / 2) * ZONE_SIZE,
              dx, dy;
        for(i = zones[0]; i <= zones[2]; ++i){
            for(j = zones[1]; j <= zones[3]; ++j){
                dx = C->center.x - (i + 0.5) * ZONE_SIZE;
                dy = C->center.y - (j + 0.5) * ZONE_SIZE;
                if(sqrt(dx * dx + dy * dy) < squareEquation(r0, VectorAngle(dx, dy)) + C->r){
                    add_primitive_to_zone(&Data->Level.zones[i][j], key);
                }
            }
        }
    }
}

void add_square(struct GameSharedData *Data, struct squareData *square){
    add_point(Data, square->v1);
    add_point(Data, square->v2);
    add_point(Data, square->v3);
    add_point(Data, square->v4);
    add_segment(Data, square->v1, square->v2);
    add_segment(Data, square->v2, square->v3);
    add_segment(Data, square->v3, square->v4);
    add_segment(Data, square->v4, square->v1);
}

void add_rectangle(struct GameSharedData *Data, struct rectangleData *rectangle){
    add_point(Data, rectangle->v1);
    add_point(Data, rectangle->v2);
    add_point(Data, rectangle->v3);
    add_point(Data, rectangle->v4);
    add_segment(Data, rectangle->v1, rectangle->v2);
    add_segment(Data, rectangle->v2, rectangle->v3);
    add_segment(Data, rectangle->v3, rectangle->v4);
    add_segment(Data, rectangle->v4, rectangle->v1);
}

void get_velocities_after_two_balls_collision(float *v1x, float *v1y, float *v2x, float *v2y,
                                              double dx, double dy, float m1, float m2, float restitution){
    *v1x -= *v2x;
    *v1y -= *v2y;
    dy = VectorAngle(dx, dy);
    dx = cos(dy);
    dy = sin(dy);
    double v_into = *v1x * dx + *v1y * dy,
          v_perp = *v1y * dx - *v1x * dy,
          mc = m1 + m2;
    *v1x = v_into * ((m1 - restitution * m2) / mc);
    *v1y = *v1x * dy + *v2y + v_perp * dx;
    *v1x = *v1x * dx + *v2x - v_perp * dy;
    v_perp = (((1 + restitution) * m1) / mc) * v_into;
    *v2x += v_perp * dx;
    *v2y += v_perp * dy;

}

void separate_two_balls(float *x1, float *y1, float m1, float *x2, float *y2, float m2, double d){
    double dx = (double)*x2 - *x1,
           dy = (double)*y2 - *y1,
           ang = VectorAngle(dx, dy);
    dx = sqrt(dx * dx + dy * dy);
    if(dx < d){
        d -= dx;
        dx = d * sin(ang); //y
        d *= cos(ang); //x
        ang = m1 + m2;

        dy = m1 / ang; //m1 mass factor
        ang = m2 / ang; //m2 mass factor
        *x1 -= d * ang;
        *y1 -= dx * ang;
        *x2 += d * dy;
        *y2 += dx * dy;
    }
}

void player_get_dx_dy(struct movable_object_structure *Obj, float dt){
    #define Data ((struct playerData*)Obj->ObjectData)
    Obj->dx = Data->vx * dt;
    Obj->dy = Data->vy * dt;
    #undef Data
}

void particle_get_dx_dy(struct movable_object_structure *Obj, float dt){
    #define Data ((struct particleData*)Obj->ObjectData)
    Obj->dx = Data->vx * dt;
    Obj->dy = Data->vy * dt;
    #undef Data
}

void collide(struct GameSharedData *Data, short int who, short int with, bool with_movable, float dt){
    if(with_movable){
        Data->DeCollAngs[0] = VectorAngle(Data->Level.MovableObjects[who].dx, Data->Level.MovableObjects[who].dy);
        Data->DeCollAngs[1] = VectorAngle(Data->Level.MovableObjects[with].dx, Data->Level.MovableObjects[with].dy);
        switch(Data->Level.MovableObjects[who].Type){
            case motPLAYER://printf("player\n");
                #define WHO_PLAYER ((struct playerData*)Data->Level.MovableObjects[who].ObjectData)
                switch(Data->Level.MovableObjects[with].Type){
                    case motPLAYER://printf("with player\n");
                        #define WITH_PLAYER ((struct playerData*)Data->Level.MovableObjects[with].ObjectData)
                        separate_two_balls(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y, WHO_PLAYER->mass,
                                           &WITH_PLAYER->center.x, &WITH_PLAYER->center.y, WITH_PLAYER->mass,
                                           WHO_PLAYER->r + WITH_PLAYER->r);
                        get_velocities_after_two_balls_collision(&(WHO_PLAYER->vx), &(WHO_PLAYER->vy),
                                                                 &(WITH_PLAYER->vx), &(WITH_PLAYER->vy),
                                                                 WITH_PLAYER->center.x - WHO_PLAYER->center.x,
                                                                 WITH_PLAYER->center.y - WHO_PLAYER->center.y,
                                                                 WHO_PLAYER->mass, WITH_PLAYER->mass,
                                                                 PLAYER_TO_PLAYER_RESTITUTION);
                        player_get_dx_dy(&Data->Level.MovableObjects[who], dt);
                        player_get_dx_dy(&Data->Level.MovableObjects[with], dt);
                        #undef WITH_PLAYER
                        break;
                    case motPARTICLE://printf("with particle\n");
                        #define WITH_PARTICLE ((struct particleData*)Data->Level.MovableObjects[with].ObjectData)
                        separate_two_balls(&WHO_PLAYER->center.x, &WHO_PLAYER->center.y, WHO_PLAYER->mass,
                                           &WITH_PARTICLE->center.x, &WITH_PARTICLE->center.y, WITH_PARTICLE->mass,
                                           WHO_PLAYER->r + WITH_PARTICLE->r);
                        get_velocities_after_two_balls_collision(&(WHO_PLAYER->vx), &(WHO_PLAYER->vy),
                                                                 &(WITH_PARTICLE->vx), &(WITH_PARTICLE->vy),
                                                                 WITH_PARTICLE->center.x - WHO_PLAYER->center.x,
                                                                 WITH_PARTICLE->center.y - WHO_PLAYER->center.y,
                                                                 WHO_PLAYER->mass, WITH_PARTICLE->mass,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        player_get_dx_dy(&Data->Level.MovableObjects[who], dt);
                        particle_get_dx_dy(&Data->Level.MovableObjects[with], dt);
                        #undef WITH_PARTICLE
                        break;
                    default:
                        break;
                }
                #undef WHO_PLAYER
                break;
            case motPARTICLE://printf("particle\n");
                #define WHO_PARTICLE ((struct particleData*)Data->Level.MovableObjects[who].ObjectData)
                switch(Data->Level.MovableObjects[with].Type){
                    case motPLAYER://printf("with player\n");
                        #define WITH_PLAYER ((struct playerData*)Data->Level.MovableObjects[with].ObjectData)
                        separate_two_balls(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y, WHO_PARTICLE->mass,
                                           &WITH_PLAYER->center.x, &WITH_PLAYER->center.y, WITH_PLAYER->mass,
                                           WHO_PARTICLE->r + WITH_PLAYER->r);
                        get_velocities_after_two_balls_collision(&(WHO_PARTICLE->vx), &(WHO_PARTICLE->vy),
                                                                 &(WITH_PLAYER->vx), &(WITH_PLAYER->vy),
                                                                 WITH_PLAYER->center.x - WHO_PARTICLE->center.x,
                                                                 WITH_PLAYER->center.y - WHO_PARTICLE->center.y,
                                                                 WHO_PARTICLE->mass, WITH_PLAYER->mass,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        particle_get_dx_dy(&Data->Level.MovableObjects[who], dt);
                        player_get_dx_dy(&Data->Level.MovableObjects[with], dt);
                        #undef WITH_PLAYER
                        break;
                    case motPARTICLE://printf("with particle\n");
                        #define WITH_PARTICLE ((struct particleData*)Data->Level.MovableObjects[with].ObjectData)
                        separate_two_balls(&WHO_PARTICLE->center.x, &WHO_PARTICLE->center.y, WHO_PARTICLE->mass,
                                           &WITH_PARTICLE->center.x, &WITH_PARTICLE->center.y, WITH_PARTICLE->mass,
                                           WHO_PARTICLE->r + WITH_PARTICLE->r);
                        get_velocities_after_two_balls_collision(&(WHO_PARTICLE->vx), &(WHO_PARTICLE->vy),
                                                                 &(WITH_PARTICLE->vx), &(WITH_PARTICLE->vy),
                                                                 WITH_PARTICLE->center.x - WHO_PARTICLE->center.x,
                                                                 WITH_PARTICLE->center.y - WHO_PARTICLE->center.y,
                                                                 WHO_PARTICLE->mass, WITH_PARTICLE->mass,
                                                                 PLAYER_TO_PARTICLE_RESTITUTION);
                        particle_get_dx_dy(&Data->Level.MovableObjects[who], dt);
                        particle_get_dx_dy(&Data->Level.MovableObjects[with], dt);
                        #undef WITH_PARTICLE
                        break;
                    default:
                        break;
                }
                #undef WHO_PARTICLE
                break;
            default:
                break;
        }
        Data->DeCollAngs[2] = VectorAngle(Data->Level.MovableObjects[who].dx, Data->Level.MovableObjects[who].dy);
        Data->DeCollAngs[3] = VectorAngle(Data->Level.MovableObjects[with].dx, Data->Level.MovableObjects[with].dy);
    }else{
        switch(Data->Level.MovableObjects[who].Type){
            case motPLAYER:
            case motPARTICLE:
            default:
                break;
        }
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
    return squareEquation(Data->r, fi - Data->ang);
    #undef Data
}

float rCircle(void *ObjectData, float fi){
    return ((struct circleData*)ObjectData)->r;
}

float rPlayer(void *ObjectData, float fi){
    return 40;
}

float rRectangle(void *ObjectData, float fi){
    #define Data ((struct rectangleData*)ObjectData)
    return rectangleEquation(Data->r, fi - Data->ang, Data->fi0, Data->fi02, Data->wsp1, Data->wsp2);
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
    draw_tetragon(Data->v1, Data->v2, Data->v3, Data->v4, Data->color);
    #undef Data
}

void draw_circle(void *ObjectData){
    #define Data ((struct circleData*)ObjectData)
    al_draw_filled_circle(Data->center.x, Data->center.y, Data->r, Data->color);
    #undef Data
}

void draw_rectangle(void *ObjectData){
    #define Data ((struct rectangleData*)ObjectData)
    draw_tetragon(Data->v1, Data->v2, Data->v3, Data->v4, Data->color);
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
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, PLAYER_RADIUS, al_map_rgb(255, 255, 255));
    al_draw_filled_circle(Data->center.x + dx + PLAYER_RADIUS * 0.5 * cos(Data->ang),
                          Data->center.y + dy + PLAYER_RADIUS * 0.5 * sin(Data->ang),
                          PLAYER_RADIUS * 0.2, al_map_rgb(0, 0, 0));
    #undef Data
}

void draw_particle(void *ObjectData, float dx, float dy){
    #define Data ((struct particleData*)ObjectData)
    al_draw_filled_circle(Data->center.x + dx, Data->center.y + dy, Data->r, al_map_rgb(255, 0, 255));
    #undef Data
}

void draw_door(void *ObjectData, float dx, float dy){
    #define Data ((struct doorData*)ObjectData)
    struct point dv1 = *Data->v1,
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

void draw_switch(void *ObjectData, float dx, float dy){
    #define Data ((struct switchData*)ObjectData)
    struct point dv1 = *Data->v1,
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

/**
    Constructors
    Primitives if needed must be added
    explicitly outside constructors
    */

void construct_circle(struct fixed_object_structure *Object){
    #define Data ((struct circleData*)ObjectData)
    Object->draw = draw_circle;
    Object->r = rCircle;
    #undef Data
}

void construct_square(struct fixed_object_structure *Object){
    #define Data ((struct squareData*)Object->ObjectData)
    Object->draw = draw_square;
    Object->r = rSquare;
    Data->r = Data->bok * SQRT2 / 2;
    float fi = PI4 + Data->ang;
    Data->v1 = (struct point*)malloc(sizeof(struct point));
    Data->v1->x = Data->center.x + Data->r * cos(fi);
    Data->v1->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v2 = (struct point*)malloc(sizeof(struct point));
    Data->v2->x = Data->center.x + Data->r * cos(fi);
    Data->v2->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v3 = (struct point*)malloc(sizeof(struct point));
    Data->v3->x = Data->center.x + Data->r * cos(fi);
    Data->v3->y = Data->center.y + Data->r * sin(fi);
    fi += PIpol;
    Data->v4 = (struct point*)malloc(sizeof(struct point));
    Data->v4->x = Data->center.x + Data->r * cos(fi);
    Data->v4->y = Data->center.y + Data->r * sin(fi);
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

    Data->r = Data->b / (sin(Data->fi02) * 2);

    fi = PIpol - Data->ang + Data->fi02;
    Data->v1 = (struct point*)malloc(sizeof(struct point));
    Data->v1->x = Data->center.x + Data->r * cos(fi);
    Data->v1->y = Data->center.y + Data->r * sin(fi);
    fi += PI;
    Data->v3 = (struct point*)malloc(sizeof(struct point));
    Data->v3->x = Data->center.x + Data->r * cos(fi);
    Data->v3->y = Data->center.y + Data->r * sin(fi);
    fi = PIpol - Data->ang - Data->fi02;
    Data->v2 = (struct point*)malloc(sizeof(struct point));
    Data->v2->x = Data->center.x + Data->r * cos(fi);
    Data->v2->y = Data->center.y + Data->r * sin(fi);
    fi += PI;
    Data->v4 = (struct point*)malloc(sizeof(struct point));
    Data->v4->x = Data->center.x + Data->r * cos(fi);
    Data->v4->y = Data->center.y + Data->r * sin(fi);
    #undef Data
}

void construct_door(struct movable_object_structure *Object){
    #define Data ((struct doorData*)(Object->ObjectData))
    construct_rectangle((struct fixed_object_structure*)Object);

    Object->draw = draw_door;
    Data->vx = 0;
    Data->vy = 0;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, Object->zones);

    #undef Data
}

void construct_switch(struct movable_object_structure *Object){
    #define Data ((struct switchData*)(Object->ObjectData))
    construct_rectangle((struct fixed_object_structure*)Object);

    Object->draw = draw_switch;
    Data->vx = 0;
    Data->vy = 0;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, Object->zones);

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
    Data->charge = 0;
    Data->r = PLAYER_RADIUS;
    Data->r0 = PLAYER_RADIUS;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, Object->zones);
    #undef Data
}

void construct_particle(struct movable_object_structure *Object){
    #define Data ((struct particleData*)(Object->ObjectData))
    Object->draw = draw_particle;
    Object->r = rCircle;
    Data->vx = 0;
    Data->vy = 0;
    Data->surface_field = Data->r * Data->r * PI;
    get_zone_for_object(Data->center.x, Data->center.y, 0, 0, Data->r, Object->zones);
    #undef Data
}

void construct_movable(struct GameSharedData *Data, struct movable_object_structure *Object){
    Object->dx = 0;
    Object->dy = 0;
    Object->colls_with_mov.nil = (coll_node*)malloc(sizeof(coll_node));
    Object->colls_with_mov.nil->color = BLACK;
    Object->colls_with_mov.nil->key.time = EMPTY_COLLISION_TIME;
    Object->colls_with_mov.nil->key.who = -10;
    Object->colls_with_mov.nil->key.with = -20;
    Object->colls_with_mov.nil->key.with_movable = false;
    Object->colls_with_mov.nil->left = Object->colls_with_mov.nil;
    Object->colls_with_mov.nil->right = Object->colls_with_mov.nil;
    Object->colls_with_mov.root = Object->colls_with_mov.nil;
    Object->next_collision = &Object->colls_with_mov.nil->key;

    switch(Object->Type){
        case motPLAYER:
            construct_player(Object);
            break;
        case motPARTICLE:
            construct_particle(Object);
            break;
        case motDOOR:
            construct_door(Object);
            break;
        case motSWITCH:
            construct_switch(Object);
            break;
    }
}
/**
    Arrays interface
    */

void add_movable_object(struct GameSharedData *Data, enum movable_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.number_of_movable_objects >= Data->Level.boundry_movable){
        Data->Level.boundry_movable *= 2;
        Data->Level.MovableObjects = (struct movable_object_structure*)realloc((void*)Data->Level.MovableObjects, sizeof(struct movable_object_structure) * Data->Level.boundry_movable);
    }
    Data->Level.MovableObjects[Data->Level.number_of_movable_objects].Type = NewObjectType;
    Data->Level.MovableObjects[Data->Level.number_of_movable_objects].ObjectData = NewObjectData;
    Data->Level.number_of_movable_objects += 1;
}

void add_fixed_object(struct GameSharedData *Data, enum fixed_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.number_of_fixed_objects >= Data->Level.boundry_fixed){
        Data->Level.boundry_fixed *= 2;
        Data->Level.FixedObjects = (struct fixed_object_structure*)realloc((void*)Data->Level.FixedObjects, sizeof(struct fixed_object_structure) * Data->Level.boundry_fixed);
    }
    Data->Level.FixedObjects[Data->Level.number_of_fixed_objects].Type = NewObjectType;
    Data->Level.FixedObjects[Data->Level.number_of_fixed_objects].ObjectData = NewObjectData;
    Data->Level.number_of_fixed_objects += 1;
}

void add_primitive_object(struct GameSharedData *Data, enum primitive_object_type NewObjectType, void* NewObjectData){
    if(Data->Level.number_of_primitive_objects >= Data->Level.boundry_primitive){
        Data->Level.boundry_primitive *= 2;
        Data->Level.PrimitiveObjects = (struct primitive_object_structure*)realloc((void*)Data->Level.PrimitiveObjects, sizeof(struct primitive_object_structure) * Data->Level.boundry_primitive);
    }
    Data->Level.PrimitiveObjects[Data->Level.number_of_primitive_objects].Type = NewObjectType;
    Data->Level.PrimitiveObjects[Data->Level.number_of_primitive_objects].ObjectData = NewObjectData;
    Data->Level.number_of_primitive_objects += 1;
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

void delete_primitive_object(struct primitive_object_structure *Object){
    switch(Object->Type){
        case potPOINT:
            free((struct point*)Object->ObjectData);
            break;
        case potSEGMENT:
            free((struct segment*)Object->ObjectData);
            break;
        case potCIRCLE:
            free((struct circle*)Object->ObjectData);
            break;
    }
}

void clear_primitive_object_list(struct GameSharedData *Data){
    int i;
    for(i = 0; i < Data->Level.number_of_primitive_objects; ++i){
        delete_primitive_object(&Data->Level.PrimitiveObjects[i]);
    }
    Data->Level.number_of_primitive_objects = 0;
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
    width -= 1;
    height -= 1;
    source_x = (source_x - 1) / width;
    source_y = (source_y - 1) / height;

	for (y = 0; y <= height; ++y) {
		pixy = (float)y * source_y;
		pixy_f = floor(pixy);
		for (x = 0; x <= width; ++x) {
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


#ifdef TESTS
extern void RunAllTests(void);

int main(){
    printf("Running in test mode\n\n");
    RunAllTests();
    return 0;
}

#else
int main(){

    int i, j;

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

    al_set_new_display_flags(ALLEGRO_FULLSCREEN); //  ALLEGRO_WINDOWED
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
    Data.DrawTimer = al_create_timer(1.0 / MAX_FPS);
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

    Data.Debug = false;
    Data.DeFont = NULL;
    Data.DeBuffer[0] = '\0';
    Data.DeFont = al_load_ttf_font("cour.ttf", 16, 0);
    for(i = 0; i < 4; ++i){
        Data.DeCollAngs[i] = 0;
    }

    Data.IterationFinished = true;
    Data.CloseLevel = false;
    Data.SpecialMainCall = false;

    Data.Level.LevelNumber = 0;
    Data.Level.number_of_movable_objects = 0;
    Data.Level.number_of_fixed_objects = 0;
    Data.Level.number_of_primitive_objects = 0;
    Data.Level.boundry_movable = INITIAL_BOUNDRY_MOVABLE;
    Data.Level.boundry_fixed = INITIAL_BOUNDRY_FIXED;
    Data.Level.boundry_primitive = INITIAL_BOUNDRY_PRIMITIVE;
    Data.Level.MovableObjects =     (struct movable_object_structure*)malloc(sizeof(struct movable_object_structure)     * Data.Level.boundry_movable);
    Data.Level.FixedObjects =         (struct fixed_object_structure*)malloc(sizeof(struct fixed_object_structure)       * Data.Level.boundry_fixed);
    Data.Level.PrimitiveObjects = (struct primitive_object_structure*)malloc(sizeof(struct primitive_object_structure)   * Data.Level.boundry_primitive);
    Data.Level.Background = NULL;
    Data.Level.ScaledBackground = NULL;
    Data.Level.Acc = NULL;

    construct_heap(&Data.Level.collision_queue, INITIAL_OBJECT_COLLISION_QUEUE_SIZE);

    for(i = 0; i < ZONE_FACTOR; ++i){
        for(j = 0; j < ZONE_FACTOR; ++j){
            Data.Level.zones[i][j].movable.nil = (RB_node*)malloc(sizeof(RB_node));
            Data.Level.zones[i][j].movable.nil->color = BLACK;
            Data.Level.zones[i][j].movable.nil->left = Data.Level.zones[i][j].movable.nil;
            Data.Level.zones[i][j].movable.nil->right = Data.Level.zones[i][j].movable.nil;
            Data.Level.zones[i][j].movable.nil->key = -10;
            Data.Level.zones[i][j].movable.root = Data.Level.zones[i][j].movable.nil;
            Data.Level.zones[i][j].number_of_primitives = 0;
            Data.Level.zones[i][j].allocated = INITIAL_PRIMITIVES_PER_ZONE;
            Data.Level.zones[i][j].primitives = (short int*)malloc(sizeof(short int) * INITIAL_PRIMITIVES_PER_ZONE);
        }
    }

    Data.Level.dirty_tree.nil = (coll_node*)malloc(sizeof(coll_node));
    Data.Level.dirty_tree.nil->color = BLACK;
    Data.Level.dirty_tree.nil->key.time = EMPTY_COLLISION_TIME;
    Data.Level.dirty_tree.nil->key.who = -10;
    Data.Level.dirty_tree.nil->key.with = -20;
    Data.Level.dirty_tree.nil->key.with_movable = false;
    Data.Level.dirty_tree.nil->left = Data.Level.dirty_tree.nil;
    Data.Level.dirty_tree.nil->right = Data.Level.dirty_tree.nil;
    Data.Level.dirty_tree.root = Data.Level.dirty_tree.nil;

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

    if(Data.Level.Acc){
        free(Data.Level.Acc);
    }
    clear_movable_object_list(&Data);
    free(Data.Level.MovableObjects);
    clear_fixed_object_list(&Data);
    free(Data.Level.FixedObjects);

    return 0;
}

#endif
