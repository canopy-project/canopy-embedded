/*
 * Interact with Raspberry Pi GPIO pins using file interface.
 *
 * Author: Greg Prisament
 */
#ifndef CANO_RPI_GPIO_INCLUDED
#define CANO_RPO_GPIO_INCLUDED

#include <stdbool.h>

bool init_gpio(int pin);
bool set_gpio(int pin, int value);
bool set_gpio_direction(int pin, const char *direction);

#endif
