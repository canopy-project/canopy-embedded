#include "canopy_boilerplate.h"
#include <canopy.h>
#include "pi_dht_read.h"

#define SENSOR_PIN 2

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
    CanopyReport report;
    int result;
    float humidity;
    float temperature;

    report = canopy_begin_report(canopy);
    if (!report)
    {
        return false;
    }

    printf("reading...\n");
    result = pi_dht_read(DHT22, SENSOR_PIN, &humidity, &temperature);
    if (result != DHT_SUCCESS)
    {
        // TODO: cancel report
        printf("Error reading DHT: %d\n", result);
        return false;
    }

    canopy_report_float32(report, "temperature", temperature);
    canopy_report_float32(report, "humidity", humidity);

    canopy_send_report(report);
    return true;
}

