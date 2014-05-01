/*
 * Copyright 2014 - Greg Prisament
 */

#ifndef HAIVE_INCLUDED
#define HAIVE_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct HaiveContextStruct * HaiveContext;
typedef struct HaiveReportStruct * HaiveReport;

typedef enum
{
    HAIVE_DATATYPE_INVALID,
    HAIVE_DATATYPE_INT8,
    HAIVE_DATATYPE_UINT8,
    HAIVE_DATATYPE_INT32,
    HAIVE_DATATYPE_UINT32,
    HAIVE_DATATYPE_FLOAT32,
    HAIVE_DATATYPE_FLOAT64,
    HAIVE_DATATYPE_STRING,
    HAIVE_DATATYPE_DATETIME
} HaiveDatatypeEnum;

typedef enum
{
    HAIVE_EVENT_REPORT_REQUESTED
} HaiveEventEnum;

typedef bool (*HaiveEventCallbackRoutine)(HaiveContext, HaiveEventEnum, void *);

HaiveContext haive_init();

bool haive_connect(const char *websocket_url);

bool haive_register_event_callback(HaiveContext haive, HaiveEventCallbackRoutine fn, void *extra);

bool haive_load_device_description(HaiveContext haive, const char *filename);
bool haive_load_device_description_file(HaiveContext haive, FILE *file);
bool haive_load_device_description_string(HaiveContext haive, const char *szDesc);

HaiveReport haive_begin_report(HaiveContext haive);
bool haive_report_i8(HaiveReport report, const char *parameter, int8_t value);
bool haive_report_u8(HaiveReport report, const char *parameter, uint8_t value);
bool haive_report_i32(HaiveReport report, const char *parameter, int32_t value);
bool haive_report_u32(HaiveReport report, const char *parameter, uint32_t value);
bool haive_report_float32(HaiveReport report, const char *parameter, float value);
bool haive_report_float64(HaiveReport report, const char *parameter, double value);
bool haive_report_string(HaiveReport report, const char *parameter, const char *value);

bool haive_send_report(HaiveReport report);

bool haive_event_loop(HaiveContext haive);

void haive_quit(HaiveContext haive);

void haive_shutdown(HaiveContext haive);
#endif
