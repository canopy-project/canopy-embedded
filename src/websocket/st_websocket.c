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

// WebSocket utility library for Canopy

#include "websocket/st_websocket.h"
#include "red_log.h"
#include "log/st_log.h"
#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct STWebSocket_t
{
    struct libwebsocket_context *ws_ctx;
    struct libwebsocket *ws;
    bool ws_write_ready;
    STWebsocketRecvCallback cb_recv;
    void *cb_recv_userdata;
};

STWebSocket st_websocket_new()
{
    return calloc(1, sizeof(struct STWebSocket_t));
}

void st_websocket_free(STWebSocket ws)
{
    free(ws);
}

static int _ws_callback(
        struct libwebsocket_context *this,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user,
        void *in,
        size_t len)
{
    STWebSocket ws = (STWebSocket)libwebsocket_context_user(this);
    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        {
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
            libwebsocket_callback_on_writable(this, wsi);
#if 0
            CanopyEventDetails_t eventDetails;
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
            libwebsocket_callback_on_writable(this, wsi);

            /* Call event callback */
            eventDetails.ctx = canopy;
            eventDetails.eventType = CANOPY_EVENT_CONNECTION_ESTABLISHED;
            eventDetails.userData = canopy->cbExtra;
            canopy->cb(canopy, &eventDetails);
            canopy->ws_closed = false; // TODO: change to "connected"
#endif
            break;
        }
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
            return -1;
        case LWS_CALLBACK_CLOSED:
        {
#if 0
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLOSED\n");
            canopy->ws_closed = true;
#endif
            return -1;
        }
        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            ws->ws_write_ready = true;
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
            /* TODO: this next line seems dangerous! */
            ((char *)in)[len] = '\0';
            fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
            //_process_ws_payload(canopy, in);
            if (ws->cb_recv)
            {
                ws->cb_recv(ws, in, ws->cb_recv_userdata);
            }
            break;
        /*case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:*/
        default:
            break;
    }
    return 0;
}

bool st_websocket_is_connected(STWebSocket ws)
{
    assert(ws);
    return ws->ws != NULL;
}

bool st_websocket_is_write_ready(STWebSocket ws)
{
    assert(ws);
    return ws->ws_write_ready;
}

CanopyResultEnum st_websocket_connect(
        STWebSocket ws,
        const char *hostname,
        uint16_t port,
        bool useSSL,
        const char *url)
{
    static struct libwebsocket_protocols sCanopyWsProtocols[] = {
        {
            "echo", // TODO: rename 
            _ws_callback,
            1024,
            1024,
            0,
            NULL,
            0
        },
        { NULL, NULL, 0, 0, 0, NULL, 0} // end
    };
    struct lws_context_creation_info info={0};
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.iface = NULL;
    info.protocols = sCanopyWsProtocols;
    info.extensions = NULL;
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;
    info.ssl_ca_filepath = NULL;
    info.ssl_cipher_list = NULL;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
    info.user = ws;
    info.ka_time = 0;
    info.ka_probes = 0;
    info.ka_interval = 0;

    //lws_set_log_level(511, NULL);

    ws->ws_ctx = libwebsocket_create_context(&info);
    if (!ws->ws_ctx)
    {
        fprintf(stderr, "Failed to create libwebsocket context\n");
        return CANOPY_ERROR_CONNECTION_FAILED;
    }

    printf("Connecting to:\n");
    printf("Host: %s\n", hostname);
    printf("Port: %d\n", port);
    printf("UseSSL: %d\n", useSSL);
    ws->ws = libwebsocket_client_connect(
            ws->ws_ctx, 
            hostname, 
            port,
            useSSL,
            url, // "/echo"
            hostname ,
            "localhost", // origin
            "echo", // TODO: rename
            -1 // latest ietf version
        );
    if (!ws->ws)
    {
        fprintf(stderr, "Failed to create libwebsocket connection\n");
        return CANOPY_ERROR_CONNECTION_FAILED;
    }

    libwebsocket_callback_on_writable(ws->ws_ctx, ws->ws);
    return CANOPY_SUCCESS;
}

void st_websocket_service(STWebSocket ws, uint32_t timeout_ms)
{
    libwebsocket_service(ws->ws_ctx, timeout_ms);
}

void st_websocket_write(STWebSocket ws, const char *msg)
{
    char *buf;
    if (!ws->ws_write_ready)
    {
        RedLog_DebugLog("canopy", "WS not ready for write!  Skipping.");
        return;
    }

    // libwebsockets requires all this crazy padding.
    size_t len = strlen(msg);
    buf = calloc(1, LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
    strcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], msg);

    // Log payload
    st_log_debug("Websocket Send: %d '%s'\n", (int)len, (char *)msg);

    // Send msg.
    libwebsocket_write(ws->ws, (unsigned char *)&buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
    ws->ws_write_ready = false;

    // Register callback so that we're informed when it is safe to write again.
    libwebsocket_callback_on_writable(ws->ws_ctx, ws->ws);

    // Cleanup.
    free(buf);
}

void st_websocket_recv_callback(STWebSocket ws, STWebsocketRecvCallback cb, void *userdata)
{
    ws->cb_recv = cb;
    ws->cb_recv_userdata = userdata;
}
