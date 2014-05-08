/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy.h"
#include "canopy_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include <unistd.h>
#include <assert.h>

CanopyContext canopy_init()
{
    CanopyContext ctx = NULL;
    ctx = calloc(1, sizeof(CanopyContext));
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

bool canopy_register_event_callback(CanopyContext canopy, CanopyEventCallbackRoutine fn, void *extra)
{
    canopy->cb = fn;
    canopy->cbExtra = extra;
    return true;
}


CanopyReport canopy_begin_report(CanopyContext canopy)
{
    CanopyReport report = NULL;
    report = calloc(1, sizeof(CanopyReport));
    if (!report)
    {
        goto fail;
    }

    report->values = RedHash_New(0);
    if (!report->values)
    {
        goto fail;
    }

    report->ctx = canopy; /* TODO: refcnt? */

    return report;
fail:
    if (report)
    {
        /*RedHash_Free(report->values);*/
        free(report);
    }
    return NULL;
}

bool canopy_report_i8(CanopyReport report, const char *parameter, int8_t value)
{
    /* First verify that value is acceptable */
    CanopyContext ctx = report->ctx;
    _CanopyProperty *prop;
    _CanopyPropertyValue *propval;
    _CanopyPropertyValue *oldValue;

    if (report->finished)
    {
        /* canopy_end_report already called, cannot make further changes. */
        return false;
    }
    prop = RedHash_GetWithDefaultS(ctx->properties, parameter, NULL);
    if (!prop)
    {
        /* property not found! */
        return false;
    }
    else if (prop->datatype != CANOPY_DATATYPE_INT8)
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
    propval = calloc(1, sizeof(_CanopyPropertyValue));
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

bool canopy_report_float32(CanopyReport report, const char *parameter, float value)
{
    /* First verify that value is acceptable */
    CanopyContext ctx = report->ctx;
    _CanopyProperty *prop;
    _CanopyPropertyValue *propval;
    _CanopyPropertyValue *oldValue;

    if (report->finished)
    {
        fprintf(stderr, "canopy_end_report already called, cannot make further changes.");
        return false;
    }
    prop = RedHash_GetWithDefaultS(ctx->properties, parameter, NULL);
    if (!prop)
    {
        fprintf(stderr, "property not found!");
        return false;
    }
    else if (prop->datatype != CANOPY_DATATYPE_FLOAT32)
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
    propval = calloc(1, sizeof(_CanopyPropertyValue));
    if (!propval)
    {
        /* allocation failed */
        return false;
    }

    propval->datatype = CANOPY_DATATYPE_FLOAT32;
    propval->val.val_float32 = value;
    
    /* Add it to report's hash table */
    if (RedHash_UpdateOrInsertS(report->values, (void **)&oldValue, parameter, propval))
    {
        free(oldValue);
    }
    return true;
}

bool canopy_send_report(CanopyReport report)
{
    /* construct websocket message */
    RedHashIterator_t iter;
    const void *key;
    size_t keySize;
    const void * value;

    RedJsonObject jsonObj = RedJsonObject_New();
    RED_HASH_FOREACH(iter, report->values, &key, &keySize, &value)
    {
        _CanopyPropertyValue * propVal = (_CanopyPropertyValue *)value;
        switch (propVal->datatype)
        {
            case CANOPY_DATATYPE_FLOAT32:
            {
                RedJsonObject_SetNumber(jsonObj, key, propVal->val.val_float32);
                break;
            }
            default:
                assert(!"unimplemented datatype");
        }
    }
    printf("%s\n", RedJsonObject_ToJsonString(jsonObj));
    return false;
}

bool canopy_load_device_description(CanopyContext canopy, const char *filename, const char *descriptionName)
{
    FILE *fp;
    bool result;
    fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }
    result = canopy_load_device_description_file(canopy, fp, descriptionName);
    fclose(fp);
    return result;
}

bool canopy_load_device_description_file(CanopyContext canopy, FILE *file, const char *descriptionName)
{
    /* Read entire file into memory */
    long filesize;
    char *buffer;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file); 
    fseek(file, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, file);
    canopy_load_device_description_string(canopy, buffer, descriptionName);
    free(buffer);
    return true;
}

bool canopy_event_loop(CanopyContext canopy)
{
    int cnt = 0;
    while (!canopy->quitRequested)
    {
        if (canopy->cb && (cnt % 10 == 0))
        {
            CanopyEventDetails event;
            event = calloc(1, sizeof(CanopyEventDetailsStruct));
            event->eventType = CANOPY_EVENT_REPORT_REQUESTED;
            event->userData = canopy->cbExtra;
            canopy->cb(canopy, event);
            free(event);
        }
        libwebsocket_service(canopy->ws_ctx, 800);
        sleep(1);
        cnt++;
    }
    libwebsocket_context_destroy(canopy->ws_ctx);
    return true;
}

void canopy_quit(CanopyContext canopy)
{
    canopy->quitRequested = true;
}

void canopy_shutdown(CanopyContext canopy)
{
    free(canopy);
}
