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

/*
 * _ConfigOpts_t
 *
 *  Represents a set of configuration options.
 */
typedef struct _ConfigOpts_t
{
    bool has_ctx;
    CanopyCtx ctx;

    bool has_cloud_server;
    char *cloud_server;

    bool has_control_protocol;
    CanopyProtocolEnum control_protocol;

    bool has_device_uuid;
    char *device_uuid;

    bool has_notify_msg;
    char *notify_msg;

    bool has_on_change_float32_callback;
    CanopyOnChangeFloat32Callback on_change_float32_callback;

    bool has_notify_protocol;
    CanopyProtocolEnum notify_protocol;

    bool has_notify_type;
    CanopyNotifyTypeEnum notify_type;

    bool has_property_name;
    char *property_name;

    bool has_report_protocol;
    CanopyProtocolEnum report_protocol;

    bool has_value_float32;
    float value_float32;

} _ConfigOpts_t;

typedef struct CanopyCtx_t
{
    _ConfigOpts_t opts;

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

typedef struct _ConfigOpts_t * _ConfigOpts;

static void _config_opts_extend(_ConfigOpts dest, _ConfigOpts base, _ConfigOpts override);

/*
 * _config_opts_defaults
 *
 *  Set a configuration set to system defaults.
 */
static void _config_opts_defaults(_ConfigOpts dest)
{
    dest->has_cloud_server = true;
    dest->cloud_server = RedString_strdup("canopy.link");

    dest->has_control_protocol = true;
    dest->control_protocol = CANOPY_PROTOCOL_WS;

    dest->has_notify_protocol = true;
    dest->notify_protocol = CANOPY_PROTOCOL_HTTP;

    dest->has_notify_type = true;
    dest->notify_type = CANOPY_NOTIFY_MED_PRIORITY;

    dest->has_report_protocol = true;
    dest->report_protocol = CANOPY_PROTOCOL_HTTP;
}

CanopyCtx _get_ctx(_ConfigOpts opts)
{
    assert(opts);
    if (opts->has_ctx)
    {
        return opts->ctx;
    }
    return gCtx;
}

/*
 * _new_config_opts_default
 *
 *  Create a new configuration set, using system default settings.
 */
#if 0
static _ConfigOpts _new_config_opts_default()
{
    _ConfigOpts opts;

    opts = calloc(1, sizeof(struct _ConfigOpts_t));
    if (!opts)
    {
        return NULL;
    }

    _config_opts_defaults(opts);

    return opts;
}
#endif

/*
 * _new_config_opts_empty
 *
 *  Create a new configuration set that does not have any options set.
 */
static _ConfigOpts _new_config_opts_empty()
{
    _ConfigOpts opts;
    opts = calloc(1, sizeof(struct _ConfigOpts_t));
    return opts;
}

/*
 * _config_opts_init_empty
 *
 *  Initialize an empty configuration set.
 */
static void _config_opts_empty(_ConfigOpts opts)
{
    memset(opts, 0, sizeof(_ConfigOpts_t));
}

static void _copy_config_opts(_ConfigOpts dest, _ConfigOpts src)
{
    /* TODO: duplicate strings */
    memcpy(dest, src, sizeof(struct _ConfigOpts_t));
}

/*
 * _new_config_opts_copy
 *
 *  Create a new configuration set by copying another.
 */
static _ConfigOpts _new_config_opts_copy(_ConfigOpts src)
{
    _ConfigOpts opts = _new_config_opts_empty();
    if (!opts)
        return NULL;

    _copy_config_opts(opts, src);
    return opts;
}

/*
 * _config_opts_extend_va
 *
 *  Update configuration options using an argument list.
 */
static CanopyResultEnum _config_opts_extend_va(_ConfigOpts dest, _ConfigOpts src, va_list ap)
{
    _ConfigOpts new_opts = _new_config_opts_empty();
    bool done;

    if (!new_opts)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    while (!done)
    {
        CanopyOptEnum param;
        param = va_arg(ap, CanopyOptEnum);
        /* TODO: free old strings */

        switch (param)
        {
            case CANOPY_CLOUD_SERVER:
            {
                new_opts->has_cloud_server = true;
                new_opts->cloud_server = va_arg(ap, char *);
                break;
            }
            case CANOPY_CONTROL_PROTOCOL:
            {
                new_opts->has_control_protocol = true;
                new_opts->control_protocol = va_arg(ap, CanopyProtocolEnum);
                break;
            }
            case CANOPY_DEVICE_UUID:
            {
                new_opts->has_device_uuid = true;
                new_opts->device_uuid = va_arg(ap, char *);
                break;
            }
            case CANOPY_NOTIFY_MSG:
            {
                new_opts->has_notify_msg = true;
                new_opts->notify_msg = va_arg(ap, char *);
                break;
            }
            case CANOPY_NOTIFY_PROTOCOL:
            {
                new_opts->has_notify_protocol = true;
                new_opts->notify_protocol = va_arg(ap, CanopyProtocolEnum);
                break;
            }
            case CANOPY_NOTIFY_TYPE:
            {
                new_opts->has_notify_type = true;
                new_opts->notify_type = va_arg(ap, CanopyNotifyTypeEnum);
                break;
            }
            case CANOPY_ON_CHANGE_FLOAT32_CALLBACK:
            {
                new_opts->has_on_change_float32_callback = true;
                new_opts->on_change_float32_callback = va_arg(ap, CanopyOnChangeFloat32Callback);
                break;
            }
            case CANOPY_PROPERTY_NAME:
            {
                new_opts->has_property_name = true;
                new_opts->property_name = va_arg(ap, char *);
                break;
            }
            case CANOPY_REPORT_PROTOCOL:
            {
                new_opts->has_report_protocol = true;
                new_opts->report_protocol = va_arg(ap, CanopyProtocolEnum);
                break;
            }
            case CANOPY_VALUE_FLOAT32:
            {
                new_opts->has_value_float32 = true;
                new_opts->value_float32 = (float)va_arg(ap, double);
                break;
            }
            case CANOPY_OPT_LIST_END:
            {
                done = true;
                break;
            }
            default:
            {
                /* Invalid parameter.  Throw error. */
                printf("Invalid parameter %d\n", param);
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }
    va_end(ap);

    /* No errors.  Extend the original */
    _config_opts_extend(dest, src, new_opts);

    free(new_opts);
    return CANOPY_SUCCESS;
}

/*
 * _new_config_opts_va
 *
 *  Create a new configuration set, using supplied argument list.
 */
#if 0
static _ConfigOpts _new_config_opts_va(va_list ap)
{
    _ConfigOpts opts = _new_config_opts_empty();
    if (!opts)
    {
        return NULL;
    }

    _config_opts_extend_va(opts, ap);
    return opts;
}
#endif

/*
 * _config_opts_extend
 *
 *  Merge two configuration sets.  Starts with configuration options specified
 *  in <base>, and overrides with any options specified in <override>.  Stores
 *  result in <dest>, which may be equal to <base> or <override> or another
 *  ConfigOpt object.
 */
static void _config_opts_extend(_ConfigOpts dest, _ConfigOpts base, _ConfigOpts override)
{
    /* TODO: Free memory when strings get overridden */

    dest->cloud_server = override->has_cloud_server ? override->cloud_server : base->cloud_server;
    dest->has_cloud_server = base->has_cloud_server || override->has_cloud_server;

    dest->control_protocol = override->has_control_protocol ? override->control_protocol : base->control_protocol;
    dest->has_control_protocol = base->control_protocol || override->control_protocol;

    dest->device_uuid = override->has_device_uuid ? override->device_uuid : base->device_uuid;
    dest->has_device_uuid = base->device_uuid || override->device_uuid;

    dest->notify_msg = override->has_notify_msg ? override->notify_msg : base->notify_msg;
    dest->has_notify_msg = base->notify_msg || override->notify_msg;

    dest->notify_protocol = override->has_notify_protocol ? override->notify_protocol : base->notify_protocol;
    dest->has_notify_protocol = base->notify_protocol || override->notify_protocol;

    dest->notify_type = override->has_notify_type ? override->notify_type : base->notify_type;
    dest->has_notify_type = base->notify_type || override->notify_type;

    dest->property_name = override->has_property_name ? override->property_name : base->property_name;
    dest->has_property_name = base->property_name || override->property_name;

    dest->report_protocol = override->has_report_protocol ? override->report_protocol : base->report_protocol;
    dest->has_report_protocol = base->report_protocol || override->report_protocol;

    dest->value_float32 = override->has_value_float32 ? override->value_float32 : base->value_float32;
    dest->has_value_float32 = base->has_value_float32 || override->value_float32;
}

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
        _config_opts_defaults(&ctx->opts);
    }
    else
    {
        _copy_config_opts(&ctx->opts, &copyOptsFrom->opts);
    }

    return ctx;
}

CanopyResultEnum canopy_ctx_opt_impl(CanopyCtx ctx, ...)
{
    va_list ap;
    CanopyResultEnum result;
    _init_libcanopy_if_needed();

    va_start(ap, ctx);
    result = _config_opts_extend_va(&ctx->opts, &ctx->opts, ap);

    return result;
}

CanopyCtx canopy_global_ctx()
{
    _init_libcanopy_if_needed();
    return gCtx;
}

CanopyResultEnum canopy_on_change_impl(void *start, ...)
{
    /* TODO: Free memory */
    _ConfigOpts_t opts;
    _ConfigOpts optsCopy;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;
    /* TODO: support other contexts */

    /* Process arguments */
    va_start(ap, start);
    _config_opts_empty(&opts);
    result = _config_opts_extend_va(&opts, &ctx->opts, ap); /* This calls va_end */
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    /* Copy options */
    optsCopy = _new_config_opts_copy(&opts);
    if (!optsCopy)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    /* Register callback by adding to hash table. 
     * TODO: RedHash_Insert should return error code
     */
    if (!opts.has_property_name)
    {
        /* Property name required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    /* TODO: How to handle mutliple callbacks for single property name?
     * Should we override/replace or trigger both callbacks?
     */
    RedHash_InsertS(ctx->control_cb_hash, opts.property_name, optsCopy);

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
    _ConfigOpts_t opts;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;
    /* TODO: support other contexts */

    /* Process arguments */
    va_start(ap, start);
    _config_opts_empty(&opts);
    result = _config_opts_extend_va(&opts, &ctx->opts, ap);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    /* Construct payload  */
    RedJsonObject payload_json = RedJsonObject_New();
    if (!opts.has_property_name)
    {
        /* Property name required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }
    if (opts.has_value_float32)
    {
        RedJsonObject_SetNumber(payload_json, opts.property_name, opts.value_float32);
    }
    else
    {
        /* value required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    /* Send payload to server, if using HTTP */
    if (opts.report_protocol == CANOPY_PROTOCOL_HTTP)
    {
        char *url;
        url = RedString_PrintfToNewChars("http://%s/di/device/%s", 
                opts.cloud_server,
                opts.device_uuid);
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
    _ConfigOpts_t opts;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    /* Process arguments */
    va_start(ap, start);
    _config_opts_empty(&opts);
    result = _config_opts_extend_va(&opts, &ctx->opts, ap);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    /* Construct payload */
    RedJsonObject payload_json = RedJsonObject_New();
    if (!opts.has_notify_msg)
    {
        /* Notification message required */
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    RedJsonObject_SetString(payload_json, "notify-msg", opts.notify_msg);
    RedJsonObject_SetString(payload_json, "notify-type", "email");

    if (opts.report_protocol == CANOPY_PROTOCOL_HTTP)
    {
        char *url;
        url = RedString_PrintfToNewChars("http://%s/di/device/%s/notify", 
                opts.cloud_server,
                opts.device_uuid);
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


static CanopyResultEnum _canopy_service_opts(_ConfigOpts params)
{
    CanopyCtx ctx = _get_ctx(params);

    printf("Servicing\n");

    /* Start server if necessary */
    if (!st_websocket_is_connected(ctx->ws))
    {
        CanopyResultEnum result;
        result = st_websocket_connect(
                ctx->ws,
                params->cloud_server,
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
                params->device_uuid,
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
    _ConfigOpts_t opts;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    va_start(ap, start);
    _config_opts_empty(&opts);
    result = _config_opts_extend_va(&opts, &ctx->opts, ap);
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error processing arguments in canopy_run_event_loop\n");
        return result;
    }

    while (1)
    {
        _canopy_service_opts(&opts);
        sleep(1); /* TODO: No sleeping! */
    }
    /* TODO: Implement */
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

CanopyResultEnum canopy_service_impl(void *start, ...)
{
    _ConfigOpts_t opts;
    va_list ap;
    _init_libcanopy_if_needed();
    CanopyCtx ctx = gCtx;
    CanopyResultEnum result;

    /* Process arguments */
    va_start(ap, start);
    _config_opts_empty(&opts);
    result = _config_opts_extend_va(&opts, &ctx->opts, ap);
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error processing arguments in canopy_service\n");
        return result;
    }

    return _canopy_service_opts(&opts);
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
