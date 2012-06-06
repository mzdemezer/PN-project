#ifndef _INCLUDE_FASTREADSET_H
#define _INCLUDE_FASTREADSET_H
#include <stdbool.h>
#include "../structures.h"

/**
    Fast read set - if you need to check
    a key in array of booleans it is done
    in O(1), but inserting and deleting single
    node is O(log(k)), where k is the number
    of marks set. BUT Reseting is O(k) and not O(n)!
    This should be perfect for large sets with small amount
    of marks, that have to be initialized every
    time they need to be reused - as reinitilization
    is simply reset.
    */
typedef struct fast_read_set{
    bool *array;
    RB_tree tree;
}fast_read_set;

/**
    Public methods
    */
void construct_fast_read_set(fast_read_set *set, short int number_of_elements);
bool is_marked(fast_read_set *set, short int key);
void set_mark(fast_read_set *set, short int key);
void reset_marks(fast_read_set *set);
void destroy_fast_read_set(fast_read_set *set);

#endif
