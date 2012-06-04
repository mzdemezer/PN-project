#include "CuTest.h"
#include "../main.h"
#include <stdio.h>
#include <math.h>

void testabsTrue1(CuTest *tc){
    CuAssertTrue(tc, abs(5) == 5);
}

void testabsTrue2(CuTest *tc){
    CuAssertTrue(tc, abs(-5) == 5);
}

void testabsTrue3(CuTest *tc){
    CuAssertTrue(tc, abs(995) == 995);
}

void testabsTrue4(CuTest *tc){
    CuAssertTrue(tc, abs(-234) == 234);
}

void testabsTrue5(CuTest *tc){
    CuAssertTrue(tc, abs(0) == 0);
}


void testrzadTrue01(CuTest *tc){
    CuAssertTrue(tc, rzad(0) == 1);
}

void testrzadTrue02(CuTest *tc){
    CuAssertTrue(tc, rzad(5) == 1);
}

void testrzadTrue03(CuTest *tc){
    CuAssertTrue(tc, rzad(9) == 1);
}


void testrzadTrue04(CuTest *tc){
    CuAssertTrue(tc, rzad(10) == 2);
}

void testrzadTrue05(CuTest *tc){
    CuAssertTrue(tc, rzad(50) == 2);
}

void testrzadTrue06(CuTest *tc){
    CuAssertTrue(tc, rzad(99) == 2);
}


void testrzadTrue07(CuTest *tc){
    CuAssertTrue(tc, rzad(100) == 3);
}

void testrzadTrue08(CuTest *tc){
    CuAssertTrue(tc, rzad(500) == 3);
}

void testrzadTrue09(CuTest *tc){
    CuAssertTrue(tc, rzad(999) == 3);
}


void testrzadTrue10(CuTest *tc){
    CuAssertTrue(tc, rzad(1000) == 4);
}

void testrzadTrue11(CuTest *tc){
    CuAssertTrue(tc, rzad(5000) == 4);
}

void testrzadTrue12(CuTest *tc){
    CuAssertTrue(tc, rzad(9999) == 4);
}


void testrzadTrue13(CuTest *tc){
    CuAssertTrue(tc, rzad(10000) == 5);
}

void testrzadTrue14(CuTest *tc){
    CuAssertTrue(tc, rzad(50000) == 5);
}

void testrzadTrue15(CuTest *tc){
    CuAssertTrue(tc, rzad(99999) == 5);
}


void testrzadTrue16(CuTest *tc){
    CuAssertTrue(tc, rzad(100000) == 6);
}

void testrzadTrue17(CuTest *tc){
    CuAssertTrue(tc, rzad(500000) == 6);
}

void testrzadTrue18(CuTest *tc){
    CuAssertTrue(tc, rzad(999999) == 6);
}


void testrzadTrue19(CuTest *tc){
    CuAssertTrue(tc, rzad(1000000) == 7);
}

void testrzadTrue20(CuTest *tc){
    CuAssertTrue(tc, rzad(5000000) == 7);
}

void testrzadTrue21(CuTest *tc){
    CuAssertTrue(tc, rzad(9999999) == 7);
}


void testrzadTrue22(CuTest *tc){
    CuAssertTrue(tc, rzad(10000000) == 8);
}

void testrzadTrue23(CuTest *tc){
    CuAssertTrue(tc, rzad(50000000) == 8);
}

void testrzadTrue24(CuTest *tc){
    CuAssertTrue(tc, rzad(99999999) == 8);
}


void testrzadTrue25(CuTest *tc){
    CuAssertTrue(tc, rzad(100000000) == 9);
}


void testrzadTrue26(CuTest *tc){
    CuAssertTrue(tc, rzad(-1) == 1);
}

void testrzadTrue27(CuTest *tc){
    CuAssertTrue(tc, rzad(-10) == 2);
}

void testrzadTrue28(CuTest *tc){
    CuAssertTrue(tc, rzad(-100) == 3);
}

void testrzadTrue29(CuTest *tc){
    CuAssertTrue(tc, rzad(-1000) == 4);
}

void testrzadTrue30(CuTest *tc){
    CuAssertTrue(tc, rzad(-10000) == 5);
}

void testrzadTrue31(CuTest *tc){
    CuAssertTrue(tc, rzad(-100000) == 6);
}

