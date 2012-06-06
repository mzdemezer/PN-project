#include <stdio.h>
#include <stdlib.h>
#include "collstructures.h"

#define RED true
#define BLACK false

/**
    Collision structure comparisons
    reverse compare is when the collision
    data is in different trees - who and with
    fields have swapped values then
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

/**
    Heap
    */

/**
    Private methods
    */
#define heap_left(i) (i << 1)
#define heap_right(i) ((i << 1 ) | 1)
#define heap_parent(i) (i >> 1)
void heapify(struct collision_heap* heap, short int i);

/**
    Code
    */
void construct_heap(struct collision_heap* heap, int size){
    heap->allocated = size;
    heap->length = 0;
    heap->heap = (struct collision_data*)malloc(sizeof(struct collision_data) * (size + 1));
}

void destroy_heap(struct collision_heap* heap){
    free(heap->heap);
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
    Red-Black Tree for collisions
    */
/**
    Private methods
    */
coll_node* coll_get_node(coll_tree *tree, struct collision_data *key);
coll_node* coll_get_successor(coll_node *node, coll_node *nil);
inline bool coll_is_left(coll_node *node);
void coll_delete_fixup(coll_tree *tree, coll_node *node);
void coll_clear_nodes(coll_node *node, coll_node *nil);
void coll_rotate_left(coll_tree *tree, coll_node *node);
void coll_rotate_right(coll_tree *tree, coll_node *node);

/**
    Code
    */

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

void coll_destroy_tree(coll_tree *tree){
    if(tree->root != tree->nil){
        coll_clear_nodes(tree->root, tree->nil);
    }
    free(tree->nil);
}

void coll_construct_tree(coll_tree *tree){
    tree->nil = (coll_node*)malloc(sizeof(coll_node));
    tree->nil->color = BLACK;
    tree->nil->key.time = EMPTY_COLLISION_TIME;
    tree->nil->key.who = -10;
    tree->nil->key.with = -20;
    tree->nil->key.with_movable = false;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->root = tree->nil;
}

inline bool coll_is_left(coll_node *node){
    return node == node->parent->left;
}

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

void coll_in_order(coll_node *root, coll_node *nil){
    if(root != nil){
        coll_in_order(root->left, nil);

        printf("%f\n", root->key.time);

        coll_in_order(root->right, nil);
    }
}
