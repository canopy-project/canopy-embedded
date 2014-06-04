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
        return false; /*TODO: generate warning */
    return !strcmp(name, event->eventControlName);
}

bool canopy_event_get_control_value_i8(CanopyEventDetails event, int8_t *outValue)
{
    if (event->value.datatype != SDDL_DATATYPE_INT8)
        return false; /* TODO: generate warning */
    *outValue = event->value.val.val_int8;
    return true;
}
bool canopy_event_get_control_value_u8(CanopyEventDetails event, uint8_t *outValue)
{
    if (event->value.datatype != SDDL_DATATYPE_UINT8)
        return false; /* TODO: generate warning */
    *outValue = event->value.val.val_uint8;
    return true;
}
bool canopy_event_get_control_value_i32(CanopyEventDetails event, int32_t *outValue)
{
    if (event->value.datatype != SDDL_DATATYPE_INT32)
        return false; /* TODO: generate warning */
    *outValue = event->value.val.val_int32;
    return true;
}
bool canopy_event_get_control_value_u32(CanopyEventDetails event, uint32_t *outValue)
{
    if (event->value.datatype != SDDL_DATATYPE_UINT32)
        return false; /* TODO: generate warning */
    *outValue = event->value.val.val_uint32;
    return true;
}
