#include "party_relay.h"
#include "../common/cano_rpi_gpio.h"
#include <canopy.h>

#define FOG_GPIO 23
#define LIGHTS_GPIO 15

static bool on_change__lights(CanopyContext canopy, bool value)
{
    set_gpio(LIGHTS_GPIO, value ? 0 : 1);
    return true;
}

static bool on_change__fog(CanopyContext canopy, bool value)
{
    set_gpio(FOG_GPIO, value ? 0 : 1);
    return false;
}

static bool on_canopy_init(CanopyContext canopy)
{
    printf("starting...\n");

    init_gpio(FOG_GPIO);
    set_gpio_direction(FOG_GPIO, "out");
    set_gpio(FOG_GPIO, 1);

    init_gpio(LIGHTS_GPIO);
    set_gpio_direction(LIGHTS_GPIO, "out");
    set_gpio(LIGHTS_GPIO, 1);
    return true;
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

    /* Your code here */

    canopy_send_report(report);
    return true;
}
