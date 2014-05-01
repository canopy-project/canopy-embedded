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
        HaiveReport report;
        HaiveContext haive = haive_init();
        haive_load_device_description(haive, "mydevice.sddl");

        report = haive_begin_report(haive);
        haive_report_float32(report, "temperature", 24.38f);
        haive_send_report(report);

        haive_shutdown(haive);
    }
    return RedTest_End(suite);
}
