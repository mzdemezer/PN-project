#include <stdio.h>
#include "CuTest.h"
#include "../structures/fastreadset.h"

void test_fast_read_set01(CuTest *tc){
    short int i;
    fast_read_set set;
    printf("\nFast-read-set test#01 - elements 0-19: set_mark\nmarked: 0, 2, 5, 12, 19\n\tarray read directly:\n");
    construct_fast_read_set(&set, 20);
    set_mark(&set, 5);
    set_mark(&set, 0);
    set_mark(&set, 2);
    set_mark(&set, 12);
    set_mark(&set, 19);
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)set.array[i]);
    }
    printf("\n");
    RB_display_keys_in_order(&set.tree);
    destroy_fast_read_set(&set);
}

void test_fast_read_set02(CuTest *tc){
    short int i;
    fast_read_set set;
    printf("\nFast-read-set test#02 - elements 0-19: is_marked\nmarked: 3, 4, 8, 14, 18\n\tarray read by is_marked:\n");
    construct_fast_read_set(&set, 20);
    set_mark(&set, 18);
    set_mark(&set, 3);
    set_mark(&set, 8);
    set_mark(&set, 4);
    set_mark(&set, 14);
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)is_marked(&set, i));
    }
    printf("\n");
    RB_display_keys_in_order(&set.tree);
    destroy_fast_read_set(&set);
}

void test_fast_read_set03(CuTest *tc){
    short int i;
    fast_read_set set;
    printf("\nFast-read-set test#03 - elements 0-19: reset\nmarked: 0-9, 12, 15, 17, 19\narray read by is_marked:\n");
    construct_fast_read_set(&set, 20);
    for(i = 0; i < 10; ++i){
        set_mark(&set, i);
    }
    set_mark(&set, 12);
    set_mark(&set, 19);
    set_mark(&set, 17);
    set_mark(&set, 15);
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)is_marked(&set, i));
    }
    printf("\n");
    RB_display_keys_in_order(&set.tree);


    reset_marks(&set);


    printf("\tNow all cells should be reset:\n");
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)is_marked(&set, i));
    }
    printf("\n");
    RB_display_keys_in_order(&set.tree);
    destroy_fast_read_set(&set);
}

void test_fast_read_set04(CuTest *tc){
    short int i, j;
    fast_read_set set;
    printf("\nFast-read-set test#04 - elements 0-19: unique_insert\nmarked: 0-14 repeated 50 times\narray read by is_marked:\n");
    construct_fast_read_set(&set, 20);
    for(j = 0; j < 50; ++j){
        for(i = 0; i < 15; ++i){
            set_mark(&set, i);
        }
    }
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)is_marked(&set, i));
    }
    printf("\n\tThere should be no repetitions in the tree;\t");
    RB_display_keys_in_order(&set.tree);

    reset_marks(&set);
    printf("Now all cells should be reset:\n");
    for(i = 0; i < 20; ++i){
        printf("%d ", (int)is_marked(&set, i));
    }
    printf("\n");
    RB_display_keys_in_order(&set.tree);

    destroy_fast_read_set(&set);
}

CuSuite* fastreadsetGetSuite(void){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_fast_read_set01);
    SUITE_ADD_TEST(suite, test_fast_read_set02);
    SUITE_ADD_TEST(suite, test_fast_read_set03);
    SUITE_ADD_TEST(suite, test_fast_read_set04);

    return suite;
}
