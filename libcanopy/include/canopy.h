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

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/*
 * CanopyContext
 *
 *  A CanopyContext holds configuration options, connectivity state, and other
 *  internal state used by this library.  The libcanopy library automatically
 *  creates a "global context" that can be obtained by calling:
 *
 *      CanopyContext ctx = canopy_global_ctx();
 *
 *  Some routines, such as canopy_global_opt() implicitely operate on the
 *  global context.  Other routines, such as canopy_ctx_opt() operate on an
 *  explicitly passed-in CanopyContext object.  Some routines, such as
 *  canopy_notify(), can operate either way, depending on whether or not the
 *  CANOPY_CTX option is passed to the routine.
 */
typedef struct CanopyContext_t * CanopyContext;

/* 
 * CanopyNotifyTypeEnum
 *
 *  List of ways to deliver a notification to the device owner.
 */
typedef enum {
    /*
     * CANOPY_NOTIFY_LOW_PRIORITY
     *
     *  Send a low-priority message.  The Canopy Cloud Service will decide how
     *  to deliver the notification based on the user's preferences and system
     *  defaults.
     */
    CANOPY_NOTIFY_LOW_PRIORITY,

    /*
     * CANOPY_NOTIFY_MED_PRIORITY
     *
     *  Send a medium-priority message.  The Canopy Cloud Service will decide
     *  how to deliver the notification based on the user's preferences and
     *  system defaults.
     */
    CANOPY_NOTIFY_MED_PRIORITY,

    /*
     * CANOPY_NOTIFY_HIGH_PRIORITY
     *
     *  Send a medium-priority message.  The Canopy Cloud Service will decide
     *  how to deliver the notification based on the user's preferences and
     *  system defaults.
     */
    CANOPY_NOTIFY_HIGH_PRIORITY,

    /*
     * CANOPY_NOTIFY_SMS
     *
     *  Send an SMS text message to the device owner.
     */
    CANOPY_NOTIFY_SMS,

    /*
     * CANOPY_NOTIFY_EMAIL
     *
     *  Send an email to the device owner.
     */
    CANOPY_NOTIFY_EMAIL,

    /*
     * CANOPY_NOTIFY_IN_APP
     *
     *  Send an in-app notification to the device owner.
     */
    CANOPY_NOTIFY_IN_APP,
} CanopyNotifyTypeEnum;

/*
 * CanopyOnChangeFloat32Callback
 */
typedef int (*CanopyOnChangeFloat32Callback)(CanopyContext, const char *propname, float value, void *extra);



/*
 * CanopyPromise
 *
 *  A CanopyPromise is a synchronization primitive.  When the libcanopy library
 *  begins an asynchronous operation, it creates a CanopyPromise object that
 *  can be used to wait for the completion of the event.
 */
typedef struct CanopyPromise_t * CanopyPromise;

/* 
 * CanopyProtocolEnum
 *
 *  List of communication protocols.
 */
typedef enum {
    /*
     * CANOPY_PROTOCOL_NOOP
     *
     *  Don't actually communicate with the server.  This is sometimes useful
     *  for testing/debugging.
     */
    CANOPY_PROTOCOL_NOOP,

    /*
     * CANOPY_PROTOCOL_HTTP
     *
     *  Communicate with Canopy Cloud Service using HTTP-based protocol.
     */
    CANOPY_PROTOCOL_HTTP,

    /*
     * CANOPY_PROTOCOL_HTTPS
     *
     *  Communicate with Canopy Cloud Service using HTTPS-based protocol.
     */
    CANOPY_PROTOCOL_HTTPS,

    /*
     * CANOPY_PROTOCOL_WS
     *
     *  Communicate with Canopy Cloud Service using Websocket-based protocol.
     */
    CANOPY_PROTOCOL_WS,

    /*
     * CANOPY_PROTOCOL_WSS
     *
     *  Communicate with Canopy Cloud Service using a secure Websocket-based
     *  protocol.
     */
    CANOPY_PROTOCOL_WSS,
} CanopyProtocolEnum;

/*
 * CanopyResult
 *
 *  A CanopyResult represents the results of an operation.
 */
typedef struct CanopyResult_t * CanopyResult;

/*
 * CanopyResultCallback
 *
 *  Callback routine that is triggered when an asynchronous event has
 *  completed.
 */
