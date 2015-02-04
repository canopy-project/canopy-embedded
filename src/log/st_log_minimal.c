// Copyright 2015 SimpleThings, Inc.
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

// Minimal implementation of st_log.h using canopy_os, intended for FreeRTOS.
#include "log/st_log.h"

#include <canopy_os.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct STLogger_t
{
    bool enabled;
    bool send_payloads;
    int levels;
} STLogger_t;

static STLogger_t g_logger;

void st_log_init()
{
    // TODO: only allow a singleton logger for now?
    g_logger.enabled = false;
    g_logger.send_payloads = false;
    g_logger.levels = 0;
}

CanopyResultEnum st_log_set_enabled(bool enabled)
{
    g_logger.enabled = enabled;
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_log_set_filename(const char *filename)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum st_log_set_payload_logging(bool enabled)
{
    // TBD
    g_logger.send_payloads = enabled;
    return CANOPY_SUCCESS;

}
CanopyResultEnum st_log_set_level(int levels)
{
    // TODO: VALIDATE PARAMETERS
    g_logger.levels = levels;
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_log_common(const char *file, int line, STLogLevel level, const char *fmt, ...)
{
    char _msg_buf[CANOPY_OS_MSG_MAX_LENGTH];
    char *levelString = NULL;
	va_list ap;

    // Is logging enabled overall?
    if (!g_logger.enabled) {
        return CANOPY_SUCCESS;
    }

    // Is logging for this particular log level enabled?
    if (!(g_logger.levels & level)) {
        return CANOPY_SUCCESS;
    }

    _msg_buf[0] = '\0';
	va_start(ap, fmt);
    canopy_os_vsnprintf(_msg_buf, CANOPY_OS_MSG_MAX_LENGTH, fmt, ap);
	va_end(ap);

    switch (level) {
        case ST_LOG_LEVEL_TRACE:
            levelString = "trace";
            break;
        case ST_LOG_LEVEL_INFO:
            levelString = "info";
            break;
        case ST_LOG_LEVEL_DEBUG:
            levelString = "debug";
            break;
        case ST_LOG_LEVEL_WARN:
            levelString = "warn";
            break;
        case ST_LOG_LEVEL_ERROR:
            levelString = "error";
            break;
        case ST_LOG_LEVEL_FATAL:
            levelString = "fatal";
            break;
        default:
            return CANOPY_ERROR_INVALID_VALUE;
    }

    canopy_os_log("%s:%d [%s] %s", file, line, levelString, _msg_buf);

    return CANOPY_SUCCESS;
}
