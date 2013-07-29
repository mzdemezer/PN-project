#include "CuTest.h"
#include <string.h>
#include <allegro5/allegro.h>
#include "../stringprocessing.h"

void teststringify_resolutionTrue1(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 0;
    TestedDisplay.height = 0;
    char res[20];
    stringify_resolution(&TestedDisplay, res);
    CuAssertTrue(tc, strcmp(res, "0x0") == 0);
}

void teststringify_resolutionTrue2(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 200;
    TestedDisplay.height = 320;
    char res[20];
    stringify_resolution(&TestedDisplay, res);
    CuAssertTrue(tc, strcmp(res, "200x320") == 0);
}

void teststringify_resolutionTrue3(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 800;
    TestedDisplay.height = 600;
    char res[20];
    stringify_resolution(&TestedDisplay, res);
    CuAssertTrue(tc, strcmp(res, "800x600") == 0);
}

void teststringify_resolutionTrue4(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 1024;
    TestedDisplay.height = 768;
    char res[20];
    stringify_resolution(&TestedDisplay, res);
    CuAssertTrue(tc, strcmp(res, "1024x768") == 0);
}

void teststringify_resolutionTrue5(CuTest *tc){
    ALLEGRO_DISPLAY_MODE TestedDisplay;
    TestedDisplay.width = 1280;
    TestedDisplay.height = 1024;
    char res[20];
    stringify_resolution(&TestedDisplay, res);
    CuAssertTrue(tc, strcmp(res, "1280x1024") == 0);
}

CuSuite* stringprocessingGetSuite(void){
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, teststringify_resolutionTrue1);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue2);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue3);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue4);
    SUITE_ADD_TEST(suite, teststringify_resolutionTrue5);

    return suite;
}

