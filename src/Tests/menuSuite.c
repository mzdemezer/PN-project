#include "CuTest.h"
#include "../main.h"
#include "../menu.h"
#include <string.h>

void teststringify_resolutionTrue1(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 0;
    TestedDisplay.height = 0;
    CuAssertTrue(tc, strcmp(stringify_resolution(&TestedDisplay), "0x0") == 0);
}

void teststringify_resolutionTrue2(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 200;
    TestedDisplay.height = 320;
    CuAssertTrue(tc, strcmp(stringify_resolution(&TestedDisplay), "200x320") == 0);
}

void teststringify_resolutionTrue3(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 800;
    TestedDisplay.height = 600;
    CuAssertTrue(tc, strcmp(stringify_resolution(&TestedDisplay), "800x600") == 0);
}

void teststringify_resolutionTrue4(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 1024;
    TestedDisplay.height = 768;
    CuAssertTrue(tc, strcmp(stringify_resolution(&TestedDisplay), "1024x768") == 0);
}

void teststringify_resolutionTrue5(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 1280;
    TestedDisplay.height = 1024;
    CuAssertTrue(tc, strcmp(stringify_resolution(&TestedDisplay), "1280x1024") == 0);
}

CuSuite* menuGetSuite(){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, teststringify_resolutionTrue1);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue2);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue3);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue4);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue5);

    return suite;
}

