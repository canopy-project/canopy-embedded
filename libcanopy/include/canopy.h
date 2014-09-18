/*
 * Copyright 2014 SimpleThings, Inc.
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

/*
 *  The libcanopy library provides functionality for cloud-based monitoring,
 *  control and notifications to firmware developers.
 *
 *
 *  MONITORING:
 *
 *      The following program demonstrates how to send a sensor sample to the
 *      cloud:
 * 
 *      ----------------------------------------------------------------------
 *      #include <canopy.h>
 *
 *      int main(void) 
 *      {
 *          canopy_post_sample(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_PROPERTY_NAME, "temperature",
 *              CANOPY_VALUE_FLOAT32, 98.0f
 *          );
 *          return 0;
 *      }
 *      ----------------------------------------------------------------------
 *
 *      When run, the data sample is sent using HTTP POST to the server
 *      "http://canopy.link".  The libcanopy library can be configured to use a
 *      different protocol or send the data to some other server that is
 *      running the Canopy Cloud Service.
 *
 *      Replace the UUID in the example above with a type-4 UUID specific for
 *      your device.  If your device needs a UUID, you can generate one from
 *      the linux command-line using:
 *
 *          uuid -v4
 *
 *      If you then compile and run the program, without doing anything else,
 *      you can manage this device from a web browser by going to:
 *
 *          http://canopy.link/9dfe2a00-efe2-45f9-a84c-8afc69caf4e6
 *
 *          (Of course, you must replace the UUID with your device's UUID).
 *
 *      Initially, Canopy considers the device an "Anonymous Device" since it
 *      has not been registered or associated with an account.  You can perform
 *      basic monitoring and control of an Anonymous Device simply by knowing
 *      the above access URL.  However, for greater security and functionality,
 *      you should register the device, which can be accomplished through the
 *      web interface.
 *
 *
 *  CONTROL:
 *
 *      The following example program shows how a firmware callback can be
 *      triggered based on a web UI action.
 *
 *      -----------------------------------------------------------------------
 *      #include <canopy.h>
 *
 *      int handle_brightness(const char *propname, float value) {
 *          printf("Brightness set to %f\n", value);
 *          return 0;
 *      }
 *
 *      int main(void) {
 *          canopy_on_change(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_PROPERTY_NAME, "brightness",
 *              CANOPY_ON_CHANGE_FLOAT32_CALLBACK, handle_brightness
 *          );
 *          canopy_run_event_loop();
 *          return 0;
 *      }
 *      -----------------------------------------------------------------------
 *
 *      While this program is running, you can manipulate the device's
 *      "brightness" property from any web browser by going to:
 *
 *          http://canopy.link/9dfe2a00-efe2-45f9-a84c-8afc69caf4e6
 *
 *
 *  NOTIFICATIONS:
 *
 *      If the device has been registered with the Canopy Cloud Service and has
 *      been associated with an account, the firmware can send notifications to
 *      the owner of the device.  The firmware can send SMS, email, or in-app
 *      notifications.
 *
 *      -----------------------------------------------------------------------
 *      #include <canopy.h>
 *
 *      int main(void) {
 *          canopy_notify(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS,
 *              CANOPY_NOTIFY_MSG, "Running low on green toner!"
 *          );
 *          return 0;
 *      }
 *      -----------------------------------------------------------------------
 *
 *
 *  LIBCANOPY OPTIONS AND CONTEXTS:
 *
 *      Typically, you will need to make several calls to libcanopy within a
 *      single application.  Instead of passing the same options every time you
 *      call a routine, many libcanopy options can be configured globally.  For
 *      example:
 *
 *      ----------------------------------------------------------------------
 *      #include <canopy.h>
 *
 *      int main(void) 
 *      {
 *          canopy_global_opt(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_CLOUD_SERVER, "localhost:8080",
 *              CANOPY_REPORT_PROTOCOL, CANOPY_PROTOCOL_HTTPS,
 *              CANOPY_NOTIFY_PROTOCOL, CANOPY_PROTOCOL_HTTPS,
 *              CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS
 *          );
 *
 *          canopy_post_sample(
 *              CANOPY_PROPERTY_NAME, "temperature",
 *              CANOPY_VALUE_FLOAT32, 98.0f
 *          );
 *          canopy_post_sample(
 *              CANOPY_PROPERTY_NAME, "humidity",
 *              CANOPY_VALUE_FLOAT32, 50.0f
 *          );
 *          canopy_notify(
 *              CANOPY_NOTIFY_MSG, "Running low on green toner!",
 *          );
 *          return 0;
 *      }
 *      ----------------------------------------------------------------------
 *
 *      Alternatively, you can explicitely create a Canopy Context object (or
 *      several Canopy Context objects) to hold Canopy configuration settings.
 *
 *      ----------------------------------------------------------------------
 *      #include <canopy.h>
 *      #include <assert.h>
 *
 *      int main(void) 
 *      {
 *          CanopyContext ctx;
 *          ctx = canopy_create_ctx(NULL);
 *          assert(ctx);
 *
 *          canopy_ctx_opt(ctx,
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_CLOUD_SERVER, "localhost:8080",
 *              CANOPY_REPORT_PROTOCOL, CANOPY_PROTOCOL_HTTPS,
 *              CANOPY_NOTIFY_PROTOCOL, CANOPY_PROTOCOL_HTTPS,
 *              CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS
 *          );
 *
 *          canopy_post_sample(
 *              CANOPY_CTX, ctx,
 *              CANOPY_PROPERTY_NAME, "temperature",
 *              CANOPY_VALUE_FLOAT32, 98.0f
 *          );
 *          canopy_post_sample(
 *              CANOPY_CTX, ctx,
 *              CANOPY_PROPERTY_NAME, "humidity",
 *              CANOPY_VALUE_FLOAT32, 50.0f
 *          );
 *          canopy_notify(
 *              CANOPY_CTX, ctx,
 *              CANOPY_NOTIFY_MSG, "Running low on green toner!",
 *          );
 *
 *          canopy_ctx_destroy(ctx);
 *          return 0;
 *      }
 *      ----------------------------------------------------------------------
 *      
 *
 *  PROMISES:
 *
 *      Many operations performed by libcanopy are asynchronous.  For example,
 *      canopy_post_sample() returns immediately, but may begin an asynchronous
 *      HTTP request/response communication in another thread.
 *
 *      Promises make it easy to wait for asynchronous operations to complete
 *      and to register callbacks relating to libcanopy's asynchronous
 *      operations.
 *
 *      The following example shows how to wait for the completion of a
 *      canopy_post_sample() operation using a Promise.
 *
 *      ----------------------------------------------------------------------
 *      #include <canopy.h>
 *      #include <stdio.h>
 *
 *      int main(void)
 *      {
 *          CanopyPromise promise;
 *
 *          canopy_post_sample(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_PROPERTY_NAME, "temperature",
 *              CANOPY_VALUE_FLOAT32, 98.0f,
 *              CANOPY_PROMISE, &promise
 *          );
 *
 *          canopy_promise_wait(promise, CANOPY_TIMEOUT, 10.0f);
 *          if (canopy_promise_result(promise) == CANOPY_SUCCESS)
 *          {
 *              printf("Sample successfully sent to server!\n");
 *          }
 *          else
 *          {
 *              printf("Error sending sample to server!\n");
 *              canopy_print_error();
 *          }
 *
 *          canopy_destroy_promise(promise);
 *
 *      }
 *      
 *      ----------------------------------------------------------------------
 *
 */

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/*
 * CanopyCtx
 *
 *  A CanopyCtx holds configuration options, connectivity state, and other
 *  internal state used by this library.  The libcanopy library automatically
 *  creates a "global context" that can be obtained by calling:
 *
 *      CanopyCtx ctx = canopy_global_ctx();
 *
 *  Some routines, such as canopy_global_opt() implicitely operate on the
 *  global context.  Other routines, such as canopy_ctx_opt() operate on an
 *  explicitly passed-in CanopyCtx object.  Some routines, such as
 *  canopy_notify(), can operate either way, depending on whether or not the
 *  CANOPY_CTX option is passed to the routine.
 */
