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
#include <unistd.h>

typedef struct CanopyContext_t
{
    STOptions options;

    STCloudVarSystem cloudvars;

    STWebSocket ws;

} CanopyContext_t;

CanopyContext canopy_init_context()
{
    CanopyContext ctx;

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

    ctx->cloudvars = st_cloudvar_system_new();
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
    if (ctx)
    {
        st_options_free(ctx->options);
        st_websocket_free(ctx->ws);
        st_cloudvar_system_free(ctx->cloudvars);
        free(ctx);
    }
    return CANOPY_SUCCESS;
}

CanopyResultEnum canopy_set_opt_impl(CanopyContext ctx, ...)
{
    va_list ap;
    CanopyResultEnum out;
    va_start(ap, ctx);
    out = st_options_extend_varargs(ctx->options, ap);
    va_end(ap);
    return out;
}

CanopyVarValue CANOPY_FLOAT32(float x)
{
    return st_cloudvar_value_float32(x);
}

CanopyVarValue CANOPY_STRING(const char *sz)
{
    return st_cloudvar_value_string(sz);
}

CanopyVarValue CANOPY_STRUCT(void * dummy, ...)
{
    va_list ap;
    CanopyVarValue out;
    va_start(ap, dummy);
    out = st_cloudvar_value_struct(ap);
    va_end(ap);
    return out;
}

void canopy_var_value_free(CanopyVarValue value)
{
    return st_cloudvar_value_free(value);
}

CanopyResultEnum canopy_var_set(CanopyContext ctx, const char *varname, CanopyVarValue value)
{
    CanopyResultEnum result;
    if (st_cloudvar_value_already_used(value))
    {
        // CanopyVarValue objects are meant to be used once.  If it has been
        // used already, throw an error.
        return CANOPY_ERROR_SINGLE_USE_VALUE_ALREADY_USED;
    }
    result = st_cloudvar_set_local_value(ctx->cloudvars, varname, value);

    // Mark <value> as used, since it is intended to be single-use.
    // This allows, for example:
    //      canopy_set_var(ctx, "foo", CANOPY_FLOAT32(100.0f)) 
    //  to not leak any memory.
    //  Although, it does result in an alloc and free for each call.
    st_cloudvar_value_mark_used(value);
    return result;
}

CanopyVarReader CANOPY_READ_FLOAT32(float *dest)
{
    return st_cloudvar_reader_float32(dest);
}
CanopyVarReader CANOPY_READ_STRING(const char **sz)
{
    return st_cloudvar_reader_string(sz);
}

CanopyVarReader CANOPY_READ_STRUCT(void * dummy, ...)
{
    va_list ap;
    CanopyVarReader out;
    va_start(ap, dummy);
    out = st_cloudvar_reader_struct(ap);
    va_end(ap);
    return out;
}

CanopyResultEnum canopy_var_get(CanopyContext ctx, const char *varname, CanopyVarReader dest)
{
    return st_cloudvar_get_local_value(ctx->cloudvars, varname, dest);
}

CanopyResultEnum canopy_var_on_change(CanopyContext ctx, const char *varname, CanopyOnChangeCallback cb, void *userdata)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_var_config(CanopyContext ctx, const char *varname, ...)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_sync(CanopyContext ctx, CanopyPromise promise)
{
    return st_sync(ctx, ctx->options, ctx->ws, ctx->cloudvars);
}

void canopy_debug_dump_opts(CanopyContext ctx)
{
    // TODO: would like to use OPTION_LIST expansion macro here, but dealing
    // with the different datatypes in the printf is tricky.
    printf("\n\n");
    printf("Context 0x%p settings\n", ctx);
    printf("----------------------\n");
    printf("CLOUD_SERVER: %s\n", 
            ctx->options->has_CANOPY_CLOUD_SERVER ?
                ctx->options->val_CANOPY_CLOUD_SERVER : "<undefined>");
    printf("DEVICE_UUID: %s\n", 
            ctx->options->has_CANOPY_DEVICE_UUID ?
                ctx->options->val_CANOPY_DEVICE_UUID : "<undefined>");

    if (ctx->options->has_CANOPY_VAR_SEND_PROTOCOL)
        printf("VAR_SEND_PROTOCOL: %d\n", 
                ctx->options->val_CANOPY_VAR_SEND_PROTOCOL);
    else
        printf("VAR_SEND_PROTOCOL: <undefined>\n");

    if (ctx->options->has_CANOPY_SYNC_BLOCKING)
        printf("SYNC_BLOCKING: %d\n", 
                ctx->options->val_CANOPY_SYNC_BLOCKING);
    else
        printf("SYNC_BLOCKING: <undefined>\n");

    if (ctx->options->has_CANOPY_SYNC_TIMEOUT_MS)
        printf("SYNC_TIMEOUT_MS: %d\n", 
                ctx->options->val_CANOPY_SYNC_TIMEOUT_MS);
    else
        printf("SYNC_TIMEOUT_MS: <undefined>\n");

    printf("\n\n");
}
