#ifndef _INCLUDE_RBTREE_H
#define _INCLUDE_RBTREE_H
#include <stdbool.h>

/**
    Red-Black Tree with simple keys
    */

typedef struct RB_node{
    short int key;
    bool color;
    struct RB_node *left, *right, *parent;
}RB_node;

typedef struct RB_tree{
    RB_node *root, *nil;
}RB_tree;

/**
    Public methods
    */
void RB_construct_tree(RB_tree *tree);
void RB_destroy_tree(RB_tree *tree);

void insert_node(RB_tree *tree, short int key);
void unique_insert_node(RB_tree *tree, short int key);
void delete_node(RB_tree *tree, short int key);
void clear_tree(RB_tree *tree);
RB_node* get_minimum(RB_node *node, RB_node *nil);
void RB_display_keys_in_order(RB_tree *tree);

#endif