typedef struct CanopyCtx_t CanopyCtx;


/* 
 * CanopyResultEnum
 *
 *  Contains success & error codes returned by many of the routines in this
 *  library.
 */
typedef enum {
    /*
     * CANOPY_SUCCESS
     *
     *  Command succeeded.
     */
    CANOPY_SUCCESS,

    /* 
     * CANOPY_ERROR_UNKNOWN
     *
     *  An unknown error occured.
     */
    CANOPY_ERROR_UNKNOWN,

    /* 
     * CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED
     *
     *  The requested communications protocol is not supported by the
     *  implementation.
     */
    CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED,

    /* 
     * CANOPY_ERROR_REDUNDANT_PARAMETER
     *
     *  A single parameter was provided too many times to a routine.
     */
    CANOPY_ERROR_REDUNDANT_PARAMETER,

    /* 
     * CANOPY_ERROR_PROMISE_NOT_COMPLETE
     *
     *  The requested operation cannot be performed on a non-completed promise.
     */
    CANOPY_ERROR_PROMISE_NOT_COMPLETE,

} CanopyResultEnum;

/*
 * CanopyOptEnum
 *
 *  Identifiers for the options that can be provided to this library's
 *  routines.
 */
typedef enum {
    /*
     * CANOPY_CLOUD_SERVER
     * 
     *  Configures the hostname and port of the Canopy Cloud Service to
     *  interact with.  The value must be a string, such as "canopy.link" or
     *  "localhost:8080".  Defaults to "canopy.link".
     */
    CANOPY_CLOUD_SERVER,

    /*
     * CANOPY_CONTROL_PROTOCOL
     *
     *  Configures the protocol to use for recieving control events from the
     *  Canopy Cloud Service.  The value must be a CanopyProtocolEnum value.
     *  Currently, the following protocols are supported:
     *
     *      CANOPY_PROTOCOL_WS
     *
     *  Defaults to CANOPY_PROTOCOL_WS.
     */
    CANOPY_CONTROL_PROTOCOL,

    /*
     * CANOPY_CTX
     *
     *  Selects the Canopy context that the routine should use.  The value must
     *  be a CanopyCtx object or NULL (in which case the implicit global
     *  context will be used).  Defaults to NULL.
     */
    CANOPY_CTX,

    /*
     * CANOPY_DEVICE_UUID
     *
     *  Configures the UUID of the current device.  The value must be a string
     *  containing a type-4 UUID, such as
     *  "16eeca6a-e8dc-4c54-b78e-6a7416803ca8", or NULL if unconfigured.
     *  Defaults to nULL.
     */
    CANOPY_DEVICE_UUID,

    /*
     * CANOPY_NOTIFY_PROTOCOL
     *
     *  Configures the protocol to use for sending notifications to the device
     *  owner via the Canopy Cloud Service.  The value must be a
     *  CanopyProtocolEnum value.  Currently, the following protocols are
     *  supported:
     *
     *      CANOPY_PROTOCOL_HTTP
     *      CANOPY_PROTOCOL_WS
     *
     *  Defaults to CANOPY_PROTOCOL_HTTP.
     */
    CANOPY_NOTIFY_PROTOCOL,

    /*
     * CANOPY_NOTIFY_TYPE
     *
     *  Configures the desired method of notifying the device owner (SMS,
     *  email, in-app notice, etc).  The value must be a CanopyNotifyTypeEnum
     *  value.  Defaults to CANOPY_NOTIFY_DEFAULT.
     */
    CANOPY_NOTIFY_TYPE,

    /*
     * CANOPY_PROMISE
     *
     *  Instructs an asynchronous routine to create a CanopyPromise object,
     *  which allows the application to wait for completion of the operation
     *  and to get the operation's status.
     *
     *  The value must be the address of an uninitialized CanopyPromise
     *  variable.  This variable will be assigned the newly-created
     *  CanopyPromise.
     *
     *  
     */
    CANOPY_PROMISE,

    /*
     * CANOPY_PROPERTY_NAME
     *
     *  Configures the property name to use for subsequent sensor sample
     *  reporting and control callback registration.  The value must be a
     *  string, or NULL (if unconfigured).  Defaults to NULL.
     */
    CANOPY_PROPERTY_NAME,

    /*
     * CANOPY_REPORT_PROTOCOL
     *
     *  Configures the protocol to use for reporting sensor data to the Canopy
     *  Cloud Service.  The value must be a CanopyProtocolEnum value.
     *  Currently, the following protocols are supported:
     *
     *      CANOPY_PROTOCOL_HTTP
     *      CANOPY_PROTOCOL_WS
     *
     *  Defaults to CANOPY_PROTOCOL_HTTP.
     */
    CANOPY_REPORT_PROTOCOL
} CanopyOptEnum;

