#include "CuTest.h"
#include "../main.h"
#include "../game.h"

void testVectorAngleTrue00(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(0, 0) == 0);
}

void testVectorAngleTrue01(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(1, 0) == 0);
}

void testVectorAngleTrue02(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(2, 0) == 0);
}

void testVectorAngleTrue03(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(3, 0) == 0);
}

void testVectorAngleTrue04(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(4, 0) == 0);
}

void testVectorAngleTrue05(CuTest *tc){
    CuAssertTrue(tc, VectorAngle(5, 0) == 0);
}


void testVectorAngleTrue06(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-1, 0) - PI) < eps);
}

void testVectorAngleTrue07(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-2, 0) - PI) < eps);
}

void testVectorAngleTrue08(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-3, 0) - PI) < eps);
}

void testVectorAngleTrue09(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-4, 0) - PI) < eps);
}

void testVectorAngleTrue10(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 0) - PI) < eps);
}


void testVectorAngleTrue11(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, 1) - PIpol) < eps);
}

void testVectorAngleTrue12(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, 2) - PIpol) < eps);
}

void testVectorAngleTrue13(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, 3) - PIpol) < eps);
}

void testVectorAngleTrue14(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, 4) - PIpol) < eps);
}

void testVectorAngleTrue15(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, 5) - PIpol) < eps);
}


void testVectorAngleTrue16(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, -1) - PI32) < eps);
}

void testVectorAngleTrue17(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, -2) - PI32) < eps);
}

void testVectorAngleTrue18(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, -3) - PI32) < eps);
}

void testVectorAngleTrue19(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, -4) - PI32) < eps);
}

void testVectorAngleTrue20(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(0, -5) - PI32) < eps);
}


void testVectorAngleTrue21(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, 1) - 0.19739555) < eps);
}

void testVectorAngleTrue22(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, 2) - 0.38050637) < eps);
}

void testVectorAngleTrue23(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, 3) - 0.5404195) < eps);
}

void testVectorAngleTrue24(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, 4) - 0.67474094) < eps);
}

void testVectorAngleTrue25(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, 5) - PI4) < eps);
}

void testVectorAngleTrue26(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(4, 5) - 0.89605538) < eps);
}

void testVectorAngleTrue27(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(3, 5) - 1.03037682) < eps);
}

void testVectorAngleTrue28(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(2, 5) - 1.19028994) < eps);
}

void testVectorAngleTrue29(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(1, 5) - 1.37340077) < eps);
}


void testVectorAngleTrue30(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-1, 5) - 1.76819188) < eps);
}

void testVectorAngleTrue31(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-2, 5) - 1.9513027) < eps);
}

void testVectorAngleTrue32(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-3, 5) - 2.11121582) < eps);
}

void testVectorAngleTrue33(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-4, 5) - 2.24553726) < eps);
}

void testVectorAngleTrue34(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 5) - PI34) < eps);
}

void testVectorAngleTrue35(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 4) - 2.46685171) < eps);
}

void testVectorAngleTrue36(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 3) - 2.60117315) < eps);
}

void testVectorAngleTrue37(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 2) - 2.76108627) < eps);
}

void testVectorAngleTrue38(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, 1) - 2.94419709) < eps);
}


void testVectorAngleTrue39(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, -1) - 3.33898821) < eps);
}

void testVectorAngleTrue40(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, -2) - 3.52209903) < eps);
}

void testVectorAngleTrue41(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, -3) - 3.68201215) < eps);
}

void testVectorAngleTrue42(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, -4) - 3.81633359) < eps);
}

void testVectorAngleTrue43(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-5, -5) - PI54) < eps);
}

void testVectorAngleTrue44(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-4, -5) - 4.03764803) < eps);
}

void testVectorAngleTrue45(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-3, -5) - 4.17196948) < eps);
}

void testVectorAngleTrue46(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-2, -5) - 4.3318826) < eps);
}

void testVectorAngleTrue47(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(-1, -5) - 4.51499342) < eps);
}


void testVectorAngleTrue48(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(1, -5) - 4.90978454) < eps);
}

void testVectorAngleTrue49(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(2, -5) - 5.09289535) < eps);
}

void testVectorAngleTrue50(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(3, -5) - 5.25280848) < eps);
}

void testVectorAngleTrue51(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(4, -5) - 5.38712992) < eps);
}

void testVectorAngleTrue52(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, -5) - PI74) < eps);
}

void testVectorAngleTrue53(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, -4) - 5.60844436) < eps);
}

void testVectorAngleTrue54(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, -3) - 5.7427658) < eps);
}

void testVectorAngleTrue55(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, -2) - 5.90267893) < eps);
}

void testVectorAngleTrue56(CuTest *tc){
    CuAssertTrue(tc, abs(VectorAngle(5, -1) - 6.08578974) < eps);
}

CuSuite* gameGetSuite(void){
    CuSuite* suite = CuSuiteNew();

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

    return suite;
}

