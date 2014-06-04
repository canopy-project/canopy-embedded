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
    CANOPY_EVENT_INVALID,
    CANOPY_EVENT_CONNECTION_ESTABLISHED,
    CANOPY_EVENT_CONNECTION_LOST,
    CANOPY_EVENT_REPORT_REQUESTED,
    CANOPY_EVENT_CONTROL_CHANGE,
    CANOPY_EVENT_CONTROL_TRIGGER
} CanopyEventEnum;

typedef bool (*CanopyEventCallbackRoutine)(CanopyContext, CanopyEventDetails);

CanopyContext canopy_init();

bool canopy_set_cloud_host(CanopyContext canopy, const char *hostname);
bool canopy_set_cloud_port(CanopyContext canopy, uint16_t port);
bool canopy_set_cloud_username(CanopyContext canopy, const char *username);
bool canopy_set_cloud_password(CanopyContext canopy, const char *password);
bool canopy_set_auto_reconnect(CanopyContext canopy, bool enabled);
bool canopy_set_device_id(CanopyContext canopy, const char *uuid);
bool canopy_set_device_id_filename(CanopyContext canopy, const char *filename);

bool canopy_connect(CanopyContext canopy);

bool canopy_register_event_callback(CanopyContext canopy, CanopyEventCallbackRoutine fn, void *extra);

bool canopy_load_device_description(CanopyContext canopy, const char *filename, const char *descriptionName);
bool canopy_load_device_description_file(CanopyContext canopy, FILE *file, const char *descriptionName);
bool canopy_load_device_description_string(CanopyContext canopy, const char *szDesc, const char *descriptionName);

CanopyEventEnum canopy_get_event_type(CanopyEventDetails event);
bool canopy_event_control_name_matches(CanopyEventDetails event, const char *name);
bool canopy_event_get_control_value_i8(CanopyEventDetails event, int8_t *outValue);
bool canopy_event_get_control_value_u8(CanopyEventDetails event, uint8_t *outValue);
bool canopy_event_get_control_value_i32(CanopyEventDetails event, int32_t *outValue);
bool canopy_event_get_control_value_u32(CanopyEventDetails event, uint32_t *outValue);
CanopyContext canopy_event_context(CanopyEventDetails event);

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
