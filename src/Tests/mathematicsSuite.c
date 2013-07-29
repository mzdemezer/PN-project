#include "CuTest.h"
#include "../mathematics.h"

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

void testVectorAngleTrue00(CuTest *tc){
    CuAssertTrue(tc, vector_angle(0, 0) == 0);
}

void testVectorAngleTrue01(CuTest *tc){
    CuAssertTrue(tc, vector_angle(1, 0) == 0);
}

void testVectorAngleTrue02(CuTest *tc){
    CuAssertTrue(tc, vector_angle(2, 0) == 0);
}

void testVectorAngleTrue03(CuTest *tc){
    CuAssertTrue(tc, vector_angle(3, 0) == 0);
}

void testVectorAngleTrue04(CuTest *tc){
    CuAssertTrue(tc, vector_angle(4, 0) == 0);
}

void testVectorAngleTrue05(CuTest *tc){
    CuAssertTrue(tc, vector_angle(5, 0) == 0);
}


void testVectorAngleTrue06(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-1, 0) - PI) < eps);
}

void testVectorAngleTrue07(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-2, 0) - PI) < eps);
}

void testVectorAngleTrue08(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-3, 0) - PI) < eps);
}

void testVectorAngleTrue09(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-4, 0) - PI) < eps);
}

void testVectorAngleTrue10(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 0) - PI) < eps);
}


void testVectorAngleTrue11(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, 1) - PIpol) < eps);
}

void testVectorAngleTrue12(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, 2) - PIpol) < eps);
}

void testVectorAngleTrue13(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, 3) - PIpol) < eps);
}

void testVectorAngleTrue14(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, 4) - PIpol) < eps);
}

void testVectorAngleTrue15(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, 5) - PIpol) < eps);
}


void testVectorAngleTrue16(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, -1) - PI32) < eps);
}

void testVectorAngleTrue17(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, -2) - PI32) < eps);
}

void testVectorAngleTrue18(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, -3) - PI32) < eps);
}

void testVectorAngleTrue19(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, -4) - PI32) < eps);
}

void testVectorAngleTrue20(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(0, -5) - PI32) < eps);
}


void testVectorAngleTrue21(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, 1) - 0.19739555) < eps);
}

void testVectorAngleTrue22(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, 2) - 0.38050637) < eps);
}

void testVectorAngleTrue23(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, 3) - 0.5404195) < eps);
}

void testVectorAngleTrue24(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, 4) - 0.67474094) < eps);
}

void testVectorAngleTrue25(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, 5) - PI4) < eps);
}

void testVectorAngleTrue26(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(4, 5) - 0.89605538) < eps);
}

void testVectorAngleTrue27(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(3, 5) - 1.03037682) < eps);
}

void testVectorAngleTrue28(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(2, 5) - 1.19028994) < eps);
}

void testVectorAngleTrue29(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(1, 5) - 1.37340077) < eps);
}


void testVectorAngleTrue30(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-1, 5) - 1.76819188) < eps);
}

void testVectorAngleTrue31(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-2, 5) - 1.9513027) < eps);
}

void testVectorAngleTrue32(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-3, 5) - 2.11121582) < eps);
}

void testVectorAngleTrue33(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-4, 5) - 2.24553726) < eps);
}

void testVectorAngleTrue34(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 5) - PI34) < eps);
}

void testVectorAngleTrue35(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 4) - 2.46685171) < eps);
}

void testVectorAngleTrue36(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 3) - 2.60117315) < eps);
}

void testVectorAngleTrue37(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 2) - 2.76108627) < eps);
}

void testVectorAngleTrue38(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, 1) - 2.94419709) < eps);
}


void testVectorAngleTrue39(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, -1) - 3.33898821) < eps);
}

void testVectorAngleTrue40(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, -2) - 3.52209903) < eps);
}

void testVectorAngleTrue41(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, -3) - 3.68201215) < eps);
}

