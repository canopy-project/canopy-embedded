#include "red_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct RedTest_t
{
    char * testName;
    unsigned numFailed;
    unsigned numTotal;
    void (*fnOnTestResult)(RedTest, const char *, bool);
    void (*fnOnEnd)(RedTest);
};

/*
 * Default RedTest_Verify reporting callback.  Logs failures to stderr.  Silent
 * for passing subtests.
 */
static void _DefaultOnTestResult(RedTest suite, const char *subtestName, bool passed)
{
    if (!passed)
    {
        fprintf(stderr, "%s: Subtest %d \'%s\' failed!\n",
            RedTest_GetName(suite),
            RedTest_NumTotal(suite),
            subtestName);
        fflush(stderr);
    }
}

/*
 * Default RedTest_End reporting callback.  Logs one-line final summary to
 * stderr.
 */
static void _DefaultOnEnd(RedTest suite)
{
    bool passed = (RedTest_NumFailed(suite) == 0);
    if (passed)
    {
        fprintf(stderr, "%s PASSED.  %d subtests.\n", 
            RedTest_GetName(suite),
            RedTest_NumTotal(suite));
    }
    else
    {
        fprintf(stderr, "%s FAILED! %d of %d subtests pass. %d failed.\n",
            RedTest_GetName(suite),
            RedTest_NumPassed(suite),
            RedTest_NumTotal(suite),
            RedTest_NumFailed(suite));
    }
    fflush(stderr);
}

RedTest RedTest_Begin(
        const char *testname, 
        void (*fnOnTestResult)(RedTest, const char *, bool),
        void (*fnOnEnd)(RedTest))
{
    RedTest suite;

    suite = malloc(sizeof(struct RedTest_t));
    if (!suite)
        return NULL;

    suite->testName = calloc(1, strlen(testname) + 1);
    strcpy(suite->testName, testname);
    if (!suite->testName)
    {
        free(suite);
        return NULL;
    }

    suite->fnOnTestResult = fnOnTestResult ? fnOnTestResult : _DefaultOnTestResult;
    suite->fnOnEnd = fnOnEnd ? fnOnEnd : _DefaultOnEnd;

    suite->numFailed = 0;
    suite->numTotal = 0;
    return suite;
}

void RedTest_Verify(RedTest suite, const char *subtestName, bool passCondition)
{
    if (!passCondition)
        suite->numFailed++;
    suite->numTotal++;
    suite->fnOnTestResult(suite, subtestName, passCondition);
}

unsigned RedTest_NumPassed(RedTest suite)
{
    return suite->numTotal - suite->numFailed;
}

unsigned RedTest_NumFailed(RedTest suite)
{
    return suite->numFailed;
}

unsigned RedTest_NumTotal(RedTest suite)
{
    return suite->numTotal;
}
const char * RedTest_GetName(RedTest suite)
{
    return suite->testName;
}


int RedTest_End(RedTest suite)
{
    bool passed = !(suite->numFailed);
    suite->fnOnEnd(suite);
    free(suite->testName);
    free(suite);
    return passed ? 0 : 1;
}
