/*
 * Interact with Raspberry Pi GPIO pins using file interface.
 *
 * Author: Greg Prisament
 */
#include "../common/cano_rpi_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* returns true on success */
bool set_gpio(int pin, int value)
{
    FILE *fp;
    char buf[1024];

    snprintf(buf, 1024, "/sys/class/gpio/gpio%d/value", pin);
    fp = fopen(buf, "w");
    if (!fp)
    {
        fprintf(stderr, "Failed to set value of gpio%d\n", pin);
        return false;
    }

    printf("writing %d to %s\n", value, buf);
    fprintf(fp, "%d", value);
    fclose(fp);

    return true;
}

bool set_gpio_direction(int pin, const char *direction) 
{
    FILE *fp;
    char buf[1024];

    /* set GPIO pin direction */
    snprintf(
        buf, 
        1024, 
        "/sys/class/gpio/gpio%d/direction", 
        pin);
    fp = fopen(buf, "w");
    if (!fp)
    {
        fprintf(stderr, "Failed to set direction of gpio%d\n", pin);
        goto cleanup;
    }
    printf("writing %s to %s\n", direction, buf);
    fprintf(fp, "%s", direction);
    fclose(fp);
    return true;
cleanup:
    return false;
}


bool init_gpio(int pin)
{
    FILE *fp;
    char buf[1024];

    /* export GPIO pin */
    fp = fopen("/sys/class/gpio/export", "w");
    if (!fp)
    {
        fprintf(stderr, "Failed to export pin %d\n", pin);
        goto cleanup;
    }
    printf("writing %d to /sys/class/gpio/export\n", pin);
    fprintf(fp, "%d", pin);
    fclose(fp);

    /* wait for ready */
    snprintf(
        buf, 
        1024, 
        "/sys/class/gpio/gpio%d/direction", 
        pin);
    do
    {
        fp = fopen(buf, "w");
    } while (!fp && errno == EACCES);
    if (!fp)
    {
        fprintf(stderr, "GPIO %d not ready\n", pin);
        return false;
    }
    fclose(fp);
    fprintf(stderr, "GPIO %d ready\n", pin);

    return true;
cleanup:
    return false;
}

