#include "CuTest.h"
#include "../main.h"
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
    for(i = 31; i > 0; --i){
        insert_node(&tree, i);
    }
    //test
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
    in_order(tree.root, tree.nil);

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
    for(i = 32000; i > 0; --i){
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
    printf("\nHeight test:\nmin: %d\nmax: %d\ndif: %d", min_max[0] + 1, min_max[1] + 1, min_max[1] - min_max[0]);

    clear_tree(&tree);
    free(tree.nil);
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
    SUITE_ADD_TEST(suite, testRBtree_height);

    return suite;
}