void testrzadTrue32(CuTest *tc){
    CuAssertTrue(tc, rzad(-1000000) == 7);
}

void testrzadTrue33(CuTest *tc){
    CuAssertTrue(tc, rzad(-10000000) == 8);
}

void testrzadTrue34(CuTest *tc){
    CuAssertTrue(tc, rzad(-100000000) == 9);
}

void testnormTrue01(CuTest *tc){
    CuAssertTrue(tc, norm(0) == 0);
}

void testnormTrue02(CuTest *tc){
    CuAssertTrue(tc, norm(-dwaPI) == 0);
}

void testnormTrue03(CuTest *tc){
    CuAssertTrue(tc, abs(norm(dwaPI)) < eps);
}

void testnormTrue04(CuTest *tc){
    CuAssertTrue(tc, abs(norm(dwaPI * 2)) < eps);
}

void testnormTrue05(CuTest *tc){
    CuAssertTrue(tc, abs(norm(dwaPI * 16 + PI4) - PI4) < eps);
}

void testnormTrue06(CuTest *tc){
    CuAssertTrue(tc, abs(norm(dwaPI * 300 + PI34) - PI34) < eps);
}

void testnormTrue07(CuTest *tc){
    CuAssertTrue(tc, abs(norm(-dwaPI * 16 + PI4) - PI4) < eps);
}

void testnormTrue08(CuTest *tc){
    CuAssertTrue(tc, abs(norm(-dwaPI * 300 + PI34) - PI34) < eps);
}


void testRBtree_in_order(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    int i;
    for(i = 31; i >= 0; --i){
        insert_node(&tree, i);
    }
    //test
    printf("\nRed-Black Tree simple insert, in_order test\nInserted nodes 0-31, in_order:\n");
    in_order(tree.root, tree.nil);
    printf("\n");

    clear_tree(&tree);
    free(tree.nil);
}

void testRBtree_delete(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    int i;
    for(i = 63; i > 0; --i){
        insert_node(&tree, (short int)i);
    }
    for(i = 0; i < 63; i += 2){
        delete_node(&tree, (short int)i);
    }
    //test
    printf("\nRed-Black Tree delete test\nInserted nodes 1-63, even numbers from 0 to 62 removed\n(for 0 no error), ");
    RB_display_keys_in_order(&tree);

    clear_tree(&tree);
    free(tree.nil);
}

void testRBtree_delete_is_root_nil(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    insert_node(&tree, 5);
    delete_node(&tree, 5);
    CuAssertTrue(tc, tree.root == tree.nil);

    clear_tree(&tree);
    free(tree.nil);
}

void testRBtree_height(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    int i;
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    //test

    void seek(RB_node *node, RB_node *nil, int *params, int height){
        if(node->left == nil && node->right == nil){
            if(height < params[0]){
                params[0] = height;
            }
            if(height > params[1]){
                params[1] = height;
            }
        }else{
            if(node->left != nil){
                seek(node->left, nil, params, height + 1);
            }
            if(node->right != nil){
                seek(node->right, nil, params, height + 1);
            }
        }
    }

    void DFS(RB_tree* tree, int *params){
        if(tree->root == tree->nil){
            params[0] = 0;
            params[1] = 1;
        }else{
            seek(tree->root, tree->nil, params, 0);
        }
    };
    int min_max[2];
    min_max[0] = 2000000;
    min_max[1] = 0;

    DFS(&tree, min_max);
    printf("\nRed-Black Tree height test\nfor 32000 nodes, min >= dif:\nmin: %d\nmax: %d\ndif: %d\n", min_max[0] + 1, min_max[1] + 1, min_max[1] - min_max[0]);
    CuAssertTrue(tc, 2 * min_max[0] - min_max[1] >= 0);
    clear_tree(&tree);
    free(tree.nil);
}

void testRBtree_massive_delete(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    int i;
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    for(i = 0; i <= 32000; ++i){
        delete_node(&tree, i);
    }

    CuAssertTrue(tc, tree.root == tree.nil);
    free(tree.nil);
}

void testRBtree_massive_insert_and_delete(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10;

    tree.root = tree.nil;
    int i;
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    for(i = 0; i <= 32000; ++i){
        delete_node(&tree, i);
    }
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    for(i = 0; i <= 32000; ++i){
        delete_node(&tree, i);
    }
    insert_node(&tree, 5);

    CuAssertTrue(tc, tree.root->key == 5);
    free(tree.nil);
}

