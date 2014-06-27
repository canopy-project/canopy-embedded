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

bool _canopy_load_system_config(CanopyContext ctx)
{
    FILE * fp = canopy_open_config_file("canopy.conf");
    char *buffer;
    size_t filesize;
    char **keys;
    unsigned i;
    RedJsonObject confObj;
    if (!fp)
    {
        RedLog_ErrorLog("Could not locate canopy.conf", "");
        return false;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp); 
    fseek(fp, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, fp);
    fclose(fp);

    confObj = RedJson_Parse(buffer);
    if (!confObj)
    {
        RedLog_ErrorLog("Error parsing canopy.conf", "");
        return false;
    }

    keys = RedJsonObject_NewKeysArray(confObj);
    for (i = 0; i < RedJsonObject_NumItems(confObj); i++)
    {
        if (!strcmp(keys[i], "cloud-host"))
        {
            if (!RedJsonObject_IsValueString(confObj, keys[i]))
            {
                RedLog_WarnLog("canopy.conf -- expected string for \"cloud-host\"", "");
                continue;
            }
            if (ctx->cloudHost)
                free(ctx->cloudHost);
            ctx->cloudHost = RedJsonObject_GetString(confObj, keys[i]);
        }
        else if (!strcmp(keys[i], "cloud-port"))
        {
            if (!RedJsonObject_IsValueNumber(confObj, keys[i]))
            {
                RedLog_WarnLog("canopy.conf -- expected number for \"cloud-port\"", "");
                continue;
            }
            ctx->cloudPort = RedJsonObject_GetNumber(confObj, keys[i]);
        }
        else
        {
            RedLog_WarnLog("canopy.conf -- unrecognized field: %s", keys[i]);
            continue;
        }
    }
    RedJsonObject_FreeKeysArray(keys);
    
    free(buffer);

    return true;
}