/*
 * canopy_create_ctx -- Create a new Canopy context.
 *
 *  <copyOptsFrom> is the context to copy configuration options from, or NULL
 *      to use library defaults for all configuration options.  To copy the
 *      global context, use:
 *
 *          CanopyCtx ctx = canopy_create_ctx(canopy_global_ctx());
 */
CanopyCtx canopy_create_ctx(CanopyCtx copyOptsFrom);

/*
 * canopy_ctx_opt -- Set configuration options for a particular context.
 *
 *  The <ctx> parameter specifies the context to change configuration options
 *  for.  After that, the canopy_ctx_opt() function takes an even number of
 *  arguments that must alternate between parameter identifiers and values.
 *
 *      canopy_ctx_opt(ctx,
 *          CANOPY_CLOUD_SERVER, "localhost:8080",
 *          CANOPY_DEVICE_UUID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
 *          CANOPY_REPORT_PROTOCOL, CANOPY_REPORT_PROTOCOL_HTTP
 *      );
 *      
 *  For each PARAM, VALUE pair, the context's default for that PARAM is set.
 *
 *  The canopy_ctx_opt() function takes an all-or-nothing approach.  If any of
 *  the configuration defaults could not be set, then the function does nothing
 *  and returns an error.
 *
 *  Since canopy_ctx_opt() is implemented as a macro that automatically adds a
 *  sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_ctx_opt(ctx, ...) canopy_ctx_opt_impl(ctx, __VA_ARGS__, NULL)
CanopyResultEnum canopy_ctx_opt_impl(CanopyCtx ctx, ...);

/*
 * canopy_destroy_ctx -- Destroy a Canopy context.
 */
