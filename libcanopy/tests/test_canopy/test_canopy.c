/* Copyright 2014 - Greg Prisament
 */

#include "canopy.h"
#include "red_test.h"

int main(int argc, const char *argv[])
{
    RedTest suite = RedTest_Begin(argv[0], NULL, NULL);

    /* Test creation & destruction */
    {
        CanopyContext canopy = canopy_init();
        RedTest_Verify(suite, "Initialize canopy", canopy);
        
        canopy_shutdown(canopy);
        RedTest_Verify(suite, "Shutdown canopy (didn't crash)", true);
    }

    /* Test sddl file loading */
    {
        CanopyReport report;
        CanopyContext canopy = canopy_init();
        canopy_load_device_description(canopy, "mydevice.sddl");

        report = canopy_begin_report(canopy);
        canopy_report_float32(report, "temperature", 24.38f);
        canopy_report_float32(report, "humidity", 0.01f);
        canopy_send_report(report);

        canopy_connect(canopy, "");

        canopy_shutdown(canopy);
    }
    return RedTest_End(suite);
}
