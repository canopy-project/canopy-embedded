/*
 * Copyright 2014 SimpleThings, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * canopy.c
 *
 * This file implements the API entrypoints declared in canopy.h
 */

#include <canopy.h>
#include "http/st_http.h"
#include "options/st_options.h"
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
#include <libwebsockets.h>

/*
 *
 * COMPILE WITH:
 * gcc -Isrc -Iinclude -I3rdparty/libred/include -I3rdparty/libred/under_construction -c src/canopy_simple.c -Wall -Werror
 */

static CanopyCtx gCtx=NULL;

typedef struct
{
} CanopyPromise_t;

typedef struct
{
} CanopyResult_t;

typedef struct CanopyCtx_t
{
    STOptions options;

    STWebSocket ws;

    /* 
     * Hash table storing registered callbacks.
     *      (char *)propertyName ==> (_ConfigOpts_t *)
     */
    RedHash control_cb_hash;

    /* Has SDDL been modified since last call to canopy_service? 
     */
    bool sddl_dirty;
} CanopyCtx_t;

void _init_libcanopy_if_needed()
{
    /* Create global ctx */
    if (!gCtx)
    {
        gCtx = canopy_create_ctx(NULL);
    }
}

CanopyCtx canopy_create_ctx(CanopyCtx copyOptsFrom)
{
    CanopyCtx ctx;

    ctx = calloc(1, sizeof(struct CanopyCtx_t));
    if (!ctx)
    {
        return NULL;
    }

    ctx->control_cb_hash = RedHash_New(0);

    if (copyOptsFrom == NULL)
    {
        ctx->options = st_options_new_default();
    }
    else
    {
        ctx->options = st_options_dup(copyOptsFrom->options);
    }
    if (!ctx->options)
    {
        RedLog_Error("OOM in canopy_create_ctx");
        goto fail;
    }

    return ctx;
fail:
    free(ctx->options);
    free(ctx);
    return NULL;
}

CanopyResultEnum canopy_ctx_opt_impl(CanopyCtx ctx, ...)
{
    va_list ap;
    CanopyResultEnum result;
    _init_libcanopy_if_needed();

    result = st_options_extend_varargs(ctx->options, ctx, ap);

    return result;
}

CanopyCtx canopy_global_ctx()
{
    _init_libcanopy_if_needed();
    return gCtx;
}

