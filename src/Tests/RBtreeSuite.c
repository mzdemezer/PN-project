#include <stdio.h>
#include "CuTest.h"
#include "../main.h"

#define RED true
#define BLACK false

extern void in_order(RB_node *node, RB_node *nil);

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

CuSuite* RBtreeGetSuite(void){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testRBtree_in_order);
    SUITE_ADD_TEST(suite, testRBtree_delete);
    SUITE_ADD_TEST(suite, testRBtree_delete_is_root_nil);
    SUITE_ADD_TEST(suite, testRBtree_height);
    SUITE_ADD_TEST(suite, testRBtree_massive_delete);
    SUITE_ADD_TEST(suite, testRBtree_massive_insert_and_delete);
    SUITE_ADD_TEST(suite, testRBtree_massive_absurd_delete);

    return suite;
}
