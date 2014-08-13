#include "fan.h"
#include <canopy.h>
#include <stdlib.h>

static bool on_change__speed(CanopyContext canopy, int8_t value)
{
    printf("Speed changed to %d\n", value);
    return true;
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

    canopy_report_float32(report, "humidity", rand()/(float)RAND_MAX);
    canopy_report_float32(report, "temperature", 100.0f*rand()/(float)RAND_MAX);

    canopy_send_report(report);
    return true;
}
