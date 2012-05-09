#include "CuTest.h"
#include "../main.h"

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

CuSuite* mainGetSuite(){
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

    return suite;
}

