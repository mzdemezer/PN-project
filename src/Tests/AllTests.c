#include <stdio.h>

#include "CuTest.h"

extern CuSuite* RBtreeGetSuite(void);
extern CuSuite* stringprocessingGetSuite(void);
extern CuSuite* mathematicsGetSuite(void);
extern CuSuite* heapGetSuite(void);
extern CuSuite* fastreadsetGetSuite(void);
extern CuSuite* levelGetSuite(void);
extern CuSuite* collisionsGetSuite(void);

void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	//suites adding
    CuSuiteAddSuite(suite, RBtreeGetSuite());
    CuSuiteAddSuite(suite, stringprocessingGetSuite());
    CuSuiteAddSuite(suite, mathematicsGetSuite());
    CuSuiteAddSuite(suite, heapGetSuite());
    CuSuiteAddSuite(suite, fastreadsetGetSuite());
    CuSuiteAddSuite(suite, levelGetSuite());
    CuSuiteAddSuite(suite, collisionsGetSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}
