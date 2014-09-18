#include "lightbulb.h"
#include <canopy.h>
#include <stdlib.h>

static bool on_change__brightness(CanopyContext canopy, float value)
{
    printf("Brightness changed to %f\n", value);
    return false;
}

static bool on_canopy_init(CanopyContext canopy)
{
    return false;
}

static bool on_canopy_shutdown(CanopyContext canopy)
{
    return false;
}

static bool on_connected(CanopyContext canopy)
{
    return false;
}

static bool on_disconnected(CanopyContext canopy)
{
    return false;
}

static bool on_report_requested(CanopyContext canopy)
{
    CanopyReport report = canopy_begin_report(canopy);

    canopy_report_float32(report, "ambient_light", rand()/(float)RAND_MAX);

    canopy_send_report(report);
    return true;
}