typedef bool (*CanopyResultCallback)(CanopyResult result);

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
     * CANOPY_ERROR_CONNECTION_FAILED
     *
     *  Unable to connect to cloud server.
     */
    CANOPY_ERROR_CONNECTION_FAILED,
    /* 
     * CANOPY_ERROR_NOT_IMPLEMENTED
     *
     *  The requested operation has not been implemented on the current
     *  platform.
     */
    CANOPY_ERROR_NOT_IMPLEMENTED,

    /* 
     * CANOPY_ERROR_MISSING_REQUIRED_OPTION
     *
     *  A required configuration option must be supplied to the requested
     *  operation.
     */
    CANOPY_ERROR_MISSING_REQUIRED_OPTION,

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

    /* 
     * CANOPY_ERROR_INVALID_OPT
     *
     *  A supplied option was invalid.
     */
    CANOPY_ERROR_INVALID_OPT,

    /* 
     * CANOPY_ERROR_OUT_OF_MEMORY
     *
     *  You need to add more RAM :-)
     */
    CANOPY_ERROR_OUT_OF_MEMORY,

    /* 
     * CANOPY_ERROR_VARIABLE_NOT_SET
     *
     *  A Cloud Variable exists but its value could not be read because it has
     *  never been set.
     */
    CANOPY_ERROR_VARIABLE_NOT_SET,
} CanopyResultEnum;

/*
 * CanopyOptEnum
 *
 *  Identifiers for the options that can be provided to this library's
 *  routines.
 */
