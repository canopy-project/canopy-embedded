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

#include "sync/st_sync.h"
#include "cloudvar/st_cloudvar.h"
#include "http/st_http.h"
#include "log/st_log.h"
#include "options/st_options.h"
#include "websocket/st_websocket.h"
#include "red_json.h"
#include "red_string.h"
#include <stdlib.h>
#include <stdio.h>

static CanopyResultEnum _send_payload(
        CanopyContext ctx, 
        STOptions options, 
        STWebSocket ws,
        const char *payload)
{
    // Send payload to cloud
    if (!st_option_is_set(options, CANOPY_VAR_SEND_PROTOCOL))
    {
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    if (options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_HTTP)
    {
        // Push: HTTP implementation
        char *url;
        url = RedString_PrintfToNewChars("http://%s/di/device/%s", 
                options->val_CANOPY_CLOUD_SERVER,
                options->val_CANOPY_DEVICE_UUID);
        if (!url)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }

        CanopyPromise promise;
        st_http_post(ctx, url, payload, &promise);
        free(url);
    }
    else if (options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_WS)
    {
        // Push: WS implementation
        if (!(st_websocket_is_connected(ws) && st_websocket_is_write_ready(ws)))
        {
            return CANOPY_ERROR_CONNECTION_FAILED;
        }
        // TODO: need a different payload for WS as for HTTP?
        st_websocket_write(ws, payload);
    }
    else if (options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_NOOP)
    {
        // Push: NOOP implementation
        // Just log the payload
        printf("NOOP PUSH:\n%s\n", payload);
    }
    else {
        return CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED;
    }
    return CANOPY_SUCCESS;
}

static CanopyResultEnum _process_payload(STCloudVarSystem sys, const char *payload)
{
    st_log_debug("Processing payload %s", payload); // TODO: Only log if payload logging enabled

    RedJsonObject json = RedJson_Parse(payload);
    if (!json)
    {
        return CANOPY_ERROR_PARSING_PAYLOAD;
    }

    if (RedJsonObject_HasKey(json, "vars"))
    {
        unsigned numVars, i;
        RedJsonObject varsJson;
        char ** varnames;
        CanopyResultEnum result;

        if (!RedJsonObject_IsValueObject(json, "vars"))
        {
            st_log_error("Inbound payload error: Expected \"vars\" to be JSON object\n");
            return CANOPY_ERROR_PROCESSING_PAYLOAD;
        }

        varsJson = RedJsonObject_GetObject(json, "vars");

        numVars = RedJsonObject_NumItems(varsJson);
        varnames = RedJsonObject_NewKeysArray(varsJson);
        if (!varnames)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }
        for (i = 0 ; i < numVars; i++)
        {
            result = st_cloudvar_set_local_value_from_json(sys, varnames[i], RedJsonObject_Get(varsJson, varnames[i]));
            if (result != CANOPY_SUCCESS)
            {
                return result;
            }
        }
        RedJsonObject_FreeKeysArray(varnames);
    }

    return CANOPY_SUCCESS;
}

static void _handle_ws_recv(STWebSocket ws, const char *payload, void *userdata)
{
    _process_payload((STCloudVarSystem)userdata, payload);
}

static char * _gen_outbound_payload(STCloudVarSystem cloudvars)
{
    uint32_t i, num_dirty;
    // construct payload:
    RedJsonObject json = RedJsonObject_New();
    RedJsonObject json_vars = RedJsonObject_New();
    num_dirty = st_cloudvar_system_num_dirty(cloudvars);

    if (num_dirty > 0)
    {
        RedJsonObject_SetObject(json, "vars", json_vars);

        // For each dirty cloud variable, add to the payload "vars" object:
        // TODO: race condition?
        for (i = 0; i < num_dirty; i++)
        {
            STCloudVar var = st_cloudvar_system_dirty_var(cloudvars, i);
            // TODO:
            //   - timestamp for better synchronization?
            //   - post var configuration?
            RedJsonObject_SetNumber(
                    json_vars, 
                    st_cloudvar_name(var), 
                    st_cloudvar_local_value_float32(var));
        }
    }

    return RedJsonObject_ToJsonString(json);
}

CanopyResultEnum st_sync(CanopyContext ctx, STOptions options, STWebSocket ws, STCloudVarSystem cloudvars)
{
    CanopyResultEnum result;

    if (!st_option_is_set(options, CANOPY_CLOUD_SERVER))
    {
        return CANOPY_ERROR_MISSING_REQUIRED_OPTION;
    }

    if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_NOOP)
    {
        // Noop Pull: 
    }
    else if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WS)
    {
        // WS Pull:
        // Initiate websocket connection if necessary:
        if (!st_websocket_is_connected(ws))
        {
            CanopyResultEnum result;
            result = st_websocket_connect(
                    ws,
                    options->val_CANOPY_CLOUD_SERVER,
                    80, // TODO: don't hardcode
                    false, // TODO: don't hardcode
                    "/echo"); // TODO: rename
            if (result != CANOPY_SUCCESS)
                return result;

            // Service websocket for first time
            st_websocket_recv_callback(ws, _handle_ws_recv, cloudvars);
            st_websocket_service(ws, 100);
        }
    }

    // Check if local copy of any Cloud Variables have changed since last sync.
    if (st_cloudvar_system_is_dirty(cloudvars))
    {
        char *payload;
        payload = _gen_outbound_payload(cloudvars);

        result = _send_payload(ctx, options, ws, payload);
        free(payload);
        if (result != CANOPY_SUCCESS)
            return result;

        st_cloudvar_system_clear_dirty(cloudvars);
    }

    if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WS)
    {
        // Service websockets
        // TODO: don't hardcode timeout
        st_websocket_service(ws, 1000);
    }

    return CANOPY_SUCCESS;
}
