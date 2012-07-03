#include <stdio.h>
#include <math.h>
#include "CuTest.h"
#include "../collisions.h"
#include "../mathematics.h"

void test_check_collision_between_ball_and_segment01(CuTest *tc){
    segment seg = {{0, 0}, {50, 0}, 0, {0, 0, 0, 0}};
    get_line_from_points(seg.A.x, seg.A.y, seg.B.x, seg.B.y, &seg.line_equation);
    double x = 25, y = 25,
           dx = 0, dy = -20,
           r = 15;
    CuAssertTrue(tc, double_abs(check_collision_between_ball_and_segment(x, y, dx, dy, r, 0, &seg) - 0.5) < eps);
}

void test_check_collision_between_ball_and_segment02(CuTest *tc){
    segment seg = {{0, 0}, {50, 0}, 0, {0, 0, 0, 0}};
    get_line_from_points(seg.A.x, seg.A.y, seg.B.x, seg.B.y, &seg.line_equation);
    double x = 25, y = 25,
           dx = 60, dy = -20,
           r = 15;
    CuAssertTrue(tc, double_abs(check_collision_between_ball_and_segment(x, y, dx, dy, r, 0, &seg) - EMPTY_COLLISION_TIME) < eps);
}

CuSuite* collisionsGetSuite(void){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_check_collision_between_ball_and_segment01);
    SUITE_ADD_TEST(suite, test_check_collision_between_ball_and_segment02);


    return suite;
}
