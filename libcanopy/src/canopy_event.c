/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy.h"
#include "canopy_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include <unistd.h>
#include <assert.h>


CanopyEventEnum canopy_get_event_type(CanopyEventDetails event)
{
    return event->eventType;
}

bool canopy_event_control_name_matches(CanopyEventDetails event, const char *name)
{
    if (event->eventType != CANOPY_EVENT_CONTROL_TRIGGER)
        return false; //TODO: generate warning
    return !strcmp(name, event->eventControlName);
}
