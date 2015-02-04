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

#ifndef ST_LOG_INCLUDED
#define ST_LOG_INCLUDED

// Logging utility library for Canopy.

#include <canopy.h>
#include <stdbool.h>

typedef enum
{
    ST_LOG_LEVEL_TRACE = 0x1,
    ST_LOG_LEVEL_DEBUG = 0x2,
    ST_LOG_LEVEL_INFO = 0x4,
    ST_LOG_LEVEL_WARN = 0x8,
    ST_LOG_LEVEL_ERROR = 0x10,
    ST_LOG_LEVEL_FATAL = 0x20
} STLogLevel;

#define ST_LOG_LEVEL_ERROR_AND_HIGHER \
        (ST_LOG_LEVEL_ERROR | ST_LOG_LEVEL_FATAL)

#define ST_LOG_LEVEL_WARN_AND_HIGHER \
        (ST_LOG_LEVEL_WARN | ST_LOG_LEVEL_ERROR_AND_HIGHER)

#define ST_LOG_LEVEL_INFO_AND_HIGHER \
        (ST_LOG_LEVEL_INFO | ST_LOG_LEVEL_WARN_AND_HIGHER)

#define ST_LOG_LEVEL_DEBUG_AND_HIGHER \
        (ST_LOG_LEVEL_DEBUG | ST_LOG_LEVEL_INFO_AND_HIGHER)

#define ST_LOG_LEVEL_ALL \
        (ST_LOG_LEVEL_TRACE | ST_LOG_LEVEL_DEBUG_AND_HIGHER)

void st_log_init();

// Enable or disable Canopy logging overall.
CanopyResultEnum st_log_set_enabled(bool enabled);

// Set the filename to write to.
// This is in addition to the system default output if
// st_log_use_system_default is enabled.
// This may fail and return CANOPY_ERROR_NOT_IMPLEMENTED on some platforms.
CanopyResultEnum st_log_set_filename(const char *filename);

// Enable/disable logging to the system's default output destination.
// For linux, this is stderr.  Defaults to true.
CanopyResultEnum st_log_use_system_default(bool enabled);

// Set the enabled logging levels.
CanopyResultEnum st_log_set_levels(int levels);

// Enable/Disable logging of communication payloads (HTTP & Websocket).
CanopyResultEnum st_log_set_payload_logging(bool enabled);

// Generic logging routine for Canopy
// <file> is the source filename where the logging occurs
// <line> is the source line number
// <level> must be exactly one of the log levels defined in STLogLevel enum.
// <fmt> and following arguments 
CanopyResultEnum st_log_common(const char *file, int line, STLogLevel level, const char *fmt, ...);

#define st_log_trace(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define st_log_debug(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define st_log_info(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#define st_log_warn(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

#define st_log_error(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#define st_log_fatal(fmt, ...)  \
    st_log_common(__FILE__, __LINE__, ST_LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#endif // ST_LOG_INCLUDED


