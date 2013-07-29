#include <stdlib.h>
#include "fastreadset.h"

/**
    Private methods
    */
void clear_mark_from_tree(RB_node *node, RB_node *nil, bool *array);

/**
    Code
    */

void clear_mark_from_tree(RB_node *node, RB_node *nil, bool *array){
    if(node != nil){
        clear_mark_from_tree(node->left, nil, array);
        clear_mark_from_tree(node->right, nil, array);
        array[node->key] = false;
        free(node);
    }
}

void construct_fast_read_set(fast_read_set *set, short int number_of_elements){
    set->array = (bool*)malloc(sizeof(bool) * number_of_elements);
    for(number_of_elements -= 1; number_of_elements >= 0; number_of_elements -= 1){
        set->array[number_of_elements] = false;
    }
    RB_construct_tree(&set->tree);
}

bool is_marked(fast_read_set *set, short int key){
    return set->array[key];
}

void set_mark(fast_read_set *set, short int key){
    set->array[key] = true;
    unique_insert_node(&set->tree, key);
}

void reset_marks(fast_read_set *set){
    clear_mark_from_tree(set->tree.root, set->tree.nil, set->array);
    set->tree.root = set->tree.nil;
}

void destroy_fast_read_set(fast_read_set *set){
    free(set->array);
    RB_destroy_tree(&set->tree);
}
