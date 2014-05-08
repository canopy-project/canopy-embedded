/*
 * Copyright 2014 - Greg Prisament
 */
#ifndef CANOPY_INTERNAL_INCLUDED
#define CANOPY_INTERNAL_INCLUDED

#include "canopy.h"
#include "red_hash.h"

typedef struct CanopyContextStruct
{
    bool initialized;
    RedHash properties;
    CanopyEventCallbackRoutine cb;
    void * cbExtra;
    bool quitRequested;
    char *cloudHost;
    uint16_t cloudPort;
    char *cloudUsername;
    char *cloudPassword;
} CanopyContextStruct;

typedef struct CanopyEventDetailsStruct
{
    CanopyEventEnum eventType;
    void *userData;
    char *eventControlName;
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

typedef struct _CanopyPropertyValue
{
    CanopyDatatypeEnum datatype;
    union
    {
        int8_t val_int8;
        int32_t val_int32;
        float val_float32;
    } val;
} _CanopyPropertyValue;

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


#endif
