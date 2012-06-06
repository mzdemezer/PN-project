#ifndef _INCLUDE_COLLSTRUCTURES_H
#define _INCLUDE_COLLSTRUCTURES_H
#include <stdbool.h>

#define EMPTY_COLLISION_TIME 10
struct collision_data{
    double time;
    short int who, with;
    bool with_movable;
};

#define LESS -1
#define MORE 1
#define EQUAL 0
short int coll_comp(struct collision_data *a, struct collision_data *b);
short int coll_rev_comp(struct collision_data *a, struct collision_data *b);



/**
    Heap for collisions
    */
struct collision_heap{
    int allocated, length;
    struct collision_data *heap;
};

/**
    Public methods
    */
void construct_heap(struct collision_heap* heap, int size);
void destroy_heap(struct collision_heap* heap);
void build_heap(struct collision_heap* heap);
struct collision_data pop_min(struct collision_heap* heap);
void heap_insert(struct collision_heap* heap, struct collision_data *collision);
void clear_heap(struct collision_heap* heap);



/**
    Red-Black tree for collisions
    */
typedef struct coll_node{
    struct collision_data key;
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
void coll_insert_node(coll_tree *tree, struct collision_data *key);
bool coll_delete_node(coll_tree *tree, struct collision_data *key);
void coll_clear_tree(coll_tree *tree);
void coll_in_order(coll_node *root, coll_node *nil);
coll_node* coll_get_minimum(coll_node *node, coll_node *nil);

#endif
