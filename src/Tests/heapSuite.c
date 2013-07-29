#include "CuTest.h"
#include "../structures/collstructures.h"
#include "../mathematics.h"

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
    collision_heap heap;
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

    CuAssertTrue(tc, double_abs(heap.heap[1].time - 0.01) < eps);

    free(heap.heap);
}

void test_heap02(CuTest *tc){
    collision_heap heap;
    collision_data min;
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
    CuAssertTrue(tc, double_abs(min.time - 0.01) < eps);

    free(heap.heap);
}

void test_heap03(CuTest *tc){
    collision_heap heap;
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
    CuAssertTrue(tc, double_abs(heap.heap[1].time - 0.031) < eps);

    free(heap.heap);
}

void test_heap04(CuTest *tc){
    collision_heap heap;
    collision_data max;
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
    CuAssertTrue(tc, double_abs(max.time - 2.9) < eps);

    free(heap.heap);
}

void test_heap05(CuTest *tc){
    collision_heap heap;
    collision_data min;
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

    CuAssertTrue(tc, double_abs(min.time - 0.009) < eps);

    free(heap.heap);
}

void test_heap06(CuTest *tc){
    collision_heap heap;
    collision_data min;
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
    CuAssertTrue(tc, double_abs(min.time - 1) < eps);

    free(heap.heap);
}

CuSuite* heapGetSuite(void){
    CuSuite* suite = CuSuiteNew();

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
