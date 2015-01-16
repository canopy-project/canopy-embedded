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

// canopy.c
// This file implements the API entrypoints declared in canopy.h

#include <canopy.h>
#include <assert.h>
#include "cloudvar/st_cloudvar.h"
#include "http/st_http.h"
#include "log/st_log.h"
#include "options/st_options.h"
#include "sync/st_sync.h"
#include "websocket/st_websocket.h"
#include "red_json.h"
#include "red_string.h"
#include "red_hash.h"
#include "red_log.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct _Global_t
{
    bool initialized;
    STGlobalOptions options;
    STLogger logger;
} _Global_t;

static _Global_t _global;

typedef struct CanopyContext_t
{
    STOptions options;

    STCloudVarSystem cloudvars;

    STWebSocket ws;

} CanopyContext_t;

static CanopyResultEnum _global_init()
{
    // TODO: thread safety?
    // TODO: error handling

    // Already initialized?
    if (_global.initialized)
        return CANOPY_SUCCESS;

    _global.options = st_global_options_new_default();
    if (!_global.options)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    st_global_options_load_from_env(_global.options);

    _global.logger = st_log_init();
    if (!_global.logger)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    // Setup logging
    st_log_set_enabled(_global.logger, _global.options->val_CANOPY_LOG_ENABLED);
    st_log_set_filename(_global.logger, _global.options->val_CANOPY_LOG_FILE);
    st_log_set_level(_global.logger, _global.options->val_CANOPY_LOG_LEVEL);
    st_log_set_payload_logging(_global.logger, _global.options->val_CANOPY_LOG_PAYLOADS);

    _global.initialized = true;
    return CANOPY_SUCCESS;
}

bool canopy_once_every(uint64_t *timer, uint64_t us) {
    // Timer holds the start time.
    struct timespec t;
    uint64_t curtime;
    clock_gettime(CLOCK_MONOTONIC, &t);
    curtime = t.tv_sec*CANOPY_SECONDS + (t.tv_nsec/1000);
    if (curtime > (*timer + us))
    {
        *timer = curtime;
        return true;
    }
    return false;
}

CanopyContext canopy_init_context()
{
    CanopyContext ctx;
    CanopyResultEnum result;

    // Initialize the global system-wide struct
    result = _global_init();
    if (result != CANOPY_SUCCESS)
    {
        return NULL;
    }
    
    st_log_trace("canopy_init_context");

    ctx = calloc(1, sizeof(struct CanopyContext_t));
    if (!ctx)
    {
        return NULL;
    }

    ctx->options = st_options_new_default();
    if (!ctx->options)
    {
        RedLog_Error("OOM in canopy_create_ctx");
        goto fail;
    }

    st_options_load_from_env(ctx->options);

    ctx->ws = st_websocket_new();
    if (!ctx->options)
    {
        RedLog_Error("OOM in canopy_create_ctx");
        goto fail;
    }

    ctx->cloudvars = st_cloudvar_system_new(ctx);
    if (!ctx->cloudvars)
    {
        RedLog_Error("OOM in canopy_create_ctx");
        goto fail;
    }

    return ctx;
fail:
    canopy_shutdown_context(ctx);
    return NULL;
}

CanopyResultEnum canopy_shutdown_context(CanopyContext ctx)
{
    st_log_trace("canopy_shutdown_context(0x%p)", ctx);
    if (ctx)
    {
        st_options_free(ctx->options);
        st_websocket_free(ctx->ws);
        st_cloudvar_system_free(ctx->cloudvars);
        free(ctx);
    }
    return CANOPY_SUCCESS;
}

CanopyResultEnum canopy_set_global_opt_impl(void *dummy, ...)
{
    va_list ap;
    CanopyResultEnum result;

    // Initialize the global system-wide struct
    result = _global_init();
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }
    st_log_trace("canopy_set_opt_impl");
    va_start(ap, dummy);
    result = st_global_options_extend_varargs(_global.options, ap);
    va_end(ap);

    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    // setup logging
    st_log_set_enabled(_global.logger, _global.options->val_CANOPY_LOG_ENABLED);
    st_log_set_filename(_global.logger, _global.options->val_CANOPY_LOG_FILE);
    st_log_set_level(_global.logger, _global.options->val_CANOPY_LOG_LEVEL);
    st_log_set_payload_logging(_global.logger, _global.options->val_CANOPY_LOG_PAYLOADS);
    return CANOPY_SUCCESS;
}

