/*
 * Copyright 2014 - Greg Prisament
 */
#ifndef CANOPY_INTERNAL_INCLUDED
#define CANOPY_INTERNAL_INCLUDED

#include "canopy.h"
#include "red_hash.h"
#include "libwebsockets.h"

typedef struct CanopyContextStruct
{
    bool initialized;
    char *uuid;
    RedHash properties;
    CanopyEventCallbackRoutine cb;
    void * cbExtra;
    bool quitRequested;
    char *cloudHost;
    uint16_t cloudPort;
    char *cloudUsername;
    char *cloudPassword;
    bool autoReconnect;
    struct libwebsocket *ws;
    struct libwebsocket_context *ws_ctx;
    bool ws_write_ready;
} CanopyContextStruct;

typedef struct _CanopyPropertyValue
{
    CanopyDatatypeEnum datatype;
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

typedef struct CanopyEventDetailsStruct
{
    CanopyEventEnum eventType;
    void *userData;
    char *eventControlName;
    _CanopyPropertyValue value;
} CanopyEventDetailsStruct;

typedef struct _CanopyProperty
{
    char *name;
    CanopyDatatypeEnum datatype;
    bool hasMinRange;
    bool hasMaxRange;
    int64_t rangeMin;
    uint64_t rangeMax;
} _CanopyProperty;

typedef struct CanopyReportStruct
{
    CanopyContext ctx;
    RedHash values; /* (char *)propName -> (_CanopyPropertyValue *)propVal */
    bool finished;
} CanopyReportStruct;

typedef struct _CanopyInterface
{
    RedHash properties; /* (char *)propName -> (_CanopyProperty *)prop */
} _CanopyInterface;

typedef struct _CanopyDevice
{
    RedHash interfaces; /* (char *)itfName -> (_CanopyInterface *)itf */
} _CanopyDevice;

void _canopy_ws_write(CanopyContext canopy, const char *msg);

#endif
