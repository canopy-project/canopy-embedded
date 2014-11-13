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

#ifndef ST_WEBSOCKET_INCLUDED
#define ST_WEBSOCKET_INCLUDED

// WebSocket utility library for Canopy

#include <canopy.h>

// An STWebSocket is an ADT representing a websocket connection.
typedef struct STWebSocket_t * STWebSocket;

typedef void (*STWebsocketRecvCallback)(STWebSocket ws, const char *payload, void *userdata);

// Create a new (disconnected) WebSocket object.
STWebSocket st_websocket_new();

// Free websocket object.
void st_websocket_free(STWebSocket ws);

// Connect to WebSocket server.
CanopyResultEnum st_websocket_connect(
        STWebSocket ws,
        const char *hostname,
        uint16_t port,
        bool useSSL,
        const char *url);

// Is STWebSocket connected?
bool st_websocket_is_connected(STWebSocket ws);

// Is WebSocket ready to send bytes?
bool st_websocket_is_write_ready(STWebSocket ws);

// Service WebSocket.  You must call this periodically.
void st_websocket_service(STWebSocket ws, uint32_t timeout_ms);

// Send payload over the WebSocket.  Fails silently if the WebSocket isn't
// connected to the server.
void st_websocket_write(STWebSocket ws, const char *msg);

// Set the callback that gets triggered when data is received from the server.
void st_websocket_recv_callback(STWebSocket ws, STWebsocketRecvCallback cb, void *userdata);

#endif // ST_WEBSOCKET_INCLUDED