CanopyVarInitObject CANOPY_INIT_FIELD_IMPL(const char *decl, ...)
{
    va_list ap;
    CanopyVarInitObject out;
    st_log_trace("CANOPY_INIT_FIELD_IMPL");
    va_start(ap, decl);
    out = st_cloudvar_init_field(decl, ap);
    va_end(ap);
    return out;
}

CanopyResultEnum canopy_set_opt_impl(CanopyContext ctx, ...)
{
    va_list ap;
    CanopyResultEnum out;
    st_log_trace("canopy_set_opt_impl");
    va_start(ap, ctx);
    out = st_options_extend_varargs(ctx->options, ap);
    va_end(ap);
    return out;
}
CanopyVarValue CANOPY_VALUE_BOOL(bool x)
{
    st_log_trace("CANOPY_VALUE_BOOL(%d)", x);
    return st_cloudvar_value_bool(x);
}

CanopyVarValue CANOPY_VALUE_FLOAT32(float x)
{
    st_log_trace("CANOPY_VALUE_FLOAT32(%f)", x);
    return st_cloudvar_value_float32(x);
}

CanopyVarValue CANOPY_VALUE_FLOAT64(double x)
{
    st_log_trace("CANOPY_VALUE_FLOAT64(%f)", x);
    return st_cloudvar_value_float64(x);
}

CanopyVarValue CANOPY_VALUE_INT8(int8_t x)
{
    st_log_trace("CANOPY_VALUE_INT8(%d)", x);
    return st_cloudvar_value_int8(x);
}

CanopyVarValue CANOPY_VALUE_UINT8(uint8_t x)
{
    st_log_trace("CANOPY_VALUE_UINT8(%f)", x);
    return st_cloudvar_value_uint8(x);
}
CanopyVarValue CANOPY_VALUE_INT16(int16_t x)
{
    st_log_trace("CANOPY_VALUE_INT16(%d)", x);
    return st_cloudvar_value_int16(x);
}

CanopyVarValue CANOPY_VALUE_UINT16(uint16_t x)
{
    st_log_trace("CANOPY_VALUE_UINT16(%f)", x);
    return st_cloudvar_value_uint16(x);
}

CanopyVarValue CANOPY_VALUE_INT32(int32_t x)
{
    st_log_trace("CANOPY_VALUE_INT32(%d)", x);
    return st_cloudvar_value_int32(x);
}

CanopyVarValue CANOPY_VALUE_UINT32(uint32_t x)
{
    st_log_trace("CANOPY_VALUE_UINT32(%f)", x);
    return st_cloudvar_value_uint32(x);
}

CanopyVarValue CANOPY_VALUE_STRING(const char *sz)
{
    st_log_trace("CANOPY_VALUE_STRING(%s)", sz);
    return st_cloudvar_value_string(sz);
}

CanopyVarValue CANOPY_VALUE_STRUCT_IMPL(void * dummy, ...)
{
    va_list ap;
    CanopyVarValue out;
    st_log_trace("CANOPY_STRUCT(...)");
    va_start(ap, dummy);
    out = st_cloudvar_value_struct(ap);
    va_end(ap);
    return out;
}

CanopyVarValue CANOPY_VALUE_ARRAY_IMPL(void * dummy, ...)
{
    va_list ap;
    CanopyVarValue out;
    st_log_trace("CANOPY_VALUE_ARRAY(...)");
    va_start(ap, dummy);
    out = st_cloudvar_value_array(ap);
    va_end(ap);
    return out;
}

void canopy_var_value_free(CanopyVarValue value)
{
    st_log_trace("canopy_var_value_free");
    return st_cloudvar_value_free(value);
}

CanopyResultEnum canopy_var_set(CanopyContext ctx, const char *varname, CanopyVarValue value)
{
    CanopyResultEnum result;
    STCloudVar var;
    st_log_trace("canopy_var_set(0x%p, %s, ...", ctx, varname);
    if (st_cloudvar_value_already_used(value))
    {
        // CanopyVarValue objects are meant to be used once.  If it has been
        // used already, throw an error.
        return CANOPY_ERROR_SINGLE_USE_VALUE_ALREADY_USED;
    }

    var = st_cloudvar_system_lookup_var(ctx->cloudvars, varname);
    if (!var)
    {
        return CANOPY_ERROR_VARIABLE_NOT_INITIALIZED;
    }

    result = st_cloudvar_set_var(var, value);

    // Mark <value> as used, since it is intended to be single-use.
    // This allows, for example:
    //      canopy_set_var(ctx, "foo", CANOPY_FLOAT32(100.0f)) 
    //  to not leak any memory.
    //  Although, it does result in an alloc and free for each call.
    st_cloudvar_value_mark_used(value);
    return result;
}

