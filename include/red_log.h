/*
 *  red_log.h - Logging Module
 *
 *  Author: Gregory Prsiament (greg@toruslabs.com)
 *
 *  ===========================================================================
 *  Creative Commons CC0 1.0 Universal - Public Domain 
 *
 *  To the extent possible under law, Gregory Prisament has waived all
 *  copyright and related or neighboring rights to RedTest. This work is
 *  published from: United States. 
 *
 *  For details please refer to either:
 *      - http://creativecommons.org/publicdomain/zero/1.0/legalcode
 *      - The LICENSE file in this directory, if present.
 *  ===========================================================================
 */

#ifndef RED_LOG_INCLUDED
#define RED_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
/* TODO: filename and line numbers */

typedef enum
{
    RED_LOG_LEVEL_TRACE = 0x1,
    RED_LOG_LEVEL_DEBUG = 0x2,
    RED_LOG_LEVEL_INFO = 0x4,
    RED_LOG_LEVEL_WARN = 0x8,
    RED_LOG_LEVEL_ERROR = 0x10,
    RED_LOG_LEVEL_FATAL = 0x20
} RedLogLevel;

#define RED_LOG_LEVEL_ERROR_AND_HIGHER 0x30
#define RED_LOG_LEVEL_WARN_AND_HIGHER 0x38
#define RED_LOG_LEVEL_INFO_AND_HIGHER 0x3C
#define RED_LOG_LEVEL_DEBUG_AND_HIGHER 0x3E
#define RED_LOG_LEVEL_ALL 0x3F

#define RED_LOG_DEFAULT_LOG "__default__"

typedef int RedLogLevels;

typedef void (*RedLogCallbackFunc)(const char *file, int line, const char *loggerName, RedLogLevel level, const char *msg, void *userData);

void RedLog_Logv(const char *file, int line, const char *logName, RedLogLevel level, const char *fmt, va_list ap);

void RedLog_LogCommon(const char *file, int line, const char *logName, RedLogLevel level, const char *fmt, ...);

#define RedLog_Log(loggerName, level, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, level, __VA_ARGS__)

#define RedLog_TraceLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_TRACE, __VA_ARGS__)

#define RedLog_DebugLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_DEBUG, __VA_ARGS__)

#define RedLog_InfoLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_INFO, __VA_ARGS__)

#define RedLog_WarnLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_WARN, __VA_ARGS__)

#define RedLog_ErrorLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_ERROR, __VA_ARGS__)

#define RedLog_FatalLog(loggerName, ...)  \
    RedLog_LogCommon(__FILE__, __LINE__, loggerName, RED_LOG_LEVEL_FATAL, __VA_ARGS__)

#define RedLog_Debug(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, RED_LOG_DEFAULT_LOG, RED_LOG_LEVEL_DEBUG, __VA_ARGS__)

#define RedLog_Info(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, RED_LOG_DEFAULT_LOG, RED_LOG_LEVEL_INFO, __VA_ARGS__)

#define RedLog_Warn(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, RED_LOG_DEFAULT_LOG, RED_LOG_LEVEL_WARN, __VA_ARGS__)

#define RedLog_Error(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, RED_LOG_DEFAULT_LOG, RED_LOG_LEVEL_ERROR, __VA_ARGS__)

#define RedLog_Fatal(...)  \
    RedLog_LogCommon(__FILE__, __LINE__, RED_LOG_DEFAULT_LOG, RED_LOG_LEVEL_FATAL, __VA_ARGS__)

void RedLog_SetLogCallback(const char *loggerName, RedLogLevels levels, RedLogCallbackFunc handler);
void RedLog_SetLogLevelsEnabled(const char *loggerName, RedLogLevels levels);
void RedLog_SetLogCallbackUserData(const char *loggerName, void *userData);

extern RedLogCallbackFunc RedLog_WriteToStderrRoutine;

const char * RedLog_LogLevelString(RedLogLevel level);

#ifdef __cplusplus
}
#endif

#endif
