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

static CanopyCtx gCtx;

typedef struct
{
    /* 
     * Configuration options.
     * These are configured using canopy_ctx_opt() or canopy_global_opt().
     */
    char *cloud_host;
    CanopyProtocolEnum control_protocol;
    char *device_uuid;
    CanopyProtocolEnum notify_protocol;
    CanopyNotifyTypeEnum notify_type;
    char *property_name;
    CanopyProtocolEnum report_protocol;
} CanopyCtx_t;

typedef struct
{
} CanopyPromise_t;

typedef struct
{
} CanopyResult_t;

void _init_libcanopy_if_needed()
{
    /* Create global ctx */
    gCtx = canopy_create_ctx(NULL);
}

CanopyCtx canopy_create_ctx(CanopyCtx copyOptsFrom)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_ctx_opt_impl(CanopyCtx ctx, ...)
{
    va_list ap;
    bool done = false;
    _init_libcanopy_if_needed();

    char *new_cloud_host;
    CanopyProtocolEnum new_control_protocol;
    char *new_device_uuid;
    CanopyProtocolEnum new_notify_protocol;
    CanopyNotifyTypeEnum new_notify_type;
    char *new_property_name;
    CanopyProtocolEnum new_report_protocol;
    bool cloud_server_changed = false;
    bool control_protocol_changed = false;
    bool device_uuid_changed = false;
    bool notify_protocol_changed = false;
    bool notify_type_changed = false;
    bool property_name_changed = false;
    bool report_protocol_changed = false;

    va_start(ap, ctx);
    while (!done)
    {
        CanopyOptEnum param;
        param = va_arg(ap, CanopyOptEnum);

        switch (param)
        {
            case CANOPY_CLOUD_SERVER:
            {
                new_cloud_server = va_arg(ap, char *);
                cloud_server_changed = true;
                break;
            }
            case CANOPY_CONTROL_PROTOCOL:
            {
                new_control_protocol = va_arg(ap, CanopyProtocolEnum);
                control_protocol_changed = true;
                break;
            }
            case CANOPY_DEVICE_UUID:
            {
                new_device_uuid = va_arg(ap, char *);
                device_uuid_changed = true;
                break;
            }
            case CANOPY_NOTIFY_PROTOCOL:
            {
                new_notify_protocol = va_arg(ap, CanopyProtocolEnum);
                notify_protocol_changed = true;
                break;
            }
            case CANOPY_NOTIFY_TYPE:
            {
                new_notify_type = va_arg(ap, CanopyNotifyTypeEnum);
                notify_type_changed = true;
                break;
            }
            case CANOPY_PROPERTY_NAME:
            {
                new_property_name = va_arg(ap, char *);
                property_name_changed = true;
                break;
            }
            case CANOPY_REPORT_PROTOCOL:
            {
                new_report_protocol = va_arg(ap, CanopyProtocolEnum);
                report_protocol_changed = true;
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
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }
    va_end(ap);

    /* Everything's good (no errors).  Update the context. */
    if (cloud_server_changed)
        ctx->cloud_host = RedString_strdup(new_cloud_server);
    if (control_protocol_changed)
        ctx->control_protocol = new_control_protocol;
    if (device_uuid_changed)
        ctx->device_uuid = RedString_strdup(new_device_uuid);
    if (notify_protocol_changed)
        ctx->notify_protocol = new_notify_protocol;
    if (property_name_changed)
        ctx->property_name = RedString_strdup(property_name);
    if (report_protocol_changed)
        ctx->report_protocol = new_report_protocol;

    return CANOPY_SUCCESS;
}

CanopyCtx canopy_global_ctx()
{
    _init_libcanopy_if_needed();
    return gCtx;
}

CanopyResultEnum canopy_post_sample_impl(void * start, ...)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_post_sample_impl(void * start, ...)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_on_done(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{

    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_on_failure(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_on_success(
        CanopyPromise promise, 
        CanopyResultCallback cb)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_result(CanopyPromise promise)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_promise_wait(CanopyPromise promise, ...)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_run_event_loop_impl(void *start, ...)
{
    /* TODO: Implement */
}

CanopyResultEnum canopy_service_impl(void *start, ...)
{
    /* TODO: Implement */
}