void testRBtree_massive_absurd_delete(CuTest *tc){
    RB_tree tree;
    tree.nil = (RB_node*)malloc(sizeof(RB_node));
    tree.nil->color = BLACK;
    tree.nil->key = -10000;

    tree.root = tree.nil;
    int i;
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    for(i = 0; i <= 32000; ++i){
        delete_node(&tree, i);
    }
    for(i = 32000; i >= 0; --i){
        insert_node(&tree, i);
    }
    for(i = -9999; i <= 32000; ++i){
        delete_node(&tree, i);
    }
    insert_node(&tree, 100);

    CuAssertTrue(tc, tree.root->key == 100);
    free(tree.nil);
}

void test_heap_left01(CuTest *tc){
    CuAssertTrue(tc, heap_left(1) == 2);
}

void test_heap_left02(CuTest *tc){
    CuAssertTrue(tc, heap_left(10) == 20);
}

void test_heap_left03(CuTest *tc){
    CuAssertTrue(tc, heap_left(213) == 426);
}

void test_heap_right01(CuTest *tc){
    CuAssertTrue(tc, heap_right(1) == 3);
}

void test_heap_right02(CuTest *tc){
    CuAssertTrue(tc, heap_right(10) == 21);
}

void test_heap_right03(CuTest *tc){
    CuAssertTrue(tc, heap_right(213) == 427);
}

void test_heap_parent01(CuTest *tc){
    CuAssertTrue(tc, heap_parent(3) == 1);
}

void test_heap_parent02(CuTest *tc){
    CuAssertTrue(tc, heap_parent(21) == 10);
}

void test_heap_parent03(CuTest *tc){
    CuAssertTrue(tc, heap_parent(426) == 213);
}

void test_heap01(CuTest *tc){
    struct collision_heap heap;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);

    CuAssertTrue(tc, float_abs(heap.heap[1].time - 0.01) < eps);

    free(heap.heap);
}

void test_heap02(CuTest *tc){
    struct collision_heap heap;
    struct collision_data min;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);


    min = pop_min(&heap);
    CuAssertTrue(tc, float_abs(min.time - 0.01) < eps);

    free(heap.heap);
}

void test_heap03(CuTest *tc){
    struct collision_heap heap;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);


    pop_min(&heap);
    CuAssertTrue(tc, float_abs(heap.heap[1].time - 0.031) < eps);

    free(heap.heap);
}

void test_heap04(CuTest *tc){
    struct collision_heap heap;
    struct collision_data max;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);

    int i;
    for(i = 0; i < 20; ++i){
        max = pop_min(&heap);
    }
    CuAssertTrue(tc, float_abs(max.time - 2.9) < eps);

    free(heap.heap);
}

void test_heap05(CuTest *tc){
    struct collision_heap heap;
    struct collision_data min;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);

    min.time = 0.009;
    heap_insert(&heap, &min);
    min = pop_min(&heap);

    CuAssertTrue(tc, float_abs(min.time - 0.009) < eps);

    free(heap.heap);
}

void test_heap06(CuTest *tc){
    struct collision_heap heap;
    struct collision_data min;
    construct_heap(&heap, 20);

    heap.heap[1].time = 0.7;
    heap.heap[2].time = 0.2;
    heap.heap[3].time = 0.01;
    heap.heap[4].time = 0.031;
    heap.heap[5].time = 0.041;
    heap.heap[6].time = 0.3;
    heap.heap[7].time = 0.1;
    heap.heap[8].time = 0.5;
    heap.heap[9].time = 0.9;
    heap.heap[10].time = 1.023;
    heap.heap[11].time = 1.7;
    heap.heap[12].time = 1.2;
    heap.heap[13].time = 1.01;
    heap.heap[14].time = 1.031;
    heap.heap[15].time = 1.041;
    heap.heap[16].time = 1.3;
    heap.heap[17].time = 1.1;
    heap.heap[18].time = 1.5;
    heap.heap[19].time = 2.9;
    heap.heap[20].time = 0.1;

    heap.length = 20;
    build_heap(&heap);

    min.time = 1;
    heap_insert(&heap, &min);
    int i;
    for(i = 0; i < 11; ++i){
        min = pop_min(&heap);
    }
    CuAssertTrue(tc, float_abs(min.time - 1) < eps);

    free(heap.heap);
}

