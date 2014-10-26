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

typedef struct CanopyVarStruct_t
{
    RedHash hash; // string -> CanopyVarValue
} CanopyVarStruct_t;

typedef struct CanopyVarValue_t
{
    CanopyDatatypeEnum datatype;
    union
    {
        float val_float32;
        char *val_string;
        CanopyVarStruct_t val_strct;
    } val;
} CanopyVarValue_t;

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
    st_options_free(ctx->options);
    st_websocket_free(ctx->ws);
    st_cloudvar_system_free(ctx->cloudvars);
    free(ctx);
    return NULL;
}

CanopyResultEnum canopy_destroy_context(CanopyContext ctx)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_set_opt_impl(CanopyContext ctx, CanopyOptEnum option, ...)
{
    va_list ap;
    return st_options_extend_varargs(ctx->options, option, ap);
}

CanopyVarValue CANOPY_FLOAT32(float x)
{
    CanopyVarValue out;
    out = malloc(sizeof(CanopyVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT32;
    out->val.val_float32 = x;
    return out;
}

CanopyVarValue CANOPY_STRING(const char *sz)
{
    CanopyVarValue out;
    out = malloc(sizeof(CanopyVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRING;
    out->val.val_string = RedString_strdup(sz);
    if (!out->val.val_string)
    {
        free(out);
        return NULL;
    }
    return out;
}

CanopyVarValue CANOPY_STRUCT(void * dummy, ...)
{
    CanopyVarValue out;
    char *fieldname;
    out = malloc(sizeof(CanopyVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRUCT;
    out->val.val_strct.hash = RedHash_New(0);
    if (!out->val.val_strct.hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    va_list ap;
    va_start(ap, dummy);
    while ((fieldname = va_arg(ap, char *)) != NULL)
    {
        CanopyVarValue val = va_arg(ap, CanopyVarValue);
        RedHash_InsertS(out->val.val_strct.hash, fieldname, val);
    }
    va_end(ap);

    return out;
}

void canopy_var_value_free(CanopyVarValue value)
{
    switch (value->datatype)
    {
        case CANOPY_DATATYPE_FLOAT32:
            free(value);
            break;
        case CANOPY_DATATYPE_STRING:
            free(value->val.val_string);
            free(value);
            break;
        default:
            assert(1);
            break;
    }
}

CanopyResultEnum canopy_var_set(CanopyContext ctx, const char *varname, CanopyVarValue value)
{
    return st_cloudvar_set_local_value(ctx->cloudvars, varname, value);
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
