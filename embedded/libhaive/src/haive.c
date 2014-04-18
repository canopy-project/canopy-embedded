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
    HaiveCallbackRoutine cb;
    HaiveCallbackRoutine cbExtra;
    bool quitRequested;
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

bool haive_register_event_callback(HaiveContext haive, HaiveEventCallbackRoutine fn, void *extra)
{
    haive->cb = fn;
    haive->cbExtra = extra;
    return true;
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

bool haive_send_report(HaiveReport report)
{
    // construct websocket message
}

bool haive_load_device_description(HaiveContext haive, const char *filename)
{
    FILE *fp;
    bool result;
    fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }
    result = haive_load_device_description_file(haive, fp);
    fclose(fp);
    return result;
}

bool haive_load_device_description_file(HaiveContext haive, FILE *file)
{
    /* Read entire file into memory */
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    char *buffer = malloc(filesize);
    fread(&buffer, 1, filesize, file);
    haive_load_device_description_string(haive, buffer);
    free(buffer);
}

bool haive_load_device_description_string(HaiveContext haive, const char *szDesc);
{
    RedJsonObject jsonObj = RedJson_Parse(szDesc);
    
    for (int i = 0; i < RedJsonObject_NumItems(jsonObj); i++)
    {
        RedJsonObject_GetKeyByIndex(jsonObj, i)
    }
}

bool haive_event_loop(HaiveContext haive)
{
    while (!haive->quitRequested)
    {
        if (haive->cb)
        {
            haive->cb(haive, HAIVE_EVENT_REPORT_REQUESTED, haive->cbExtra);
        }
        sleep(10)
    }
}

void haive_quit(HaiveContext haive)
{
    haive->quitRequested = true;
}

void haive_shutdown(HaiveContext haive)
{
    free(haive);
}
