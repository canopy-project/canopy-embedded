#include "canopy_boilerplate.h"
#include <canopy.h>

static bool on_change__speed(CanopyContext canopy, int8_t value)
{
   /* Your code here.
    * Return true on success.
    */
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
    canopy_send_report(report);
    return true;
}