void testVectorAngleTrue42(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, -4) - 3.81633359) < eps);
}

void testVectorAngleTrue43(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-5, -5) - PI54) < eps);
}

void testVectorAngleTrue44(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-4, -5) - 4.03764803) < eps);
}

void testVectorAngleTrue45(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-3, -5) - 4.17196948) < eps);
}

void testVectorAngleTrue46(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-2, -5) - 4.3318826) < eps);
}

void testVectorAngleTrue47(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(-1, -5) - 4.51499342) < eps);
}


void testVectorAngleTrue48(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(1, -5) - 4.90978454) < eps);
}

void testVectorAngleTrue49(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(2, -5) - 5.09289535) < eps);
}

void testVectorAngleTrue50(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(3, -5) - 5.25280848) < eps);
}

void testVectorAngleTrue51(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(4, -5) - 5.38712992) < eps);
}

void testVectorAngleTrue52(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, -5) - PI74) < eps);
}

void testVectorAngleTrue53(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, -4) - 5.60844436) < eps);
}

void testVectorAngleTrue54(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, -3) - 5.7427658) < eps);
}

void testVectorAngleTrue55(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, -2) - 5.90267893) < eps);
}

void testVectorAngleTrue56(CuTest *tc){
    CuAssertTrue(tc, abs(vector_angle(5, -1) - 6.08578974) < eps);
}

void test_do_segments_intersect01(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 5},
                 B2 = {5, 0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect02(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 0},
                 B2 = {5, 5};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect03(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2, 2},
                 B2 = {3, 3};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect04(CuTest *tc){
    point A1 = {0,  0},
                 A2 = {5,  0},
                 B1 = {5, -3},
                 B2 = {5,  0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2));
}

void test_do_segments_intersect05(CuTest *tc){
    point A1 = {0,  0},
                 A2 = {5,  0},
                 B1 = {6, -3},
                 B2 = {6,  0};

    CuAssertTrue(tc, do_segments_intersect(&A1, &A2, &B1, &B2) == false);
}

void test_get_segment_intersection01(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {0, 5},
                 B2 = {5, 0},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 double_abs(I.x - 2.5) < eps &&
                 double_abs(I.y - 2.5) < eps);
}

void test_get_segment_intersection02(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2, 2},
                 B2 = {4, 4},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f == false);
}

void test_get_segment_intersection03(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {5, 5},
                 B1 = {2.5, 2.5},
                 B2 = {5, 0},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 double_abs(I.x - 2.5) < eps &&
                 double_abs(I.y - 2.5) < eps);
}

void test_get_segment_intersection04(CuTest *tc){
    point A1 = {0, 0},
                 A2 = {0, 10},
                 B1 = {-1, 0},
                 B2 = {1, 10},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 double_abs(I.x) < eps &&
                 double_abs(I.y - 5) < eps);
}

void test_get_segment_intersection05(CuTest *tc){
    point A1 = {5.3, 0.4},
                 A2 = {0.5, 2.2},
                 B1 = {2.134, -0.07},
                 B2 = {5.93, 1.75},
                 I;
    bool f;
    f = get_segment_intersection(&A1, &A2, &B1, &B2, &I);
    CuAssertTrue(tc, f &&
                 double_abs(I.x - 4.07354709) < eps &&
                 double_abs(I.y - 0.85991983) < eps);
}

