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
#include <sddl.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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

    if (options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_HTTP ||
        options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_HTTPS)
    {
        return CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED;
    }
    else if (options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_WS ||
            options->val_CANOPY_VAR_SEND_PROTOCOL == CANOPY_PROTOCOL_WSS)
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
    fprintf(stderr, "_process_payload'%s'\n", payload);

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
            STCloudVar cloudvar;
            RedJsonValue json;
            cloudvar = st_cloudvar_system_lookup_var(sys, varnames[i]);
            if (!cloudvar)
            {
                // TODO: is this an error?
                continue;
            }
            json = RedJsonObject_Get(varsJson, varnames[i]);
            result = st_cloudvar_update_from_json(cloudvar, json);
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
    fprintf(stderr, "_handle_ws_recv '%s'\n", payload);
    _process_payload((STCloudVarSystem)userdata, payload);
}

static char * _gen_handshake_payload(const char *uuid, const char *secret)
{
    return RedString_PrintfToNewChars("{\"device_id\" : \"%s\", \"secret_key\" : \"%s\"  }", uuid, secret);
}

static char * _gen_outbound_payload(STCloudVarSystem cloudvars)
{
    uint32_t i, num_dirty;
    // construct payload:
    RedJsonObject json = RedJsonObject_New();
    RedJsonObject json_vars = RedJsonObject_New();
    RedJsonObject json_sddl = RedJsonObject_New();
    num_dirty = st_cloudvar_system_num_dirty(cloudvars);

    if (num_dirty > 0)
    {
        RedJsonObject_SetObject(json, "vars", json_vars);
        RedJsonObject_SetObject(json, "sddl", json_sddl);

        // For each dirty cloud variable, add to the payload "vars" object:
        // TODO: race condition?
        for (i = 0; i < num_dirty; i++)
        {
            STCloudVar var = st_cloudvar_system_dirty_var(cloudvars, i);

            // If the variable's configuration hasn't been sent yet, or is
            // dirty, send it
            if (st_cloudvar_is_sddl_dirty(var))
            {
                //
                // "sddl" : {
                //     "uint16 var_u16" : {}
                // }
                const char * decl = st_cloudvar_decl_string(var);
                RedJsonObject properties = st_cloudvar_definition_json(var);
                RedJsonObject_SetObject(json_sddl, decl, properties);
                // TODO: set other configuration settings

                // TODO: Only actually mark as configured after the server responds.
                st_cloudvar_clear_sddl_dirty_flag(var);
            }

            // TODO:
            //   - timestamp for better synchronization?
            if (st_cloudvar_has_value(var))
            {
                RedJsonValue val;
                CanopyResultEnum result;
                result = st_cloudvar_value_to_json(&val, var);
                if (result != CANOPY_SUCCESS)
                {
                    return NULL;
                }
                RedJsonObject_Set(
                        json_vars, 
                        st_cloudvar_name(var), 
                        val);
            }
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
    else if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WS ||
            options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WSS)
    {
        // WS Pull:
        // Initiate websocket connection if necessary:
        if (!st_websocket_is_connected(ws))
        {
            CanopyResultEnum result;
            int16_t port;
            bool useSSL = false;
            if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WSS)
            {
                port = options->val_CANOPY_HTTPS_PORT;
                useSSL = true;
            }
            else 
            {
                port = options->val_CANOPY_HTTP_PORT;
            }
            result = st_websocket_connect(
                    ws,
                    options->val_CANOPY_CLOUD_SERVER,
                    port,
                    useSSL,
                    options->val_CANOPY_SKIP_SSL_CERT_CHECK,
                    "/echo"); // TODO: rename
            if (result != CANOPY_SUCCESS)
                return result;

            // Service websocket for first time
            st_websocket_recv_callback(ws, _handle_ws_recv, cloudvars);
            st_websocket_service(ws, 1000);
            st_websocket_service(ws, 1000);

            // send handhsake
            char *handshakePayload;
            handshakePayload = _gen_handshake_payload(
                    options->val_CANOPY_DEVICE_UUID,
                    options->val_CANOPY_DEVICE_SECRET_KEY);

            while (!(st_websocket_is_connected(ws) && st_websocket_is_write_ready(ws)))
            {
                // TODO: give up eventually!
                st_websocket_service(ws, 1000);
            }
            // TODO: need a different payload for WS as for HTTP?
            st_websocket_write(ws, handshakePayload);
            free(handshakePayload);

            st_websocket_service(ws, 1000);
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

    if (options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WS ||
        options->val_CANOPY_VAR_RECV_PROTOCOL == CANOPY_PROTOCOL_WSS)
    {
        // Service websockets
        // TODO: don't hardcode timeout
        st_websocket_service(ws, 1000);
    }

    return CANOPY_SUCCESS;
}
