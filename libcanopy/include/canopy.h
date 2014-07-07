/*
 * Copyright 2014 - Greg Prisament
 */

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct CanopyContext_t * CanopyContext;
typedef struct CanopyReport_t * CanopyReport;
typedef struct CanopyEventDetails_t * CanopyEventDetails;
typedef struct CanopyProvisionResults_t * CanopyProvisionResults;

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
bool canopy_set_auto_reconnect(CanopyContext canopy, bool enabled);
bool canopy_set_device_id(CanopyContext canopy, const char *uuid);
bool canopy_set_device_id_filename(CanopyContext canopy, const char *filename);

CanopyProvisionResults canopy_provision(CanopyContext canopy, const char *cloudUsername, const char *cloudPassword);
/* Returned ptr is only valid until canopy_free_provision_results is called */
const char * canopy_provision_get_uuid(CanopyProvisionResults results);
void canopy_free_provision_results(CanopyProvisionResults results);

bool canopy_connect(CanopyContext canopy);

bool canopy_register_event_callback(CanopyContext canopy, CanopyEventCallbackRoutine fn, void *extra);

bool canopy_load_sddl(CanopyContext canopy, const char *filename, const char *className);
bool canopy_load_sddl_file(CanopyContext canopy, FILE *file, const char *className);
bool canopy_load_sddl_string(CanopyContext canopy, const char *sddl, const char *className);

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

/*
 * On Unix, Linux, and MacOSX, looks for file, in this order:
 *
 * $CANOPY_HOME/<filename>
 * ~/.canopy/<filename>
 * SYSCONFDIR/<filename>
 * /etc/canopy/<filename>
 */
FILE * canopy_open_config_file(const char* filename);

/*
 * Uses CANOPY_UUID env var, or, if not set, then config file "uuid".
 *
 * Caller must free returned string.
 */
char * canopy_read_system_uuid();

/* returns "https" or "http" */
const char * canopy_get_web_protocol(CanopyContext ctx);
const char * canopy_get_cloud_host(CanopyContext ctx);
uint16_t canopy_get_cloud_port(CanopyContext ctx);

const char * canopy_get_sysconfigdir();

#endif