CuSuite* mathematicsGetSuite(void){
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

    SUITE_ADD_TEST(suite, testVectorAngleTrue00);
    SUITE_ADD_TEST(suite, testVectorAngleTrue01);
    SUITE_ADD_TEST(suite, testVectorAngleTrue02);
    SUITE_ADD_TEST(suite, testVectorAngleTrue03);
    SUITE_ADD_TEST(suite, testVectorAngleTrue04);
    SUITE_ADD_TEST(suite, testVectorAngleTrue05);
    SUITE_ADD_TEST(suite, testVectorAngleTrue06);
    SUITE_ADD_TEST(suite, testVectorAngleTrue07);
    SUITE_ADD_TEST(suite, testVectorAngleTrue08);
    SUITE_ADD_TEST(suite, testVectorAngleTrue09);
    SUITE_ADD_TEST(suite, testVectorAngleTrue10);
    SUITE_ADD_TEST(suite, testVectorAngleTrue11);
    SUITE_ADD_TEST(suite, testVectorAngleTrue12);
    SUITE_ADD_TEST(suite, testVectorAngleTrue13);
    SUITE_ADD_TEST(suite, testVectorAngleTrue14);
    SUITE_ADD_TEST(suite, testVectorAngleTrue15);
    SUITE_ADD_TEST(suite, testVectorAngleTrue16);
    SUITE_ADD_TEST(suite, testVectorAngleTrue17);
    SUITE_ADD_TEST(suite, testVectorAngleTrue18);
    SUITE_ADD_TEST(suite, testVectorAngleTrue19);
    SUITE_ADD_TEST(suite, testVectorAngleTrue20);
    SUITE_ADD_TEST(suite, testVectorAngleTrue21);
    SUITE_ADD_TEST(suite, testVectorAngleTrue22);
    SUITE_ADD_TEST(suite, testVectorAngleTrue23);
    SUITE_ADD_TEST(suite, testVectorAngleTrue24);
    SUITE_ADD_TEST(suite, testVectorAngleTrue25);
    SUITE_ADD_TEST(suite, testVectorAngleTrue26);
    SUITE_ADD_TEST(suite, testVectorAngleTrue27);
    SUITE_ADD_TEST(suite, testVectorAngleTrue28);
    SUITE_ADD_TEST(suite, testVectorAngleTrue29);
    SUITE_ADD_TEST(suite, testVectorAngleTrue30);
    SUITE_ADD_TEST(suite, testVectorAngleTrue31);
    SUITE_ADD_TEST(suite, testVectorAngleTrue32);
    SUITE_ADD_TEST(suite, testVectorAngleTrue33);
    SUITE_ADD_TEST(suite, testVectorAngleTrue34);
    SUITE_ADD_TEST(suite, testVectorAngleTrue35);
    SUITE_ADD_TEST(suite, testVectorAngleTrue36);
    SUITE_ADD_TEST(suite, testVectorAngleTrue37);
    SUITE_ADD_TEST(suite, testVectorAngleTrue38);
    SUITE_ADD_TEST(suite, testVectorAngleTrue39);
    SUITE_ADD_TEST(suite, testVectorAngleTrue40);
    SUITE_ADD_TEST(suite, testVectorAngleTrue41);
    SUITE_ADD_TEST(suite, testVectorAngleTrue42);
    SUITE_ADD_TEST(suite, testVectorAngleTrue43);
    SUITE_ADD_TEST(suite, testVectorAngleTrue44);
    SUITE_ADD_TEST(suite, testVectorAngleTrue45);
    SUITE_ADD_TEST(suite, testVectorAngleTrue46);
    SUITE_ADD_TEST(suite, testVectorAngleTrue47);
    SUITE_ADD_TEST(suite, testVectorAngleTrue48);
    SUITE_ADD_TEST(suite, testVectorAngleTrue49);
    SUITE_ADD_TEST(suite, testVectorAngleTrue50);
    SUITE_ADD_TEST(suite, testVectorAngleTrue51);
    SUITE_ADD_TEST(suite, testVectorAngleTrue52);
    SUITE_ADD_TEST(suite, testVectorAngleTrue53);
    SUITE_ADD_TEST(suite, testVectorAngleTrue54);
    SUITE_ADD_TEST(suite, testVectorAngleTrue55);
    SUITE_ADD_TEST(suite, testVectorAngleTrue56);

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

    return suite;
}

