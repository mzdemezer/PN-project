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
    printf("\nRed-Black Tree delete test\nInserted nodes 1-63, even numbers from 0 to 62 removed\n(for 0 no error), in_order:\n");
    in_order(tree.root, tree.nil);

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

    return suite;
}
