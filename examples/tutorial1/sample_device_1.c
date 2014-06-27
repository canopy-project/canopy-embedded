#include "sample_device_1.h"
#include <canopy.h>
#include <sys/reboot.h>

static bool on_trigger__reboot(CanopyContext canopy)
{
    return (reboot(RB_AUTOBOOT) == 0);
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

    FILE *fp;
    float val;

    /* Read current CPU usage */
    fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return false;
    fscanf(fp, "%f", &val);
    fclose(fp);

    /* Report it to the cloud */
    canopy_report_float32(report, "cpu", val);

    canopy_send_report(report);
    return true;
}