CanopyVarReader CANOPY_READ_BOOL(bool *dest)
{
    st_log_trace("CANOPY_READ_BOOL(0x%p)", dest);
    return st_cloudvar_reader_bool(dest);
}
CanopyVarReader CANOPY_READ_FLOAT32(float *dest)
{
    st_log_trace("CANOPY_READ_FLOAT32(0x%p)", dest);
    return st_cloudvar_reader_float32(dest);
}
CanopyVarReader CANOPY_READ_FLOAT64(double *dest)
{
    st_log_trace("CANOPY_READ_FLOAT64(0x%p)", dest);
    return st_cloudvar_reader_float64(dest);
}
CanopyVarReader CANOPY_READ_INT8(int8_t *dest)
{
    st_log_trace("CANOPY_READ_INT8(0x%p)", dest);
    return st_cloudvar_reader_int8(dest);
}
CanopyVarReader CANOPY_READ_INT16(int16_t *dest)
{
    st_log_trace("CANOPY_READ_INT16(0x%p)", dest);
    return st_cloudvar_reader_int16(dest);
}
CanopyVarReader CANOPY_READ_INT32(int32_t *dest)
{
    st_log_trace("CANOPY_READ_INT32(0x%p)", dest);
    return st_cloudvar_reader_int32(dest);
}
CanopyVarReader CANOPY_READ_STRING(char **sz)
{
    st_log_trace("CANOPY_READ_STRING(0x%p)", sz);
    return st_cloudvar_reader_string(sz);
}
CanopyVarReader CANOPY_READ_UINT8(uint8_t *dest)
{
    st_log_trace("CANOPY_READ_UINT8(0x%p)", dest);
    return st_cloudvar_reader_uint8(dest);
}
CanopyVarReader CANOPY_READ_UINT16(uint16_t *dest)
{
    st_log_trace("CANOPY_READ_UINT16(0x%p)", dest);
    return st_cloudvar_reader_uint16(dest);
}
CanopyVarReader CANOPY_READ_UINT32(uint32_t *dest)
{
    st_log_trace("CANOPY_READ_UINT32(0x%p)", dest);
    return st_cloudvar_reader_uint32(dest);
}

CanopyVarReader CANOPY_READ_STRUCT_IMPL(void * dummy, ...)
{
    va_list ap;
    CanopyVarReader out;
    st_log_trace("CANOPY_READ_STRUCT(...)");
    va_start(ap, dummy);
    out = st_cloudvar_reader_struct(ap);
    va_end(ap);
    return out;
}
CanopyVarReader CANOPY_READ_ARRAY_IMPL(void * dummy, ...)
{
    va_list ap;
    CanopyVarReader out;
    st_log_trace("CANOPY_READ_ARRAY(...)");
    va_start(ap, dummy);
    out = st_cloudvar_reader_array(ap);
    va_end(ap);
    return out;
}

CanopyResultEnum canopy_var_get(CanopyContext ctx, const char *varname, CanopyVarReader dest)
{
    STCloudVar var;
    st_log_trace("canopy_var_get(...)");

    var = st_cloudvar_system_lookup_var(ctx->cloudvars, varname);
    if (!var)
    {
        return CANOPY_ERROR_VARIABLE_NOT_INITIALIZED;
    }

    return st_cloudvar_read_var(var, dest);
}

CanopyResultEnum canopy_var_on_change(CanopyContext ctx, const char *varname, CanopyOnChangeCallback cb, void *userdata)
{
    STCloudVar var;
    st_log_trace("canopy_var_on_change(...)");

    var = st_cloudvar_system_lookup_var(ctx->cloudvars, varname);
    if (!var)
    {
        return CANOPY_ERROR_VARIABLE_NOT_INITIALIZED;
    }

    return st_cloudvar_register_on_change_callback(var, cb, userdata);
}

CanopyResultEnum canopy_var_init_impl(CanopyContext ctx, const char *decl, ...)
{
    st_log_trace("canopy_var_init(...)");
    va_list ap;
    CanopyResultEnum result;

    va_start(ap, decl);
    result = st_cloudvar_init_var(ctx->cloudvars, decl, ap);
    va_end(ap);

    return result;
}

