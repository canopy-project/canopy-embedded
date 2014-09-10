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

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/*
 * To post a sensor sample:
 *
 *    canopy_easy_post_sample(
 *      CANOPY_CLOUD_HOST, "http://canopy.link:8080",
 *      CANOPY_DEVICE_ID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
 *      CANOPY_PROPERTY_NAME, "temperature",
 *      CANOPY_VALUE_FLOAT32, 4.0f,
 *      NULL
 *      );
 *
 *
 * To receive notification when a control changes.
 *
 *     canopy_easy_on_control_event(
 *      CANOPY_CLOUD_HOST, "http://canopy.link:8080",
 *      CANOPY_DEVICE_ID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
 *      CANOPY_PROPERTY_NAME, "master.on_off",
 *      CANOPY_CONTROL_CALLBACK, HandleOnOff,
 *      NULL
 *     );
 *
 */

/*
    canopy_global_server("https://canopy.link");
    canopy_global_device_uuid("16eeca6a-e8dc-4c54-b78e-6a7416803ca8");

    canopy_report_float32("temperature", 4.3f);
    canopy_report_float32("gps.longitude", 4.3f);
    canopy_report_send("gps.longitude", 4.3f);
*/



#define CANOPY_CLOUD_HOST (void *)0xCA0000
#define CANOPY_DEVICE_ID (void *)0xCA0001
#define CANOPY_PROPERTY_NAME (void *)0xCA0002
#define CANOPY_VALUE_FLOAT32 (void *)0xCA0003
#define CANOPY_CONTROL_CALLBACK (void *)0xCA0004

bool canopy_post_sample(void * params, ...);

bool canopy_on_control_event(void * params, ...);

/*
 * OLD INTERFACE -- Deprecated
 */

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
bool canopy_event_get_control_value_string(CanopyEventDetails event, const char **outValue);
bool canopy_event_get_control_value_bool(CanopyEventDetails event, bool *outValue);
bool canopy_event_get_control_value_i8(CanopyEventDetails event, int8_t *outValue);
bool canopy_event_get_control_value_u8(CanopyEventDetails event, uint8_t *outValue);
bool canopy_event_get_control_value_i16(CanopyEventDetails event, int16_t *outValue);
bool canopy_event_get_control_value_u16(CanopyEventDetails event, uint16_t *outValue);
bool canopy_event_get_control_value_i32(CanopyEventDetails event, int32_t *outValue);
bool canopy_event_get_control_value_u32(CanopyEventDetails event, uint32_t *outValue);
bool canopy_event_get_control_value_float32(CanopyEventDetails event, float *outValue);
bool canopy_event_get_control_value_float64(CanopyEventDetails event, double *outValue);
bool canopy_event_get_control_value_datetime(CanopyEventDetails event, struct tm *outValue);
CanopyContext canopy_event_context(CanopyEventDetails event);

CanopyReport canopy_begin_report(CanopyContext canopy);
bool canopy_report_void(CanopyReport report, const char *parameter);
bool canopy_report_string(CanopyReport report, const char *parameter, const char *value);
bool canopy_report_bool(CanopyReport report, const char *parameter, bool value);
bool canopy_report_i8(CanopyReport report, const char *parameter, int8_t value);
bool canopy_report_u8(CanopyReport report, const char *parameter, uint8_t value);
bool canopy_report_i16(CanopyReport report, const char *parameter, int16_t value);
bool canopy_report_u16(CanopyReport report, const char *parameter, uint16_t value);
bool canopy_report_i32(CanopyReport report, const char *parameter, int32_t value);
bool canopy_report_u32(CanopyReport report, const char *parameter, uint32_t value);
bool canopy_report_float32(CanopyReport report, const char *parameter, float value);
bool canopy_report_float64(CanopyReport report, const char *parameter, double value);
bool canopy_report_datetime(CanopyReport report, const char *parameter, const struct tm *value);

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
