/*  
 *  red_test.h -- Simple C Testing Framework
 *
 *  Author: Gregory Prsiament (greg@toruslabs.com)
 *
 *  ===========================================================================
 *  Creative Commons CC0 1.0 Universal - Public Domain 
 *
 *  To the extent possible under law, Gregory Prisament has waived all
 *  copyright and related or neighboring rights to RedTest. This work is
 *  published from: United States. 
 *
 *  For details please refer to either:
 *      - http://creativecommons.org/publicdomain/zero/1.0/legalcode
 *      - The LICENSE file in this directory, if present.
 *  ===========================================================================
 */

/*
 * RedTest - Simple C Testing Framework
 *
 *  At the beginning of your test application, create a test suite by calling:
 *
 *      --------------------------------------------------------------------
 *      RedTest suite = RedTest_Begin(argv[0], NULL, NULL);
 *      --------------------------------------------------------------------
 *
 *  The first parameter is the name of test suite.  In this example we use the
 *  name of the executable from argv[0], but it may be any string.  The next
 *  two parameters are callbacks, which can be used for customized reporting
 *  and/or logging.  Setting these to NULL uses the default reporting routines,
 *  which output to stderr.
 *
 *  A test suite consists of zero or more subtests.  The routine
 *  RedTest_Verify is used to mark the completion of a subtest.
 *
 *  NOTE: There is no routine to begin a subtest.  Simply perform your test
 *  logic and call RedTest_Verify when you've determined success or failure.  
 *
 *  The result of the subtest may be computed ahead of time, or computed inside
 *  the call to RedTest_Verify.  Here is a mostly-complete example that shows
 *  both approaches:
 *
 *      --------------------------------------------------------------------
 *      int Fib(int x) { return 1; } // intentionally broken
 *
 *      int main(int argc, const char *argv[])
 *      {
 *          RedTest suite = RedTest_Begin(argv[0], NULL, NULL);
 *          bool passed = (Fib(0) == 1);
 *          RedTest_Verify(suite, "Fib(0) is 1", passed);
 *          RedTest_Verify(suite, "Fib(1) is also 1", (Fib(1) == 1));
 *          RedTest_Verify(suite, "Fib(2) is 2", (Fib(2) == 2));
 *          return RedTest_End(suite);
 *      }
 *      --------------------------------------------------------------------
 *
 *  The code above will generate three subtests and track the results.  Since
 *  our fibinocci function is broken and Fib(2) returns 1 (and we're using the
 *  default reporting routines) the following will be output to stderr:
 *  
 *      ---(stderr)---------------------------------------------------------
 *      fib_test: Subtest 2 'Fib(2) is 2' failed!
 *      fib_test FAILED!  2 of 3 subtests passed.  1 failed.
 *      --------------------------------------------------------------------
 *
 *  When finished, the single routine RedTest_End reports a final summary and
 *  frees the resources used by RedTest.  Also, RedTest_End returns non-zero if
 *  any tests failed, so it can used in the application's main() return
 *  statement:
 *
 *      --------------------------------------------------------------------
 *      return RedTest_End(suite);
 *      --------------------------------------------------------------------
 *
 *  If default reporting routines are being used, then a one-line final summary
 *  will printed to stderr:
 *
 *      ---(stderr)---------------------------------------------------------
 *      fib_test FAILED!  2 of 3 subtests pass.  1 failed.
 *      --------------------------------------------------------------------
 *
 *   or if things went smoothly: 
 *
 *      ---(stderr)---------------------------------------------------------
 *      fib_test PASSED.  3 subtests.
 *      --------------------------------------------------------------------
 */

#ifndef RED_TEST_INCLUDED
#define RED_TEST_INCLUDED

#include <stdbool.h>

/*
 * RedTest datatype -- ADT representing a collection of subtests.  This is
 *      referred to as a test "suite".
 */
typedef struct RedTest_t * RedTest;

