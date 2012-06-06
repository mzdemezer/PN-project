#include <stdio.h>
#include <stdlib.h>
#include "RBtree.h"

#define RED true
#define BLACK false

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

void RB_insert_fixup(RB_tree *tree, RB_node *node){
    node->color = RED;
    RB_node *last;
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
    }else{
        if(key < last->key){
            last->left = node;
        }else{
            last->right = node;
        }
        RB_insert_fixup(tree, node);
    }
    tree->root->color = BLACK;
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

void RB_destroy_tree(RB_tree *tree){
    if(tree->root != tree->nil){
        clear_nodes(tree->root, tree->nil);
    }
    free(tree->nil);
}

void RB_construct_tree(RB_tree *tree){
    tree->nil = (RB_node*)malloc(sizeof(RB_node));
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->key = -10;
    tree->root = tree->nil;
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

void in_order(RB_node *node, RB_node *nil){
    if(node != nil){
        in_order(node->left, nil);

        printf("%hd\t", node->key);

        in_order(node->right, nil);
    }
}

void RB_display_keys_in_order(RB_tree *tree){
    printf("In order:\n");
    in_order(tree->root, tree->nil);
    printf("\n");
}
