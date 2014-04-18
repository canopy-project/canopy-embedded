/*
 * Copyright 2014 - Greg Prisament
 */
#include "haive.h"

typedef struct _HaiveProperty
{
    const char *name;
    HaiveDatatypeEnum datatype;
    int64_t rangeMin;
    uint64_t rangeMax;
}

typedef struct _HaivePropertyValue
{
    union
    {
        int8_t val_int8;
        int32_t val_int32;
    }
}

typedef struct _HaiveReport
{
    RedHash values;
    bool finished;
}

typedef struct HaiveContextStruct
{
    bool initialized;
    RedHash properties;
} HaiveContextStruct;

HaiveContext haive_init()
{
    HaiveContext ctx = NULL;
    ctx = calloc(1, sizeof(HaiveContext));
    if (!ctx)
    {
        // TODO: set error
        goto fail;
    }

    ctx->properties = RedHash_New(0);
    if (!ctx->properties)
    {
        goto fail;
    }
    ctx->initialized = true;
    return ctx;
fail:
    if (ctx)
    {
        RedHash_Free(ctx->properties);
        free(ctx);
    }
    return NULL;
}

HaiveReport haive_begin_report(HaiveContext haive)
{
    HaiveReport report = NULL;
    report = calloc(1, sizeof(HaiveReport));
    if (!report)
    {
        goto fail;
    }

    report->values = RedHash_New(0);
    if (!report->values)
    {
        goto fail;
    }

    return report;
fail:
    if (report)
    {
        RedHash_Free(report->values);
        free(report)
    }
    return NULL;
}

bool haive_report_i8(HaiveReport report, const char *parameter, int8_t value)
{
    /* First verify that value is acceptable */
    HaiveContext ctx = report->ctx;
    _HaiveProperty *prop;
    if (report->finished)
    {
        // haive_end_report already called, cannot make further changes.
        return false;
    }
    prop = RedHash_GetWithDefaultS(ctx->properties, parameter, NULL);
    if (!prop)
    {
        // property not found!
        return false;
    }
    else if (prop->datatype != HAIVE_DATATYPE_INT8)
    {
        // incorrect datatype
        return false;
    }
    else if (prop->hasMinRange && value < prop->rangeMin)
    {
        // value too low
        return false;
    }
    else if (prop->hasMaxRange && value > prop->rangeMax)
    {
        // value too large
        return false;
    }

    // create property value object
    _HaivePropertyValue *value = calloc(1, sizeof(_HaivePropertyValue));
    if (!value)
    {
        // allocation failed
        return false;
    }

    value->val_int8 = value;
    
    // Add it to report's hash table
    if (RedHash_UpdateOrInsertS(report->values, &oldValue, parameter, value))
    {
        free(oldValue);
    }
    return true;
}

bool haive_end_report(HaiveReport report)
{
    if (report->finished)
    {
        // report already finished
        return false;
    }
    report->finished = true;
    return true;
}

bool haive_send_report(HaiveReport report)
{
    // construct websocket message
}


