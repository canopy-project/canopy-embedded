/*
 * Copyright 2014 - Greg Prisament
 */
#ifndef HAIVE_INTERNAL_INCLUDED
#define HAIVE_INTERNAL_INCLUDED

#include "haive.h"
#include "red_hash.h"

typedef struct HaiveContextStruct
{
    bool initialized;
    RedHash properties;
    HaiveEventCallbackRoutine cb;
    void * cbExtra;
    bool quitRequested;
} HaiveContextStruct;


typedef struct _HaiveProperty
{
    char *name;
    HaiveDatatypeEnum datatype;
    bool hasMinRange;
    bool hasMaxRange;
    int64_t rangeMin;
    uint64_t rangeMax;
} _HaiveProperty;

typedef struct _HaivePropertyValue
{
    HaiveDatatypeEnum datatype;
    union
    {
        int8_t val_int8;
        int32_t val_int32;
        float val_float32;
    } val;
} _HaivePropertyValue;

typedef struct HaiveReportStruct
{
    HaiveContext ctx;
    RedHash values; /* (char *)propName -> (_HaivePropertyValue *)propVal */
    bool finished;
} HaiveReportStruct;

typedef struct _HaiveInterface
{
    RedHash properties; /* (char *)propName -> (_HaiveProperty *)prop */
} _HaiveInterface;

typedef struct _HaiveDevice
{
    RedHash interfaces; /* (char *)itfName -> (_HaiveInterface *)itf */
} _HaiveDevice;


#endif