typedef enum {
    /*
     * CANOPY_INVALID_OPT
     *
     *  Invalid option. (Equal to NULL).
     */
    CANOPY_INVALID_OPT=0,

    /*
     * CANOPY_OPT_LIST_END
     *
     *  Special value used as marker at end of argument lists.  (Also == NULL).
     */
    CANOPY_OPT_LIST_END=0,

    /*
     * CANOPY_AUTO_SYNC
     */
    CANOPY_AUTO_SYNC,
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
     *  be a CanopyContext object or NULL (in which case the implicit global
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
     * CANOPY_NOTIFY_MSG
     *
     *  Configures the message body to send to the device
     *  owner via the Canopy Cloud Service.  The value must be a
     *  (char *) value.
     *
     *  Defaults to NULL.
     */
    CANOPY_NOTIFY_MSG,

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
     *  value.  Defaults to CANOPY_NOTIFY_MED_PRIORITY.
     */
    CANOPY_NOTIFY_TYPE,

    /*
     * CANOPY_VAR_CALLBACK_FLOAT32
     *
     *  Configures float32 control event callback.  The value must be a
     *  function pointer with the following type:
     *      
     *      int (*func)(CanopyContext, const char *propname, float value, void *extra)
     */
    CANOPY_VAR_CALLBACK_FLOAT32,

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
     * CANOPY_REPORT_POST_IMMEDIATELY
     *
     *  Configures when reports will be posted to the Canopy Cloud Service.
     *
     *  If true, a report will be sent immediately when canopy_post_sample() is
     *  called.
     *
     *  If false, a report will be sent the next time canopy_service() is
     *  called.  This allows multiple reports to be combined into a single
     *  payload, which can be much more efficient.
     *
     *  Defaults to true.
     */
    CANOPY_REPORT_POST_IMMEDIATELY,

    /*
     * CANOPY_VALUE_FLOAT32
     *
     *  Configures a 32-bit floating-point value.
     *  Used by canopy_post_sample().
     */
    CANOPY_VALUE_FLOAT32,

    CANOPY_STORE_VALUE_FLOAT32,
    CANOPY_SYNC_DURATION_MS,

    /*
     * CANOPY_VAR_DATATYPE
     */
    CANOPY_VAR_DATATYPE,

    /*
     * CANOPY_VAR_NAME
     *
     *  Configures the property name to use for subsequent sensor sample
     *  reporting and control callback registration.  The value must be a
     *  string, or NULL (if unconfigured).  Defaults to NULL.
     */
    CANOPY_VAR_NAME,

    /*
     * CANOPY_VAR_PUSH_PROTOCOL
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
    CANOPY_VAR_PUSH_PROTOCOL,

    CANOPY_VAR_PULL_PROTOCOL, // TODO: Rename?
} CanopyOptEnum;

/*
 * canopy_create_ctx -- Create a new Canopy context.
 *
 *  <copyOptsFrom> is the context to copy configuration options from, or NULL
 *      to use library defaults for all configuration options.  To copy the
 *      global context, use:
 *
 *          CanopyContext ctx = canopy_create_ctx(canopy_global_ctx());
 */
CanopyContext canopy_create_ctx(CanopyContext copyOptsFrom);

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
 *  The following parameters are supported:
 *
 *      CANOPY_CLOUD_SERVER
 *      CANOPY_CONTROL_PROTOCOL
 *      CANOPY_DEVICE_UUID
 *      CANOPY_NOTIFY_PROTOCOL
 *      CANOPY_NOTIFY_TYPE
 *      CANOPY_PROPERTY_NAME
 *      CANOPY_REPORT_PROTOCOL
 *
 *  The canopy_ctx_opt() function takes an all-or-nothing approach.  If any of
 *  the configuration defaults could not be set, then the function does nothing
 *  and returns an error.
 *
 *  Since canopy_ctx_opt() is implemented as a macro that automatically adds a
 *  sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_ctx_opt(ctx, ...) canopy_ctx_opt_impl(ctx, __VA_ARGS__, NULL)
CanopyResultEnum canopy_ctx_opt_impl(CanopyContext ctx, ...);

/*
 * canopy_destroy_ctx -- Destroy a Canopy context.
 */
void canopy_destroy_ctx(CanopyContext ctx);

/*
 * canopy_global_ctx -- Obtain the default "global context".
 */
CanopyContext canopy_global_ctx();

/*
 * canopy_global_opt -- Set global configuration defaults.
 *
 *  This is equivalent to:
 *      
 *      canopy_ctx_opt(canopy_global_ctx(), ...)
 *
 *  The canopy_global_opt() function takes an even number of arguments that
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
 *  The canopy_global_opt() function takes an all-or-nothing approach.  If
 *  any of the configuration defaults could not be set, then the function does
 *  not set any of them and returns an error.
 *
 *  Since canopy_global_opt() is implemented as a macro that automatically adds
 *  a sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_global_opt(...) canopy_ctx_opt_impl(canopy_global_ctx(), __VA_ARGS__, NULL)

/*
 * canopy_notify -- Send a notification to the device owner.
 *
 *  Instructs the Canopy Cloud Service to send a notification to the device's
 *  owner.  This notification may be:
 *      
 *      - An email.
 *      - An SMS.
 *      - An in-app notification.
 *
 *  Here is an example:
 *
 *          canopy_notify(
 *              CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6",
 *              CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS,
 *              CANOPY_NOTIFY_MSG, "Running low on green toner!"
 *          );
 *
 *  If the device is connected to the Canopy Cloud Service, the notification
 *  will be sent to the device owner.  This feature is only available to
 *  registered (non-anonymous) devices.
 *
 *  This routine accepts the following parameters:
 *
 *      CANOPY_CLOUD_SERVER
 *      CANOPY_CTX
 *      CANOPY_DEVICE_UUID
 *      CANOPY_NOTIFY_MSG
 *      CANOPY_NOTIFY_PROTOCOL
 *      CANOPY_NOTIFY_TYPE
 *      CANOPY_PROMISE
 *
 *  Since canopy_post_sample is implemented as a macro that automatically adds
 *  a sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_notify(...) canopy_notify_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_notify_impl(void * start, ...);

/*
 * canopy_var_on_change -- Register a remote control event callback.
 *
 *  Registers a callback that gets triggered when a Clouf Variable change event
 *  is recieved from the Canopy Cloud Service.  The callback gets triggered
 *  from within canopy_sync() or canopy_event_loop().
 */
#define canopy_var_on_change(...) canopy_var_on_change_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_var_on_change_impl(void *start, ...);

/*
 * canopy_var_read -- Read the local value of a Cloud Variable
 */
#define canopy_var_read(...) canopy_var_read_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_var_read_impl(void * start, ...);

/*
 * canopy_var_config - Configure a Cloud Variable.
 *
 *  This routine behaves identically to canopy_var_set, except that
 *  CANOPY_VALUE_* parameters are not allowed.
 */
#define canopy_var_config(...) canopy_var_config_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_var_config_impl(void * start, ...);

typedef enum
{
    CANOPY_INVALID_DATATYPE,
    CANOPY_VOID,
    CANOPY_STRING,
    CANOPY_BOOL,
    CANOPY_INT8,
    CANOPY_UINT8,
    CANOPY_INT16,
    CANOPY_UINT16,
    CANOPY_INT32,
    CANOPY_UINT32,
    CANOPY_FLOAT32,
    CANOPY_FLOAT64,
    CANOPY_DATETIME
} CanopyDatatypeEnum;

/*
 * canopy_var_set -- Set the value of a Cloud Variable.
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
 *  Since canopy_post_sample is implemented as a macro that automatically adds
 *  a sentinal NULL value, there is no need to end the argument list with NULL.
 */
#define canopy_var_set(...) canopy_var_set_impl(NULL, __VA_ARGS__, NULL)
CanopyResultEnum canopy_var_set_impl(void * start, ...);


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
        CanopyResultCallback cb);

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
        CanopyResultCallback cb);

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
        CanopyResultCallback cb);

/*
 * canopy_promise_result -- Get the result of a completed asyncrhonous
 *  operation.
 *
 *  If the asynchronous operation hasn't completed yet, this returns CANOPY_ERROR_PROMISE_NOT_COMPLETE
 */
CanopyResultEnum canopy_promise_result(CanopyPromise promise);

/*
 * canopy_promise_wait -- Wait for the completion of an asynchronous operation.
 *
 *  This routine blocks the current thread until an asynchronous operation has
 *  completed.
 */
CanopyResultEnum canopy_promise_wait(CanopyPromise promise, ...);

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
CanopyResultEnum canopy_run_event_loop_impl(void *start, ...);

/*
 * canopy_sync -- Perform outstanding tasks and triggers callbacks.
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
#define canopy_sync(...) canopy_sync_impl_temp(NULL, ##__VA_ARGS__)
#define canopy_sync_impl_temp(...) canopy_sync_impl(__VA_ARGS__, NULL)
CanopyResultEnum canopy_sync_impl(void *start, ...);

#endif
