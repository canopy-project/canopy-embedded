/*
 * Copyright 2014 - Greg Prisament
 */
#ifndef CANOPY_INTERNAL_INCLUDED
#define CANOPY_INTERNAL_INCLUDED

#include "canopy.h"
#include "sddl.h"
#include "red_hash.h"
#include "libwebsockets.h"

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
    bool ws_write_ready;
} CanopyContext_t;

typedef struct _CanopyPropertyValue
{
    SDDLDatatypeEnum datatype;
    union
    {
        bool val_bool;
        int8_t val_int8;
        uint8_t val_uint8;
        int16_t val_int16;
        uint16_t val_uint16;
        int32_t val_int32;
        uint32_t val_uint32;
        float val_float32;
        double val_float64;
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

#endif