CanopyResultEnum canopy_on_change_impl(void *start, ...)
{
    // TODO: Free memory 
    STOptions options;
    STOptions optionsCopy;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;
    /* TODO: support other contexts */

    /* Process arguments */
    result = st_options_new_extend_varargs(&options, ctx->options, start, ap);
    if (result != CANOPY_SUCCESS)
    {
        // TODO: Error details
        return result;
    }

    /* Copy options */
    optionsCopy = st_options_dup(options);
    if (!optionsCopy)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    /* Register callback by adding to hash table. 
     */
    if (!st_option_is_set(options, CANOPY_PROPERTY_NAME))
    {
        /* Property name required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    /* TODO: How to handle mutliple callbacks for single property name?
     * Should we override/replace or trigger both callbacks?
     * TODO: RedHash_Insert should return error code
     */
    RedHash_InsertS(ctx->control_cb_hash, options->val_CANOPY_PROPERTY_NAME, optionsCopy);

    /* TODO: Promises */

    /* Mark SDDL as dirty, so that details about this control get sent to the
     * Cloud Service next time canopy_service() is called
     */
    ctx->sddl_dirty = true;

    return CANOPY_SUCCESS;
}

CanopyResultEnum canopy_post_sample_impl(void * start, ...)
{
    /* TODO: Free memory */
    STOptions options;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;
    /* TODO: support other contexts */

    /* Process arguments */
    result = st_options_new_extend_varargs(&options, ctx->options, start, ap);
    if (result != CANOPY_SUCCESS)
    {
        // TODO: Error details
        return result;
    }

    /* Construct payload  */
    RedJsonObject payload_json = RedJsonObject_New();
    if (!st_option_is_set(options, CANOPY_PROPERTY_NAME))
    {
        /* Property name required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }
    if (st_option_is_set(options, CANOPY_VALUE_FLOAT32))
    {
        RedJsonObject_SetNumber(payload_json, options->val_CANOPY_PROPERTY_NAME, options->val_CANOPY_VALUE_FLOAT32);
    }
    else
    {
        /* value required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    /* Send payload to server, if using HTTP */
    if (options->val_CANOPY_REPORT_PROTOCOL == CANOPY_PROTOCOL_HTTP)
    {
        char *url;
        url = RedString_PrintfToNewChars("http://%s/di/device/%s", 
                options->val_CANOPY_CLOUD_SERVER,
                options->val_CANOPY_DEVICE_UUID);
        if (!url)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }
        /* Use curl */
        CanopyPromise promise;
        st_http_post(ctx, url, RedJsonObject_ToJsonString(payload_json) /* TODO: mem leak */, &promise);
        free(url);
    }
    else
    {
        return CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED;
    }

    return CANOPY_SUCCESS;
}

CanopyResultEnum canopy_notify_impl(void *start, ...)
{
    STOptions options;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    /* Process arguments */
    result = st_options_new_extend_varargs(&options, ctx->options, start, ap);
    if (result != CANOPY_SUCCESS)
    {
        // TODO: Error details
        return result;
    }

    /* Construct payload */
    RedJsonObject payload_json = RedJsonObject_New();
    if (!options->has_CANOPY_NOTIFY_MSG)
    {
        /* Notification message required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    RedJsonObject_SetString(payload_json, "notify-msg", options->val_CANOPY_NOTIFY_MSG);
    RedJsonObject_SetString(payload_json, "notify-type", "email");

    if (options->has_CANOPY_REPORT_PROTOCOL == CANOPY_PROTOCOL_HTTP)
    {
        char *url;
        url = RedString_PrintfToNewChars("http://%s/di/device/%s/notify", 
                options->val_CANOPY_CLOUD_SERVER,
                options->val_CANOPY_DEVICE_UUID);
        if (!url)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }
        /* Use curl */
        CanopyPromise promise;
        st_http_post(ctx, url, RedJsonObject_ToJsonString(payload_json) /* TODO: mem leak */, &promise);
        free(url);
    }
    else
    {
        return CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED;
    }

    return CANOPY_SUCCESS;
}

CanopyResultEnum canopy_promise_on_done(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{

    return CANOPY_ERROR_NOT_IMPLEMENTED;
    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_on_failure(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_promise_on_success(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_promise_result(CanopyPromise promise)
{
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_promise_wait(CanopyPromise promise, ...)
{
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}


static CanopyResultEnum _canopy_service_opts(STOptions options)
{
    CanopyCtx ctx = gCtx; // TODO: Use real ctx

    printf("Servicing\n");

    /* Start server if necessary */
    if (!st_websocket_is_connected(ctx->ws))
    {
        CanopyResultEnum result;
        result = st_websocket_connect(
                ctx->ws,
                options->val_CANOPY_CLOUD_SERVER,
                80, // TODO: don't hardcode
                false, // TODO: don't hardcode
                "/echo"); // TODO: rename
        if (result != CANOPY_SUCCESS)
            return result;

        /* Service websocket for first time */
        st_websocket_service(ctx->ws, 100);
    }

    /* If SDDL dirty flag is set, write updated SDDL to websocket */
    if (ctx->sddl_dirty && st_websocket_is_write_ready(ctx->ws))
    {
        printf("SDDL IS DIRTY SENDING PAYLOAD\n");
        /* Construct payload */
        char * payload;
        /* TODO: make this work with multiple controls */
        char * controlName = "onoff";
        payload = RedString_PrintfToNewChars("{\"device_id\" : \"%s\", \"__sddl_update\" : {\"control %s\" : { \"datatype\" : \"float32\" } } }", 
                options->val_CANOPY_DEVICE_UUID,
                controlName);
        st_websocket_write(ctx->ws, payload);

        ctx->sddl_dirty = false;
    }

    /* Service websockets */
    st_websocket_service(ctx->ws, 1000);

    return CANOPY_SUCCESS;
}


CanopyResultEnum canopy_run_event_loop_impl(void *start, ...)
{
    STOptions options;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    result = st_options_new_extend_varargs(&options, ctx->options, start, ap);
    if (result != CANOPY_SUCCESS)
    {
        // TODO: Error details
        fprintf(stderr, "Error processing arguments in canopy_run_event_loop\n");
        return result;
    }

    while (1)
    {
        _canopy_service_opts(options);
        sleep(1); /* TODO: No sleeping! */
    }
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_service_impl(void *start, ...)
{
    STOptions options;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    /* Process arguments */
    result = st_options_new_extend_varargs(&options, ctx->options, start, ap);
    if (result != CANOPY_SUCCESS)
    {
        // TODO: Error details
        fprintf(stderr, "Error processing arguments in canopy_service\n");
        return result;
    }

    return _canopy_service_opts(options);
}

/*
 * TODO:
 *
 * 1) Send actual report.
 *      - Add REPORT_POST_IMMEDIATELY
 *      - Support multiple value types
 *      - Use actual hostname & device UUID in request
 *
 *
 */