CanopyResultEnum canopy_sync_blocking(CanopyContext ctx, int timeout_us)
{
    // TODO: don't ignore timeout_us!
    st_log_trace("canopy_sync_blocking(...)");
    return st_sync(ctx, ctx->options, ctx->ws, ctx->cloudvars);
}


CanopyResultEnum canopy_sync(CanopyContext ctx, CanopyPromise promise)
{
    st_log_trace("canopy_sync(...)");
    return st_sync(ctx, ctx->options, ctx->ws, ctx->cloudvars);
}

void canopy_debug_dump_opts(CanopyContext ctx)
{
    RedStringList out = RedStringList_New();

    st_log_trace("canopy_debug_dump_opts(0x%p)", ctx);
    // TODO: would like to use OPTION_LIST expansion macro here, but dealing
    // with the different datatypes in the printf is tricky.
    RedStringList_AppendPrintf(out, "\n\n");
    RedStringList_AppendPrintf(out, "Global Settings\n");
    RedStringList_AppendPrintf(out, "----------------------\n");

    if (_global.options->has_CANOPY_LOG_ENABLED)
        RedStringList_AppendPrintf(out, "LOG_ENABLED: %d\n", 
                _global.options->val_CANOPY_LOG_ENABLED);
    else
        RedStringList_AppendPrintf(out, "LOG_ENABLED: <undefined>\n");

    if (_global.options->has_CANOPY_LOG_FILE)
        RedStringList_AppendPrintf(out, "LOG_FILE: %s\n", 
                _global.options->val_CANOPY_LOG_FILE);
    else
        RedStringList_AppendPrintf(out, "LOG_FILE: <undefined>\n");

    if (_global.options->has_CANOPY_LOG_LEVEL)
        RedStringList_AppendPrintf(out, "LOG_LEVEL: %d\n", 
                _global.options->val_CANOPY_LOG_LEVEL);
    else
        RedStringList_AppendPrintf(out, "LOG_LEVEL: <undefined>\n");

    if (_global.options->has_CANOPY_LOG_PAYLOADS)
        RedStringList_AppendPrintf(out, "LOG_PAYLOADS: %d\n", 
                _global.options->val_CANOPY_LOG_PAYLOADS);
    else
        RedStringList_AppendPrintf(out, "LOG_PAYLOADS: <undefined>\n");

    RedStringList_AppendPrintf(out, "\n\n");
    RedStringList_AppendPrintf(out, "Context 0x%p settings\n", ctx);
    RedStringList_AppendPrintf(out, "----------------------\n");
    RedStringList_AppendPrintf(out, "CLOUD_SERVER: %s\n", 
            ctx->options->has_CANOPY_CLOUD_SERVER ?
                ctx->options->val_CANOPY_CLOUD_SERVER : "<undefined>");
    RedStringList_AppendPrintf(out, "DEVICE_UUID: %s\n", 
            ctx->options->has_CANOPY_DEVICE_UUID ?
                ctx->options->val_CANOPY_DEVICE_UUID : "<undefined>");

    if (ctx->options->has_CANOPY_VAR_SEND_PROTOCOL)
        RedStringList_AppendPrintf(out, "VAR_SEND_PROTOCOL: %d\n", 
                ctx->options->val_CANOPY_VAR_SEND_PROTOCOL);
    else
        RedStringList_AppendPrintf(out, "VAR_SEND_PROTOCOL: <undefined>\n");

    if (ctx->options->has_CANOPY_SYNC_BLOCKING)
        RedStringList_AppendPrintf(out, "SYNC_BLOCKING: %d\n", 
                ctx->options->val_CANOPY_SYNC_BLOCKING);
    else
        RedStringList_AppendPrintf(out, "SYNC_BLOCKING: <undefined>\n");

    if (ctx->options->has_CANOPY_SYNC_TIMEOUT_MS)
        RedStringList_AppendPrintf(out, "SYNC_TIMEOUT_MS: %d\n", 
                ctx->options->val_CANOPY_SYNC_TIMEOUT_MS);
    else
        RedStringList_AppendPrintf(out, "SYNC_TIMEOUT_MS: <undefined>\n");

    RedStringList_AppendPrintf(out, "\n\n");

    char *outsz = RedStringList_ToNewChars(out);
    st_log_info("%s", outsz);
    free(outsz);
    RedStringList_Free(out);

}
