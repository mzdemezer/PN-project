#include <stdio.h>

#include "CuTest.h"

void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	//suites adding
    CuSuiteAddSuite(suite, mainGetSuite());
    CuSuiteAddSuite(suite, menuGetSuite());
    CuSuiteAddSuite(suite, gameGetSuite());



	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}
