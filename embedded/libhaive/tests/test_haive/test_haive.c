/* Copyright 2014 - Greg Prisament
 */

#include "haive.h"
#include "red_test.h"

int main(int argc, const char *argv[])
{
    RedTest suite = RedTest_Begin(argv[0], NULL, NULL);

    /* Test creation & destruction */
    {
        HaiveContext haive = haive_init();
        RedTest_Verify(suite, "Initialize haive", haive);
        
        haive_shutdown(haive);
        RedTest_Verify(suite, "Shutdown haive (didn't crash)", true);
    }

    /* Test sddl file loading */
    {
        /* HaiveContext haive = haive_init(); */
        /* bool haive_load_device_description(HaiveContext haive, const char *filename); */
    }
    return RedTest_End(suite);
}
