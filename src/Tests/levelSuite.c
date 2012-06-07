#include <math.h>
#include "CuTest.h"
#include "../structures/level.h"

/**
    These test assume that ZONE_FACTOR == 50
    Should work for other values, though...
    */
void test_get_outer_zones_of_segment01(CuTest *tc){
    point A = {SCREEN_BUFFER_HEIGHT / 25, SCREEN_BUFFER_HEIGHT / 25},
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
    point A = {-1, -1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, -1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment03(CuTest *tc){
    point A = {-1, SCREEN_BUFFER_HEIGHT + 1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, SCREEN_BUFFER_HEIGHT + 1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment04(CuTest *tc){
    point A = {-1, -1},
                 B = {-1, SCREEN_BUFFER_HEIGHT + 1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment05(CuTest *tc){
    point A = {SCREEN_BUFFER_HEIGHT + 1, SCREEN_BUFFER_HEIGHT + 1},
                 B = {SCREEN_BUFFER_HEIGHT + 1, -1};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment06(CuTest *tc){
    point A = {-ZONE_SIZE * 3, ZONE_SIZE / 5},
                 B = {SCREEN_BUFFER_HEIGHT / 2, -ZONE_SIZE * 5};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, !f);
}

void test_get_outer_zones_of_segment07(CuTest *tc){
    point A = {-ZONE_SIZE, ZONE_SIZE * 2},
                 B = {ZONE_SIZE * 2, -ZONE_SIZE};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 0 && zones[1] == 1 &&
                          zones[2] == 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment08(CuTest *tc){
    point A = {SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2, SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3},
                 B = {SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3, SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == ZONE_FACTOR - 1 && zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == ZONE_FACTOR - 1 && zones[3] == ZONE_FACTOR - 1);
}

void test_get_outer_zones_of_segment09(CuTest *tc){
    point A = {SCREEN_BUFFER_HEIGHT + ZONE_SIZE * 2, ZONE_SIZE * 3},
                 B = {SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 3, -ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == ZONE_FACTOR - 1 && zones[1] == 1 &&
                          zones[2] == ZONE_FACTOR - 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment10(CuTest *tc){
    point A = {-ZONE_SIZE, ZONE_SIZE * 4},
                 B = {ZONE_SIZE * 2, -ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 0 && zones[1] == 2 &&
                          zones[2] == 1 && zones[3] == 0);
}

void test_get_outer_zones_of_segment11(CuTest *tc){
    point A = {ZONE_SIZE * 9, SCREEN_BUFFER_HEIGHT + ZONE_SIZE},
                 B = {-ZONE_SIZE, SCREEN_BUFFER_HEIGHT - ZONE_SIZE * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == 5 && zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == 0 && zones[3] == ZONE_FACTOR - 2);
}

void test_get_outer_zones_of_segment12(CuTest *tc){
    point A = {(SCREEN_BUFFER_HEIGHT * 2) / 3, (SCREEN_BUFFER_HEIGHT * 2) / 3},
                 B = {(SCREEN_BUFFER_HEIGHT * 5) / 3, 0};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == (short int)(((double)(ZONE_FACTOR * 2)) / 3) &&
                          zones[1] == (short int)(((double)(ZONE_FACTOR * 2)) / 3) &&
                          zones[2] == ZONE_FACTOR - 1 &&
                          zones[3] == (short int)(((double)(ZONE_FACTOR * 4)) / 9));
}

void test_get_outer_zones_of_segment13(CuTest *tc){
    point A = {SCREEN_BUFFER_HEIGHT / 2,  SCREEN_BUFFER_HEIGHT * 2},
                 B = {SCREEN_BUFFER_HEIGHT / 2, -SCREEN_BUFFER_HEIGHT * 2};
    bool f;
    short int zones[4];
    f = get_outer_zones_of_segment(&A, &B, zones);
    CuAssertTrue(tc, f && zones[0] == (short int)((double)(ZONE_FACTOR / 2)) &&
                          zones[1] == ZONE_FACTOR - 1 &&
                          zones[2] == (short int)((double)(ZONE_FACTOR / 2)) &&
                          zones[3] == 0);
}

CuSuite* levelGetSuite(void){
    CuSuite* suite = CuSuiteNew();

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

    return suite;
}