/*
 * RedTest_Begin -- Begin a test suite.
 *
 *      Creates a newly allocated RedTest object (referred to as the test
 *      "suite") and returns it.  Initially the suite has 0 subtests.
 *
 *      <testname> is a null-terminated string name for the test suite.  Often
 *          argv[0] is used.  Internally, a copy of this string is stored (up
 *          to 1024 characters).
 *
 *      <fnOnTestResult> is a callback routine that is triggered each time
 *          RedTest_Verify is called, which can be used for customized
 *          reporting and logging of subtest results.  If NULL, the default
 *          reporting routine is used, which prints messages to stderr.
 *          Otherwise, this must be a function that takes three parameters:
 *
 *              <RedTest> - Test suite object.
 *
 *              <const char *> - Subtest name.
 *
 *              <bool> - Subtest result (true iff subtest passed).
 *
 *      <fnOnEnd> is a callback routine that is triggered when RedTest_End is
 *          called, which can be used for customized reporting and logging of
 *          the final summary.  If NULL, the default reporting routine is used,
 *          which prints a one-line final summary to stderr.  Otherwise, this
 *          must be a function that takes one parameter:
 *
 *              <RedTest> - Test suite object.
 *
 *      RedTest_Begin returns the newly created RedTest object, or NULL on
 *      failure (OOM).
 */
RedTest RedTest_Begin(
        const char *testname, 
        void (*fnOnTestResult)(RedTest, const char *, bool),
        void (*fnOnEnd)(RedTest));

/*
 * RedTest_Verify -- Log the result of a subtest.
 *
 *      Each call to RedTest_Verify corresponds to the completion of a
 *      "subtest".
 *
 *      This routine triggers the "fnOnTestResult" callback (which was provided
 *      to RedTest_Begin) to log the subtest result.  If "fnOnTestResult" is
 *      NULL, the default reporting routine is used which prints failure
 *      messages to stderr (and is silent for passing tests).
 *
 *      Additionally, this routine updates the subtest counts for <suite>.
 *
 *      NOTE: This routine DOES NOT internally store the subtestName or result.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      <subtestName> is the null-terminated string name of the subtest.
 *
 *      <passCondition> is the result of the subtest.  If true, the subtest is
 *          considered to have passed.  If false, the subtest is considered to
 *          have failed.
 *
 *      This routine does not allocate any memory and cannot fail.
 */
void RedTest_Verify(RedTest suite, const char *subtestName, bool passCondition);

/*
 * RedTest_GetName -- Get the name of the test suite.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      Returns the name of the test suite, as passed into the <testname>
 *      parameter of RedTest_Begin.
 */
const char * RedTest_GetName(RedTest suite);

/*
 * RedTest_NumTotal -- Count number of subtests that have passed or failed up
 *      to this point.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      Returns number of subtests that have passed or failed.  This is equal
 *      to the number of time RedTest_Verify has been called.
 */
unsigned RedTest_NumTotal(RedTest suite);

/*
 * RedTest_NumPassed -- Count number of subtests that have passed up to this
 *      point.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      Returns number of subtests that have passed so far.
 */
unsigned RedTest_NumPassed(RedTest suite);

/*
 * RedTest_NumFailed -- Count number of subtests that have failed up to this
 *      point.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      Returns number of subtests that have failed so far.
 */
unsigned RedTest_NumFailed(RedTest suite);

/*
 * RedTest_NumTotal -- Count number of subtests that have passed or failed up
 *      to this point.
 *
 *      <suite> is the test suite object from RedTest_Begin.
 *
 *      Returns number of subtests that have passed or failed.  This is equal
 *      to the number of time RedTest_Verify has been called.
 */
unsigned RedTest_NumTotal(RedTest suite);

/*
 * RedTest_End -- Call when all subtests have completed.  This will report a
 *      final summary and destroy the test suite object.
 *
 *      This routine triggers the "fnOnEnd" callback (which was provided
 *      to RedTest_Begin) to report and log the final summary.  If
 *      "fnOnTestResult" is NULL, the default reporting routine is used which
 *      prints a one-line final summary to stderr.
 *
 *      This routine frees all resources used by <suite>.  DO NOT continue to
 *      use <suite> after calling this routine, or...crashy crash crash!
 *
 *      <suite> is the test suite object from RedTest_Begin.
 */
int RedTest_End(RedTest suite);

#endif
