#ifndef _INCLUDE_RBTREE_H
#define _INCLUDE_RBTREE_H
#include <stdbool.h>

typedef struct RB_node{
    short int key;
    bool color;
    struct RB_node *left, *right, *parent;
}RB_node;

typedef struct RB_tree{
    RB_node *root, *nil;
}RB_tree;

void RB_construct_tree(RB_tree *tree);
void RB_destroy_tree(RB_tree *tree);

RB_node* get_node(RB_tree *tree, short int key);
RB_node* get_minimum(RB_node *node, RB_node *nil);
RB_node* get_successor(RB_node *node, RB_node *nil);
inline bool is_left(RB_node *node);
void rotate_left(RB_tree *tree, RB_node *node);
void rotate_right(RB_tree *tree, RB_node *node);
void insert_node(RB_tree *tree, short int key);
void RB_insert_fixup(RB_tree *tree, RB_node *node);
void delete_node(RB_tree *tree, short int key);
void RB_delete_fixup(RB_tree *tree, RB_node *node);
void clear_nodes(RB_node *node, RB_node *nil);
void clear_tree(RB_tree *tree);

void in_order(RB_node *node, RB_node *nil);
void RB_display_keys_in_order(RB_tree *tree);

#endif
