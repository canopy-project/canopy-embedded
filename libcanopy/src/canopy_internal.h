/*
 * Copyright 2014 Gregory Prisament
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
#ifndef CANOPY_INTERNAL_INCLUDED
#define CANOPY_INTERNAL_INCLUDED

#include "canopy.h"
#include "sddl.h"
#include "red_hash.h"

typedef struct CanopyContext_t
{
    bool initialized;
    char *uuid;
    RedHash properties;
    SDDLClass sddl;
    CanopyEventCallbackRoutine cb;
    void * cbExtra;
    bool quitRequested;
    char *cloudHost;
    uint16_t cloudHttpPort;
    uint16_t cloudHttpsPort;
    char *cloudWebProtocol;
    bool autoReconnect;
    struct libwebsocket *ws;
    struct libwebsocket_context *ws_ctx;
    bool ws_closed;
    bool ws_write_ready;
} CanopyContext_t;

typedef struct _CanopyPropertyValue
{
    SDDLDatatypeEnum datatype;
    union
    {
        bool val_bool;
        char * val_string;
        int8_t val_int8;
        uint8_t val_uint8;
        int16_t val_int16;
        uint16_t val_uint16;
        int32_t val_int32;
        uint32_t val_uint32;
        float val_float32;
        double val_float64;
        struct tm val_datetime;
    } val;
} _CanopyPropertyValue;

typedef struct CanopyEventDetails_t
{
    CanopyContext ctx;
    CanopyEventEnum eventType;
    void *userData;
    char *eventControlName;
    _CanopyPropertyValue value;
} CanopyEventDetails_t;

typedef struct CanopyReport_t
{
    CanopyContext ctx;
    RedHash values; /* (char *)propName -> (_CanopyPropertyValue *)propVal */
    bool finished;
} CanopyReport_t;

void _canopy_ws_write(CanopyContext canopy, const char *msg);

bool _canopy_load_system_config(CanopyContext canopy);

bool canopy_ws_use_ssl(CanopyContext canopy);
#endif
