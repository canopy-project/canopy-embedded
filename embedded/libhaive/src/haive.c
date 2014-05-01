/*
 * Copyright 2014 - Greg Prisament
 */
#include "haive.h"
#include "haive_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include <unistd.h>

HaiveContext haive_init()
{
    HaiveContext ctx = NULL;
    ctx = calloc(1, sizeof(HaiveContext));
    if (!ctx)
    {
        /* TODO: set error */
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
        /*RedHash_Free(ctx->properties);*/
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

    report->ctx = haive; /* TODO: refcnt? */

    return report;
fail:
    if (report)
    {
        /*RedHash_Free(report->values);*/
        free(report);
    }
    return NULL;
}

bool haive_report_i8(HaiveReport report, const char *parameter, int8_t value)
{
    /* First verify that value is acceptable */
    HaiveContext ctx = report->ctx;
    _HaiveProperty *prop;
    _HaivePropertyValue *propval;
    _HaivePropertyValue *oldValue;

    if (report->finished)
    {
        /* haive_end_report already called, cannot make further changes. */
        return false;
    }
    prop = RedHash_GetWithDefaultS(ctx->properties, parameter, NULL);
    if (!prop)
    {
        /* property not found! */
        return false;
    }
    else if (prop->datatype != HAIVE_DATATYPE_INT8)
    {
        /* incorrect datatype */
        return false;
    }
    else if (prop->hasMinRange && value < prop->rangeMin)
    {
        /* value too low */
        return false;
    }
    else if (prop->hasMaxRange && value > prop->rangeMax)
    {
        /* value too large */
        return false;
    }

    /* create property value object */
    propval = calloc(1, sizeof(_HaivePropertyValue));
    if (!propval)
    {
        /* allocation failed */
        return false;
    }

    propval->val.val_int8 = value;
    
    /* Add it to report's hash table */
    if (RedHash_UpdateOrInsertS(report->values, (void **)&oldValue, parameter, propval))
    {
        free(oldValue);
    }
    return true;
}

bool haive_report_float32(HaiveReport report, const char *parameter, float value)
{
    /* First verify that value is acceptable */
    HaiveContext ctx = report->ctx;
    _HaiveProperty *prop;
    _HaivePropertyValue *propval;
    _HaivePropertyValue *oldValue;

    if (report->finished)
    {
        fprintf(stderr, "haive_end_report already called, cannot make further changes.");
        return false;
    }
    prop = RedHash_GetWithDefaultS(ctx->properties, parameter, NULL);
    if (!prop)
    {
        fprintf(stderr, "property not found!");
        return false;
    }
    else if (prop->datatype != HAIVE_DATATYPE_INT8)
    {
        fprintf(stderr, "incorrect datatype");
        return false;
    }
    else if (prop->hasMinRange && value < prop->rangeMin)
    {
        fprintf(stderr, "value too low");
        return false;
    }
    else if (prop->hasMaxRange && value > prop->rangeMax)
    {
        fprintf(stderr, "value too large");
        return false;
    }

    /* create property value object */
    propval = calloc(1, sizeof(_HaivePropertyValue));
    if (!propval)
    {
        /* allocation failed */
        return false;
    }

    propval->val.val_float32 = value;
    
    /* Add it to report's hash table */
    if (RedHash_UpdateOrInsertS(report->values, (void **)&oldValue, parameter, propval))
    {
        free(oldValue);
    }
    return true;
}

bool haive_send_report(HaiveReport report)
{
    /* construct websocket message */
    RedHashIterator_t iter;
    const void *key;
    size_t keySize;
    const void * value;
    RED_HASH_FOREACH(iter, report->values, &key, &keySize, &value)
    {
        printf("Property `%s` has value: %f\n", (char *)key, ((_HaivePropertyValue *)value)->val.val_float32);
    }
    return false;
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
    long filesize;
    char *buffer;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file); 
    fseek(file, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, file);
    haive_load_device_description_string(haive, buffer);
    free(buffer);
    return true;
}

bool haive_event_loop(HaiveContext haive)
{
    while (!haive->quitRequested)
    {
        if (haive->cb)
        {
            haive->cb(haive, HAIVE_EVENT_REPORT_REQUESTED, haive->cbExtra);
        }
        sleep(10);
    }
    return true;
}

void haive_quit(HaiveContext haive)
{
    haive->quitRequested = true;
}

void haive_shutdown(HaiveContext haive)
{
    free(haive);
}