void canopy_destroy_ctx(CanopyCtx ctx);

/*
 * canopy_global_ctx -- Obtain the default "global context".
 */
CanopyCtx canopy_global_ctx();

/*
 * canopy_global_opt -- Set global configuration defaults.
 *
 *  This is equivalent to:
 *      
 *      canopy_ctx_opt(canopy_global_ctx(), ...)
 *
 *  The canopy_global_config() function takes an even number of arguments that
 *  must alternate between parameter identifiers and values.
 *
 *      canopy_global_opt(
 *          CANOPY_CLOUD_SERVER, "localhost:8080",
 *          CANOPY_DEVICE_UUID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
 *          CANOPY_REPORT_PROTOCOL, CANOPY_REPORT_PROTOCOL_HTTP
 *      );
 *
 *  For each PARAM, VALUE pair, the global default for that PARAM is set.
 *  These global default values are used by the following routines (as long as
 *  the CANOPY_CTX parameter is not supplied):
 *      
 *      canopy_post_sample()
 *      canopy_on_change()
 *      canopy_notify()
 *
 *  The canopy_global_config() function takes an all-or-nothing approach.  If
 *  any of the configuration defaults could not be set, then the function does
 *  not set any of them and returns an error.
 *
 *  Since canopy_global_opt() is implemented as a macro that automatically adds
 *  a sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_global_opt(...) canopy_ctx_opt_impl(canopy_global_ctx(), __VA_ARGS__, NULL)

/*
 * canopy_post_sample -- Post sensor data sample to the Canopy Cloud Service.
 *
 *  Provides a convenient and flexible routine for posting data samples to the
 *  Canopy Cloud Service.  It takes a variable number of arguments that must
 *  alternate between parameter identifiers and values.
 *
 *  A simple example:
 *
 *      canopy_post_sample(
 *          CANOPY_CLOUD_SERVER, "canopy.link",
 *          CANOPY_DEVICE_UUID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
 *          CANOPY_PROPERTY_NAME, "temperature",
 *          CANOPY_VALUE_FLOAT32, 4.0f
 *      );
 *
 *  In your web browser, you can see the posted data by going to:
 *
 *      http://canopy.link/16eeca6a-e8dc-4c54-b78e-6a7416803ca8
 *
 *      (Of course, replace the UUID with the UUID of your device).
 *
 *  This routine accepts the following parameters:
 *
 *      CANOPY_CLOUD_SERVER
 *      CANOPY_REPORT_PROTOCOL
 *      CANOPY_CTX
 *      CANOPY_DEVICE_UUID
 *      CANOPY_PROPERTY_NAME
 *      CANOPY_VALUE_FLOAT32
 *      CANOPY_VALUE_FLOAT64
 *      CANOPY_VALUE_INT8
 *      CANOPY_VALUE_UINT8
 *      CANOPY_VALUE_INT16
 *      CANOPY_VALUE_UINT16
 *      CANOPY_VALUE_INT32
 *      CANOPY_VALUE_UINT32
 *
 *  At most one of the CANOPY_VALUE_* parameters may be supplied.  If more than
 *  one CANOPY_VALUE_* parameter is supplied, the error
 *  CANOPY_ERROR_REDUNDANT_PARAMETER will be returned.
 *
 *  Since canopy_post_sample is implemented as a macro that automatically adds a
 *  sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_post_sample(...) canopy_post_sample_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_post_sample_impl(start, ...);


/*
 * canopy_promise_wait -- Wait for the completion of an asynchronous operation.
 *
 *  This routine blocks the current thread until an asynchronous operation has
 *  completed.
 */
