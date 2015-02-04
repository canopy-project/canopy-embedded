// Copyright 2014 SimpleThings, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "log/st_log.h"

#include <stdio.h>
#include <stdlib.h>
#include "red_string.h"

typedef struct STLogger_t
{
    bool enabled;
    bool send_payloads;
    char *filename;
    bool previously_failed_to_open;
} STLogger_t;

static void _log(const char *file, int line, const char *loggerName, RedLogLevel level, const char *msg, void *userData)
{
    FILE *fp;
    STLogger logger = (STLogger)userData;

    // If logging is disabled do nothing.
    if (!logger->enabled)
    {
        return;
    }

    // Attempt to open log file for write.
    fp = fopen(logger->filename, "a");
    if (!fp)
    {
        if (!logger->previously_failed_to_open)
        {
            fprintf(stderr, "WARNING: Failed to open log file: %s.  Make sure\n", logger->filename);
            fprintf(stderr, "that parent directory exists and check permissionss.\n");
            // Only display this warning once:
            logger->previously_failed_to_open = true;
        }
        return;
    }

    // Log the msg
    fprintf(fp, "%s:%d [%s %s] %s\n", file, line, loggerName, RedLog_LogLevelString(level), msg);

    // Clean up
    fclose(fp);
}

STLogger st_log_init()
{
    // TODO: only allow a singleton logger for now?
    STLogger out;
    out = calloc(1, sizeof(struct STLogger_t));
    if (!out)
    {
        return NULL;
    }
    RedLog_SetLogCallbackUserData("canopy", out);
    RedLog_SetLogCallback("canopy", RED_LOG_LEVEL_ALL, _log);
    return out;
}

CanopyResultEnum st_log_set_enabled(STLogger logger, bool enabled)
{
    logger->enabled = enabled;
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_log_set_filename(STLogger logger, const char *filename)
{
    free(logger->filename);
    logger->filename = RedString_strdup(filename);
    if (!logger->filename)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_log_set_payload_logging(STLogger logger, bool enabled)
{
    logger->send_payloads = enabled;
    return CANOPY_SUCCESS;

}
CanopyResultEnum st_log_set_level(STLogger logger, int levels)
{
    RedLog_SetLogLevelsEnabled("canopy", levels);
    return CANOPY_SUCCESS;
}

void st_log_common(const char *file, int line, STLogLevel level, const char *fmt, ...)
{

}
