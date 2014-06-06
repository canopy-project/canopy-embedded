#include "canopy_boilerplate.h"
#include <canopy.h>

static bool on_change__speed(CanopyContext canopy, int8_t value)
{
    FILE *fp;
    printf("Setting speed to %d\n", value);

    fp = fopen("/sys/class/gpio/gpio14/value", "w");
    if (!fp)
    {
        printf("Failed to open /sys/class/gpio/gpio14/value for write\n"); 
        return false;
    }
    fprintf(fp, "%d", value);
    fclose(fp);
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
    canopy_send_report(report);
    return true;
}