CanopyResultEnum canopy_promise_wait(CanopyPromise promise, ...);

/*
 * canopy_promise_result -- Get the result of a completed asyncrhonous
 *  operation.
 *
 *  If the asynchronous operation hasn't completed yet, this returns CANOPY_ERROR_PROMISE_NOT_COMPLETE
 */
CanopyResultEnum canopy_promise_result(CanopyPromise promise);

/*
 * canopy_promise_on_success -- Register a success callback for an async op.
 *
 *  Registers a callback that gets triggered when an asynchronous operation has
 *  completed successfully.
 *
 *  This callback will always be triggered from within canopy_service or
 *  canopy_run_event_loop.
 */
CanopyResultEnum canopy_promise_on_success(
        CanopyPromise promise, 
        CanopySuccessCallback cb);
/*
 * canopy_promise_on_failure -- Register a failure callback for an async op.
 *
 *  Registers a callback that gets triggered when an asyhchronous operation has
 *  failed.
 *
 *  This callback will always be triggered from within canopy_service or
 *  canopy_run_event_loop.
 *
 */
CanopyResultEnum canopy_promise_on_failure(
        CanopyPromise promise, 
        CanopyFailureCallback cb);

/*
 * canopy_promise_on_done -- Register a completion callback for an async op.
 *
 *  Registers a callback that gets triggered when an asyhchronous operation has
 *  completed, whether or not it succeeded.  This is triggered in addition to
 *  any registered canopy_promise_on_success and canopy_promise_on_failure
 *  callbacks.
 *
 *  This callback will always be triggered from within canopy_service or
 *  canopy_run_event_loop.
 *
 */
CanopyResultEnum canopy_promise_on_done(
        CanopyPromise promise, 
        CanopyFailureCallback cb);

/*
 * canopy_run_event_loop -- Run the Canopy event loop.
 *
 *  This is roughly equivalent to:
 *
 *      while (1) {
 *          canopy_service(...);
 *      }
 *
 */
#define canopy_run_event_loop(...) canopy_run_event_loop_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_run_event_loop(start, ...);

/*
 * canopy_service -- Perform outstanding tasks and triggers callbacks.
 *
 *  This routine instructs libcanopy to perform any outstanding tasks.  These
 *  tasks may include:
 *
 *      - Servicing websocket connection(s).
 *      - Triggering control event callbacks and other callbacks.
 *
 *  This only performs outstanding tasks for a single context.  If CANOPY_CTX
 *  is omitted or NULL, the global context is serviced.
 */
#define canopy_service(...) canopy_service_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_service_impl(start, ...);






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
