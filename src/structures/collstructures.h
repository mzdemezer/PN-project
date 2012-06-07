#ifndef _INCLUDE_COLLSTRUCTURES_H
#define _INCLUDE_COLLSTRUCTURES_H
#include <stdbool.h>

#define EMPTY_COLLISION_TIME 10
typedef struct collision_data{
    double time;
    short int who, with;
    bool with_movable;
}collision_data;

#define LESS -1
#define MORE 1
#define EQUAL 0
short int coll_comp(collision_data *a, collision_data *b);
short int coll_rev_comp(collision_data *a, collision_data *b);



/**
    Heap for collisions
    */
typedef struct collision_heap{
    int allocated, length;
    collision_data *heap;
}collision_heap;

/**
    Public methods
    */
#define heap_left(i) (i << 1)
#define heap_right(i) ((i << 1 ) | 1)
#define heap_parent(i) (i >> 1)
void construct_heap(collision_heap* heap, int size);
void destroy_heap(collision_heap* heap);
void build_heap(collision_heap* heap);
collision_data pop_min(collision_heap* heap);
void heap_insert(collision_heap* heap, collision_data *collision);
void heap_add(collision_heap *heap, collision_data *collision);
void clear_heap(collision_heap* heap);



/**
    Red-Black tree for collisions
    */
typedef struct coll_node{
    collision_data key;
    bool color;
    short int counter;
    struct coll_node *left, *right, *parent;
}coll_node;

typedef struct coll_tree{
    coll_node *root, *nil;
}coll_tree;

/**
    Public methods
    */
void coll_construct_tree(coll_tree *tree);
void coll_destroy_tree(coll_tree *tree);
void coll_insert_node(coll_tree *tree, collision_data *key);
bool coll_delete_node(coll_tree *tree, collision_data *key);
void coll_clear_tree(coll_tree *tree);
void coll_in_order(coll_node *root, coll_node *nil);
coll_node* coll_get_minimum(coll_node *node, coll_node *nil);

#endif