void test_do_segments_intersect01(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 5},
                 B2 = {5, 0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect02(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 0},
                 B2 = {5, 5};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect03(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2, 2},
                 B2 = {3, 3};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect04(CuTest *tc){
    struct point A1 = {0,  0},
                 A2 = {5,  0},
                 B1 = {5, -3},
                 B2 = {5,  0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect05(CuTest *tc){
    struct point A1 = {0,  0},
                 A2 = {5,  0},
                 B1 = {6, -3},
                 B2 = {6,  0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2) == false);
}

void test_get_segment_intersection01(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 5},
                 B2 = {5, 0},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 float_abs(I.x - 2.5) < eps &&
                 float_abs(I.y - 2.5) < eps);
}

void test_get_segment_intersection02(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2, 2},
                 B2 = {4, 4},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f == false);
}

void test_get_segment_intersection03(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2.5, 2.5},
                 B2 = {5, 0},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 float_abs(I.x - 2.5) < eps &&
                 float_abs(I.y - 2.5) < eps);
}

void test_get_segment_intersection04(CuTest *tc){
    struct point A1 = {0, 0},
                 A2 = {0, 10},
                 B1 = {-1, 0},
                 B2 = {1, 10},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 float_abs(I.x) < eps &&
                 float_abs(I.y - 5) < eps);
}

void test_get_segment_intersection05(CuTest *tc){
    struct point A1 = {5.3, 0.4},
                 A2 = {0.5, 2.2},
                 B1 = {2.134, -0.07},
                 B2 = {5.93, 1.75},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 float_abs(I.x - 4.07354709) < eps &&
                 float_abs(I.y - 0.85991983) < eps);
}

/**
    These test assume that ZONE_FACTOR == 50
    */
void test_get_outer_zones_of_segment01(CuTest *tc){
    struct point A = {SCREEN_BUFFER_HEIGHT / 25, SCREEN_BUFFER_HEIGHT / 25},
                 B = {SCREEN_BUFFER_HEIGHT, SCREEN_BUFFER_HEIGHT};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 2 &&
                          zones[1] == 2 &&
                          zones[2] == ZONE_FACTOR - 1 &&
                          zones[3] == ZONE_FACTOR - 1);
}

