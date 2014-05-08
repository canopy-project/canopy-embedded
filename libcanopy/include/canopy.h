/*
 * Copyright 2014 - Greg Prisament
 */

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct CanopyContextStruct * CanopyContext;
typedef struct CanopyReportStruct * CanopyReport;
typedef struct CanopyEventDetailsStruct * CanopyEventDetails;

typedef enum
{
    CANOPY_DATATYPE_INVALID,
    CANOPY_DATATYPE_INT8,
    CANOPY_DATATYPE_UINT8,
    CANOPY_DATATYPE_INT32,
    CANOPY_DATATYPE_UINT32,
    CANOPY_DATATYPE_FLOAT32,
    CANOPY_DATATYPE_FLOAT64,
    CANOPY_DATATYPE_STRING,
    CANOPY_DATATYPE_DATETIME
} CanopyDatatypeEnum;

typedef enum
{
    CANOPY_EVENT_INVALID,
    CANOPY_EVENT_REPORT_REQUESTED,
    CANOPY_EVENT_CONTROL_TRIGGER
} CanopyEventEnum;

typedef bool (*CanopyEventCallbackRoutine)(CanopyContext, CanopyEventDetails);

CanopyContext canopy_init();

bool canopy_set_cloud_host(CanopyContext canopy, const char *hostname);
bool canopy_set_cloud_port(CanopyContext canopy, uint16_t port);
bool canopy_set_cloud_username(CanopyContext canopy, const char *username);
bool canopy_set_cloud_password(CanopyContext canopy, const char *password);

bool canopy_connect(CanopyContext canopy);

bool canopy_register_event_callback(CanopyContext canopy, CanopyEventCallbackRoutine fn, void *extra);

bool canopy_load_device_description(CanopyContext canopy, const char *filename, const char *descriptionName);
bool canopy_load_device_description_file(CanopyContext canopy, FILE *file, const char *descriptionName);
bool canopy_load_device_description_string(CanopyContext canopy, const char *szDesc, const char *descriptionName);

CanopyEventEnum canopy_get_event_type(CanopyEventDetails event);
bool canopy_event_control_name_matches(CanopyEventDetails event, const char *name);

CanopyReport canopy_begin_report(CanopyContext canopy);
bool canopy_report_i8(CanopyReport report, const char *parameter, int8_t value);
bool canopy_report_u8(CanopyReport report, const char *parameter, uint8_t value);
bool canopy_report_i32(CanopyReport report, const char *parameter, int32_t value);
bool canopy_report_u32(CanopyReport report, const char *parameter, uint32_t value);
bool canopy_report_float32(CanopyReport report, const char *parameter, float value);
bool canopy_report_float64(CanopyReport report, const char *parameter, double value);
bool canopy_report_string(CanopyReport report, const char *parameter, const char *value);

bool canopy_send_report(CanopyReport report);

bool canopy_event_loop(CanopyContext canopy);

void canopy_quit(CanopyContext canopy);

void canopy_shutdown(CanopyContext canopy);
#endif
