#include "red_log.h"
#include "red_hash.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define _NUM_LOG_LEVELS 6

typedef struct _Logger_t
{
    bool levelEnabled[_NUM_LOG_LEVELS];
    RedLogCallbackFunc callback[_NUM_LOG_LEVELS];
} _Logger_t;

typedef struct _LogSystem_t
{
    bool initialized;
    RedHash loggers; /* logName-> _RedLogParms_t */
} _LogSystem_t;

static _LogSystem_t sRedLogSys;

static void _CleanupOnExit()
{
    /* TODO */
}

static _Logger_t * _CreateOrGetLogger(const char *loggerName)
{
    _Logger_t * logger;
    int i;
    logger = RedHash_GetWithDefaultS(sRedLogSys.loggers, loggerName, NULL);
    if (!logger)
    {
        logger = calloc(1, sizeof(_Logger_t));
        if (logger)
        {
            for (i = 0; i < _NUM_LOG_LEVELS; i++)
            {
                logger->levelEnabled[i] = true;
                logger->callback[i] = RedLog_WriteToStderrRoutine;
            }
        }
    }
    if (logger)
    {
        RedHash_InsertS(sRedLogSys.loggers, loggerName, logger);
    }
    return logger;
}

static void _InitializeIfNeeded()
{
    if (!sRedLogSys.initialized)
    {
        sRedLogSys.initialized = true;
        sRedLogSys.loggers = RedHash_New(0);
        _CreateOrGetLogger("__default__");
        atexit(_CleanupOnExit);
    }
}

static const char * _LogLevelString(RedLogLevel level)
{
    switch (level)
    {
        case RED_LOG_LEVEL_TRACE:
            return "TRACE";
        case RED_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case RED_LOG_LEVEL_INFO:
            return "INFO";
        case RED_LOG_LEVEL_WARN:
            return "WARNING";
        case RED_LOG_LEVEL_ERROR:
            return "ERROR";
        case RED_LOG_LEVEL_FATAL:
            return "FATAL ERROR";
        default:
            return 0;
    }
}


static void _WriteToStderrRoutine(const char *file, int line, const char *loggerName, RedLogLevel level, const char *msg)
{
    fprintf(stderr, "%s:%d [%s %s] %s\n", file, line, loggerName, _LogLevelString(level), msg);
}

RedLogCallbackFunc RedLog_WriteToStderrRoutine = _WriteToStderrRoutine;

static int _LogLevelIdx(RedLogLevel level)
{
    switch (level)
    {
        case RED_LOG_LEVEL_TRACE:
            return 0;
        case RED_LOG_LEVEL_DEBUG:
            return 1;
        case RED_LOG_LEVEL_INFO:
            return 2;
        case RED_LOG_LEVEL_WARN:
            return 3;
        case RED_LOG_LEVEL_ERROR:
            return 4;
        case RED_LOG_LEVEL_FATAL:
            return 5;
        default:
            return 0;
    }
}


void RedLog_SetLogLevelsEnabled(const char *loggerName, RedLogLevels levels)
{
    int i;
    _Logger_t *logger;
    logger = _CreateOrGetLogger(loggerName);
    for (i = 0; i < _NUM_LOG_LEVELS; i++)
    {
        logger->levelEnabled[i] = (levels & (1 << i)) ? true : false;
    }
}

void RedLog_SetLogCallback(const char *loggerName, RedLogLevels levels, RedLogCallbackFunc handler)
{
    int i;
    _Logger_t *logger;
    logger = _CreateOrGetLogger(loggerName);
    for (i = 0; i < _NUM_LOG_LEVELS; i++)
    {
        if (levels & (1 << i))
        {
            logger->callback[i] = handler;
        }
    }
}

void RedLog_LogCommon(const char *file, int line, const char *logName, RedLogLevel level, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    RedLog_Logv(file, line, logName, level, fmt, ap);
    va_end(ap);
}

void RedLog_Logv(const char *file, int line, const char *logName, RedLogLevel level, const char *fmt, va_list ap)
{
    _InitializeIfNeeded();
    _Logger_t * logger;
    int levelIdx = _LogLevelIdx(level);

    /* determine which logging routine to use based on logName and level:  */
    if (logName && RedHash_HasKeyS(sRedLogSys.loggers, logName))
    {
        logger = RedHash_GetS(sRedLogSys.loggers, logName);
    }
    else
    {
        logger = RedHash_GetS(sRedLogSys.loggers, "__default__");
    }

    if (logger->levelEnabled[levelIdx])
    {
        char *msg;
        va_list ap2;
        int n;
        va_copy(ap2, ap);
        n = vsnprintf(NULL, 0, fmt, ap);
        msg = (char *)calloc(n+1, sizeof(char));
        vsnprintf(msg, n+1, fmt, ap2);
        logger->callback[levelIdx](file, line, logName, level, msg);
    }
}
