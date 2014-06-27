/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy.h"
#include "canopy_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include "red_log.h"
#include "red_string.h"
#include <unistd.h>
#include <assert.h>

CanopyContext canopy_init()
{
    CanopyContext ctx = NULL;
    bool result;
    char *uuid = NULL;
    ctx = calloc(1, sizeof(CanopyContext_t));
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

    _canopy_load_system_config(ctx);

    uuid = canopy_read_system_uuid();
    if (!uuid)
    {
        RedLog_Warn("Could not determine device UUID.");
        RedLog_Warn("Please run 'sudo cano uuid --install'.");
        RedLog_Warn("Or call canopy_set_device_id before canopy_connect");
    }
    else
    {
        result = canopy_set_device_id(ctx, uuid);
        if (!result)
        {
            RedLog_Error("Could not set UUID to %s", uuid);
            goto fail;
        }
    }

    free(uuid);
    return ctx;
fail:
    free(uuid);
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
    report = calloc(1, sizeof(CanopyReport_t));
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
    CanopyContext canopy = report->ctx;
    RedHashIterator_t iter;
    const void *key;
    size_t keySize;
    const void * value;
    RedJsonObject sddlJson;

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

    sddlJson = sddl_class_json(canopy->sddl);
    RedJsonObject_SetObject(jsonObj, "sddl", sddlJson);

    RedLog_DebugLog("canopy", "Sending Message: %s\n", RedJsonObject_ToJsonString(jsonObj));
    _canopy_ws_write(report->ctx, RedJsonObject_ToJsonString(jsonObj));
    return false;
}

bool canopy_load_sddl(CanopyContext canopy, const char *filename, const char *className)
{
    FILE *fp;
    bool out;
    fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }
    out = canopy_load_sddl_file(canopy, fp, className);
    fclose(fp);
    return out;
}

bool canopy_load_sddl_file(CanopyContext canopy, FILE *file, const char *className)
{
    /* Read entire file into memory */
    long filesize;
    char *buffer;
    bool out;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file); 
    fseek(file, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, file);
    out = canopy_load_sddl_string(canopy, buffer, className);
    free(buffer);
    return out;
}

bool canopy_load_sddl_string(CanopyContext canopy, const char *sddl, const char *className)
{
    SDDLDocument doc;
    SDDLClass cls;

    doc = sddl_parse(sddl);
    if (!doc)
    {
        RedLog_ErrorLog("canopy", "Failed to parse SDDL");
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
            event = calloc(1, sizeof(CanopyEventDetails_t));
            event->ctx = canopy;
            event->eventType = CANOPY_EVENT_REPORT_REQUESTED;
            event->userData = canopy->cbExtra;
            canopy->cb(canopy, event);
            free(event);
        }
        libwebsocket_service(canopy->ws_ctx, 1000);
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

FILE * canopy_open_config_file(const char* filename)
{
    FILE *fp;
    const char *canopyHome;
    const char *home;

    /* Try: $CANOPY_HOME/<filename> */
    canopyHome = getenv("CANOPY_HOME");
    if (canopyHome)
    {
        RedString fns = RedString_NewPrintf("%s/%s", 2048, canopyHome, filename);
        if (!fns)
            return NULL;
        fp = fopen(RedString_GetChars(fns), "r");
        RedString_Free(fns);
        if (fp)
            return fp;
    }

    /* Try: ~/.canopy/<filename> */
    home = getenv("HOME");
    if (home)
    {
        RedString fns = RedString_NewPrintf("%s/.canopy/%s", 2048, home, filename);
        if (!fns)
            return NULL;
        fp = fopen(RedString_GetChars(fns), "r");
        RedString_Free(fns);
        if (fp)
            return fp;
    }

    /* Try: SYSCONFIGDIR/<filename> */
#ifdef CANOPY_SYSCONFIGDIR
    {
        RedString fns = RedString_NewPrintf("%s/%s", 2048, CANOPY_SYSCONFIGDIR, filename);
        if (!fns)
            return NULL;
        fp = fopen(RedString_GetChars(fns), "r");
        RedString_Free(fns);
        if (fp)
            return fp;
    }
#endif

    /* Try /etc/canopy/<filename> */
    {
        RedString fns = RedString_NewPrintf("/etc/canopy/%s", 2048, filename);
        if (!fns)
            return NULL;
        fp = fopen(RedString_GetChars(fns), "r");
        RedString_Free(fns);
        if (fp)
            return fp;
    }

    return NULL;
}

char *canopy_read_system_uuid()
{
    char *uuidEnv;
    FILE *fp;
    char uuid[37];
    char *out;
    uuidEnv = getenv("CANOPY_UUID");
    if (uuidEnv)
    {
        /* TODO: Verify that it is, in fact, a UUID */
        return RedString_strdup(uuidEnv);
    }

    fp = canopy_open_config_file("uuid");
    if (fp)
    {
        size_t len;
        len = fread(uuid, sizeof(char), 36, fp);
        if (len != 36)
        {
            RedLog_WarnLog("Expected 36 characters in UUID file", "");
            return NULL;
        }
        uuid[36] = '\0';
        /* TODO: Verify that it is, in fact, a UUID */
        out = RedString_strdup(uuid);
        fclose(fp);
        return out;
    }
    return NULL;
}