void test_get_outer_zones_of_segment02(CuTest *tc){
    struct point A = {-1, -1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, -1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment03(CuTest *tc){
    struct point A = {-1, SCREEN_BUFFER_HEIGHT + 1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, SCREEN_BUFFER_HEIGHT + 1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment04(CuTest *tc){
    struct point A = {-1, -1},
                 B = {-1, SCREEN_BUFFER_HEIGHT + 1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment05(CuTest *tc){
    struct point A = {SCREEN_BUFFER_HEIGHT + 1, SCREEN_BUFFER_HEIGHT + 1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, -1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment06(CuTest *tc){
    struct point A = {-ZONE_SIZE * 3, ZONE_SIZE / 5},
                 B = {SCREEN_BUFFER_HEIGHT / 2, -ZONE_SIZE * 5};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment07(CuTest *tc){
    struct point A = {-ZONE_SIZE, ZONE_SIZE * 2},
                 B = {ZONE_SIZE * 2, -ZONE_SIZE};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 0 && zones[1] == 1 &&
                          zones[2] == 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment08(CuTest *tc){
    struct point A = {SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2, SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3},
                 B = {SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3, SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == ZONE_FACTOR - 1 && zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == ZONE_FACTOR - 1 && zones[3] == ZONE_FACTOR - 1);
}

void test_get_outer_zones_of_segment09(CuTest *tc){
    struct point A = {SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2, ZONE_SIZE * 3},
                 B = {SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3, -ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == ZONE_FACTOR - 1 && zones[1] == 1 &&
                          zones[2] == ZONE_FACTOR - 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment10(CuTest *tc){
    struct point A = {-ZONE_SIZE, ZONE_SIZE * 4},
                 B = {ZONE_SIZE * 2, -ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 0 && zones[1] == 2 &&
                          zones[2] == 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment11(CuTest *tc){
    struct point A = {ZONE_SIZE * 9, SCREEN_BUFFER_HEIGHT + ZONE_SIZE},
                 B = {-ZONE_SIZE, SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 5 && zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == 0 && zones[3] == ZONE_FACTOR - 2);
}

void test_get_outer_zones_of_segment12(CuTest *tc){
    struct point A = {(SCREEN_BUFFER_HEIGHT * 2) / 3, (SCREEN_BUFFER_HEIGHT * 2) / 3},
                 B = {(SCREEN_BUFFER_HEIGHT * 5) / 3, 0};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == (short int)(((float)(ZONE_FACTOR * 2)) / 3) &&
                          zones[1] == (short int)(((float)(ZONE_FACTOR * 2)) / 3) &&
                          zones[2] == ZONE_FACTOR - 1 &&
                          zones[3] == (short int)(((float)(ZONE_FACTOR * 4)) / 9));
}

void test_get_outer_zones_of_segment13(CuTest *tc){
    struct point A = {SCREEN_BUFFER_HEIGHT / 2,  SCREEN_BUFFER_HEIGHT * 2},
                 B = {SCREEN_BUFFER_HEIGHT / 2, -SCREEN_BUFFER_HEIGHT * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == (short int)((float)(ZONE_FACTOR / 2)) &&
                          zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == (short int)((float)(ZONE_FACTOR / 2)) &&
                          zones[3] == 0);
}

void test_check_collision_between_ball_and_segment01(CuTest *tc){
    struct segment seg = {{0, 0}, {50, 0}, 0, {0, 0, 0, 0}};
    float  x = 25, y = 25,
           dx = 0, dy = -20,
           r = 15;
    CuAssertTrue(tc, double_abs(check_collision_between_ball_and_segment(x, y, dx, dy, r, &seg) - 0.5) < eps);
}

void test_check_collision_between_ball_and_segment02(CuTest *tc){
    struct segment seg = {{0, 0}, {50, 0}, 0, {0, 0, 0, 0}};
    float  x = 25, y = 25,
           dx = 60, dy = -20,
           r = 15;
    CuAssertTrue(tc, double_abs(check_collision_between_ball_and_segment(x, y, dx, dy, r, &seg) - EMPTY_COLLISION_TIME) < eps);
}

void test_fast_read_set01(CuTest *tc){
    short int i;
    fast_read_set set;
    printf("\nFast-read-set test#01 - elements 0-19: set_mark\nmarked: 0, 2, 5, 12, 19\n\tarray read directly:\n");
    initialize_fast_read_set(&set, 20);
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
    initialize_fast_read_set(&set, 20);
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
    initialize_fast_read_set(&set, 20);
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
    initialize_fast_read_set(&set, 20);
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

CuSuite* mainGetSuite(void){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testabsTrue1);
    SUITE_ADD_TEST(suite, testabsTrue2);
    SUITE_ADD_TEST(suite, testabsTrue3);
    SUITE_ADD_TEST(suite, testabsTrue4);
    SUITE_ADD_TEST(suite, testabsTrue5);

    SUITE_ADD_TEST(suite, testrzadTrue01);
    SUITE_ADD_TEST(suite, testrzadTrue02);
    SUITE_ADD_TEST(suite, testrzadTrue03);
    SUITE_ADD_TEST(suite, testrzadTrue04);
    SUITE_ADD_TEST(suite, testrzadTrue05);
    SUITE_ADD_TEST(suite, testrzadTrue06);
    SUITE_ADD_TEST(suite, testrzadTrue07);
    SUITE_ADD_TEST(suite, testrzadTrue08);
    SUITE_ADD_TEST(suite, testrzadTrue09);
    SUITE_ADD_TEST(suite, testrzadTrue11);
    SUITE_ADD_TEST(suite, testrzadTrue12);
    SUITE_ADD_TEST(suite, testrzadTrue13);
    SUITE_ADD_TEST(suite, testrzadTrue14);
    SUITE_ADD_TEST(suite, testrzadTrue15);
    SUITE_ADD_TEST(suite, testrzadTrue16);
    SUITE_ADD_TEST(suite, testrzadTrue17);
    SUITE_ADD_TEST(suite, testrzadTrue18);
    SUITE_ADD_TEST(suite, testrzadTrue19);
    SUITE_ADD_TEST(suite, testrzadTrue20);
    SUITE_ADD_TEST(suite, testrzadTrue21);
    SUITE_ADD_TEST(suite, testrzadTrue22);
    SUITE_ADD_TEST(suite, testrzadTrue23);
    SUITE_ADD_TEST(suite, testrzadTrue24);
    SUITE_ADD_TEST(suite, testrzadTrue25);
    SUITE_ADD_TEST(suite, testrzadTrue26);
    SUITE_ADD_TEST(suite, testrzadTrue27);
    SUITE_ADD_TEST(suite, testrzadTrue28);
    SUITE_ADD_TEST(suite, testrzadTrue29);
    SUITE_ADD_TEST(suite, testrzadTrue30);
    SUITE_ADD_TEST(suite, testrzadTrue31);
    SUITE_ADD_TEST(suite, testrzadTrue32);
    SUITE_ADD_TEST(suite, testrzadTrue33);
    SUITE_ADD_TEST(suite, testrzadTrue34);

    SUITE_ADD_TEST(suite, testnormTrue01);
    SUITE_ADD_TEST(suite, testnormTrue02);
    SUITE_ADD_TEST(suite, testnormTrue03);
    SUITE_ADD_TEST(suite, testnormTrue04);
    SUITE_ADD_TEST(suite, testnormTrue05);
    SUITE_ADD_TEST(suite, testnormTrue06);
    SUITE_ADD_TEST(suite, testnormTrue07);
    SUITE_ADD_TEST(suite, testnormTrue08);

    SUITE_ADD_TEST(suite, testRBtree_in_order);
    SUITE_ADD_TEST(suite, testRBtree_delete);
    SUITE_ADD_TEST(suite, testRBtree_delete_is_root_nil);
    SUITE_ADD_TEST(suite, testRBtree_height);
    SUITE_ADD_TEST(suite, testRBtree_massive_delete);
    SUITE_ADD_TEST(suite, testRBtree_massive_insert_and_delete);
    SUITE_ADD_TEST(suite, testRBtree_massive_absurd_delete);


    SUITE_ADD_TEST(suite, test_heap_left01);
    SUITE_ADD_TEST(suite, test_heap_left02);
    SUITE_ADD_TEST(suite, test_heap_left03);
    SUITE_ADD_TEST(suite, test_heap_right01);
    SUITE_ADD_TEST(suite, test_heap_right02);
    SUITE_ADD_TEST(suite, test_heap_right03);
    SUITE_ADD_TEST(suite, test_heap_parent01);
    SUITE_ADD_TEST(suite, test_heap_parent02);
    SUITE_ADD_TEST(suite, test_heap_parent03);
    SUITE_ADD_TEST(suite, test_heap01);
    SUITE_ADD_TEST(suite, test_heap02);
    SUITE_ADD_TEST(suite, test_heap03);
    SUITE_ADD_TEST(suite, test_heap04);
    SUITE_ADD_TEST(suite, test_heap05);
    SUITE_ADD_TEST(suite, test_heap06);


    SUITE_ADD_TEST(suite, test_do_segments_intersect01);
    SUITE_ADD_TEST(suite, test_do_segments_intersect02);
    SUITE_ADD_TEST(suite, test_do_segments_intersect03);
    SUITE_ADD_TEST(suite, test_do_segments_intersect04);
    SUITE_ADD_TEST(suite, test_do_segments_intersect05);

    SUITE_ADD_TEST(suite, test_get_segment_intersection01);
    SUITE_ADD_TEST(suite, test_get_segment_intersection02);
    SUITE_ADD_TEST(suite, test_get_segment_intersection03);
    SUITE_ADD_TEST(suite, test_get_segment_intersection04);
    SUITE_ADD_TEST(suite, test_get_segment_intersection05);

    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment01);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment02);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment03);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment04);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment05);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment06);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment07);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment08);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment09);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment10);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment11);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment12);
    SUITE_ADD_TEST(suite, test_get_outer_zones_of_segment13);

    SUITE_ADD_TEST(suite, test_check_collision_between_ball_and_segment01);
    SUITE_ADD_TEST(suite, test_check_collision_between_ball_and_segment02);


    SUITE_ADD_TEST(suite, test_fast_read_set01);
    SUITE_ADD_TEST(suite, test_fast_read_set02);
    SUITE_ADD_TEST(suite, test_fast_read_set03);
    SUITE_ADD_TEST(suite, test_fast_read_set04);


    return suite;
}
