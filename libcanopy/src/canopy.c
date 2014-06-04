/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy.h"
#include "canopy_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include "red_log.h"
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

bool canopy_set_device_id(CanopyContext canopy, const char *uuid)
{
    canopy->uuid = calloc(1, strlen(uuid)+1);
    if (!canopy->uuid)
    {
        return false;
    }
    strcpy(canopy->uuid, uuid);
    return true;
}

bool canopy_set_device_id_filename(CanopyContext canopy, const char *filename)
{
    FILE *fp;
    bool result;
    long filesize;
    char *buffer;

    fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp); 
    fseek(fp, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, fp);
    result = canopy_set_device_id(canopy, buffer);
    free(buffer);
    fclose(fp);

    return result;
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
    _CanopyPropertyValue *propval;
    _CanopyPropertyValue *oldValue;

    SDDLSensor sensor = sddl_class_lookup_sensor(ctx->sddl, parameter);
    if (!sensor)
    {
        RedLog_WarnLog("Device does not have sensor %s", parameter);
        return false;
    }
    if (report->finished)
    {
        /* canopy_end_report already called, cannot make further changes. */
        return false;
    }
    if (sddl_sensor_datatype(sensor) != SDDL_DATATYPE_INT8)
    {
        /* incorrect datatype */
        return false;
    }
    else if (sddl_sensor_min_value(sensor) && value < (int8_t)*sddl_sensor_min_value(sensor))
    {
        /* value too low */
        return false;
    }
    else if (sddl_sensor_max_value(sensor) && value > (int8_t)*sddl_sensor_max_value(sensor))
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

    propval->datatype = SDDL_DATATYPE_INT8;
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
    _CanopyPropertyValue *propval;
    _CanopyPropertyValue *oldValue;

    SDDLSensor sensor = sddl_class_lookup_sensor(ctx->sddl, parameter);
    if (!sensor)
    {
        RedLog_WarnLog("Device does not have sensor %s", parameter);
        return false;
    }
    if (report->finished)
    {
        /* canopy_end_report already called, cannot make further changes. */
        return false;
    }
    if (sddl_sensor_datatype(sensor) != SDDL_DATATYPE_FLOAT32)
    {
        /* incorrect datatype */
        return false;
    }
    else if (sddl_sensor_min_value(sensor) && value < (int8_t)*sddl_sensor_min_value(sensor))
    {
        /* value too low */
        return false;
    }
    else if (sddl_sensor_max_value(sensor) && value > (int8_t)*sddl_sensor_max_value(sensor))
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

    propval->datatype = SDDL_DATATYPE_FLOAT32;
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
            case SDDL_DATATYPE_FLOAT32:
            {
                RedJsonObject_SetNumber(jsonObj, key, propVal->val.val_float32);
                break;
            }
            default:
                assert(!"unimplemented datatype");
        }
    }

    RedJsonObject_SetString(jsonObj, "device_id", report->ctx->uuid);

    RedLog_DebugLog("canopy", "Sending Message: %s\n", RedJsonObject_ToJsonString(jsonObj));
    _canopy_ws_write(report->ctx, RedJsonObject_ToJsonString(jsonObj));
    return false;
}

bool canopy_load_device_description(CanopyContext canopy, const char *filename, const char *className)
{
    SDDLDocument doc;
    SDDLClass cls;

    doc = sddl_load_and_parse(filename);
    if (!doc)
    {
        RedLog_ErrorLog("canopy", "Failed to read SDDL file");
        return false;
    }

    cls = sddl_document_lookup_class(doc, className);
    if (!cls)
    {
        RedLog_ErrorLog("canopy", "Class not found in SDDL: %s", className);
        return false;
    }

    canopy->sddl = cls;
    return true;
}

bool canopy_load_device_description_file(CanopyContext canopy, FILE *file, const char *className)
{
    SDDLDocument doc;
    SDDLClass cls;

    doc = sddl_load_and_parse_file(file);
    if (!doc)
    {
        RedLog_ErrorLog("canopy", "Failed to read SDDL file");
        return false;
    }

    cls = sddl_document_lookup_class(doc, className);
    if (!cls)
    {
        RedLog_ErrorLog("canopy", "Class not found in SDDL: %s", className);
        return false;
    }

    canopy->sddl = cls;
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
            event->ctx = canopy;
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
