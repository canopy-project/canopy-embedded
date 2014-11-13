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

#ifndef ST_LOG_INCLUDED
#define ST_LOG_INCLUDED

#include <canopy.h>
#include "red_log.h"

// Logging utility library for Canopy.
// Implemented as a simple wrapper around RedLog.

typedef struct STLogger_t * STLogger;

STLogger st_log_init();
CanopyResultEnum st_log_set_enabled(STLogger logger, bool enabled);
CanopyResultEnum st_log_set_filename(STLogger logger, const char *filename);
CanopyResultEnum st_log_set_level(STLogger logger, int level);
CanopyResultEnum st_log_set_payload_logging(STLogger logger, bool enabled);

#define st_log_trace(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_TRACE, __VA_ARGS__)

#define st_log_debug(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_DEBUG, __VA_ARGS__)

#define st_log_info(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_INFO, __VA_ARGS__)

#define st_log_warn(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_WARN, __VA_ARGS__)

#define st_log_error(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_ERROR, __VA_ARGS__)

#define st_log_fatal(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, "canopy", RED_LOG_LEVEL_FATAL, __VA_ARGS__)

#endif // ST_LOG_INCLUDED


