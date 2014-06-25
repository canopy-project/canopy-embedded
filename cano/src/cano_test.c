/* Copyright 2014 - Greg Prisament
 */

/* TODO: Combine w/ libcanopy/tests?
 */
#include "cano.h"
#include "canopy.h"
#include "red_test.h"

int RunTest(int argc, const char *argv[])
{
    RedTest suite = RedTest_Begin(argv[0], NULL, NULL);

    /* Test creation & destruction */
    {
        CanopyContext canopy = canopy_init();
        RedTest_Verify(suite, "Initialize canopy", canopy);
        
        canopy_shutdown(canopy);
        RedTest_Verify(suite, "Shutdown canopy (didn't crash)", true);
    }

    return RedTest_End(suite);
}
