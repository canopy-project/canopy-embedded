/*
 * Copyright 2014 Gregory Prisament
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
