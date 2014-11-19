// Copyright 2014 SimpleThings, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CANOPY_INCLUDED
#define CANOPY_INCLUDED

#include <stdbool.h>
#include <stdint.h>

// A CanopyContext holds the internal state used by the libcanopy library.
// 
// The state contained in a CanopyContext includes:
//   - Internal representation of all Cloud Variables.
//   - Configuration options.
//   - State needed for the connection to the cloud server.
//   - Error information.
//   - Multi-threading and asynchronous results.
//
// Most routines in this library require a CanopyContext as the first
// parameter.
//
// Creating a CanopyContext initializes this library:
//
//     CanopyContext canopy = canopy_init_context();
//
typedef struct CanopyContext_t * CanopyContext;

// A CanopyVarValue represents a value that can be assigned to Cloud Variable.
typedef struct STCloudVarValue_t * CanopyVarValue;

// A CanopyVarReader represents a destination for reading a Cloud Variable.
typedef struct STCloudVarReader_t * CanopyVarReader;

typedef int (*CanopyOnChangeCallback)(CanopyContext, const char *, void *);

// A CanopyPromise is a synchronization primitive.  When the libcanopy library
// begins an asynchronous operation, it creates a CanopyPromise object that
// can be used to wait for the completion of the event.
typedef struct CanopyPromise_t * CanopyPromise;

// Must match SDDLDatatypeEnum exactly!
typedef enum
{
    CANOPY_INVALID_DATATYPE,
    CANOPY_DATATYPE_VOID,
    CANOPY_DATATYPE_STRING,
    CANOPY_DATATYPE_BOOL,
    CANOPY_DATATYPE_INT8,
    CANOPY_DATATYPE_UINT8,
    CANOPY_DATATYPE_INT16,
    CANOPY_DATATYPE_UINT16,
    CANOPY_DATATYPE_INT32,
    CANOPY_DATATYPE_UINT32,
    CANOPY_DATATYPE_FLOAT32,
    CANOPY_DATATYPE_FLOAT64,
    CANOPY_DATATYPE_DATETIME,
    CANOPY_DATATYPE_STRUCT,
    CANOPY_DATATYPE_ARRAY
} CanopyDatatypeEnum;

// Must match SDDLDirectionEnum exactly!
typedef enum
{
    CANOPY_INVALID_DIRECTION,
    CANOPY_DIRECTION_INHERIT,
    CANOPY_DIRECTION_INOUT,
    CANOPY_DIRECTION_IN,
    CANOPY_DIRECTION_OUT
} CanopyDirectionEnum;
// 
// CanopyResultEnum
//
//  Contains success & error codes returned by many of the routines in this
//  library.
//
typedef enum
{
    // Command succeeded.
    CANOPY_SUCCESS,

    // An unknown error occured.
    CANOPY_ERROR_UNKNOWN,

    // Unable to connect to cloud server.
    CANOPY_ERROR_CONNECTION_FAILED,

    // The requested operation has not been implemented on the current
    // platform.
    CANOPY_ERROR_NOT_IMPLEMENTED,

    // A required configuration option must be supplied to the requested
    // operation.
    CANOPY_ERROR_MISSING_REQUIRED_OPTION,

    // The requested communications protocol is not supported by the
    // implementation.
    CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED,

    // A single parameter was provided too many times to a routine.
    CANOPY_ERROR_REDUNDANT_PARAMETER,

    //  The requested operation cannot be performed on a non-completed promise.
    CANOPY_ERROR_PROMISE_NOT_COMPLETE,

    //  A supplied option was invalid.
    CANOPY_ERROR_INVALID_OPT,

    //  You need to add more RAM :-)
    CANOPY_ERROR_OUT_OF_MEMORY,

    // Cloud Variable declaration could not be parsed.
    CANOPY_ERROR_BAD_VARIABLE_DECLARATION,

    // The requested cloud variable has not been initialized with
    // canopy_var_init(...).
    CANOPY_ERROR_VARIABLE_NOT_INITIALIZED,

    // A Cloud Variable cannot be initialized because it has already been
    // initialized.
    CANOPY_ERROR_VARIABLE_ALREADY_INITIALIZED,

    //  A Cloud Variable exists but its value could not be read because it has
    //  never been set.
    CANOPY_ERROR_VARIABLE_NOT_SET,
   
    // A single-use Cloud Variable value has already been used as an argument.
    CANOPY_ERROR_SINGLE_USE_VALUE_ALREADY_USED,

    // The provided datatype does not match the dataype expected.
    CANOPY_ERROR_INCORRECT_DATATYPE,

    // An invalid value was provided as a parameter.
    CANOPY_ERROR_INVALID_VALUE,

    // Unable to parse received payload.
    CANOPY_ERROR_PARSING_PAYLOAD,

    // Error processing received payload.
    CANOPY_ERROR_PROCESSING_PAYLOAD,

    // Cannot modify variable that has "in" direction.
    CANOPY_ERROR_CANNOT_MODIFY_INPUT_VARIABLE,

} CanopyResultEnum;

// CanopyGlobalOptEnum
//
// Identifiers for the options that can be provided to canopy_set_global_opt
typedef enum
{
    //  Invalid option. (Equal to NULL).
    CANOPY_INVALID_GLOBAL_OPT=0,
    
    // Special value used as marker at end of argument lists.  (Also == NULL).
    CANOPY_GLOBAL_OPT_LIST_END=0,

    // Enable/disable logging within libcanopy.  Value must be a boolean.
    // Defaults to true.
    CANOPY_LOG_ENABLED,

    // Filename of log file to write to.  The value must be a string.
    // Defaults to "~/.canopy/log".
    CANOPY_LOG_FILE,

    // Logging level:
    //
    //  0 = Trace, Debug, Info, Warn, Error & Fatal messages
    //  1 = Debug, Info, Warn, Error & Fatal messages
    //  2 = Info, Warn, Error & Fatal messages
    //  3 = Warn, Error & Fatal messages
    //  4 = Error & Fatal messages
    //  5 = Fatal messages only
    //
    // The value must be an integer.  Defaults to 2.
    CANOPY_LOG_LEVEL,

    // Global configuration option that determines if communication payloads
    // (requests and responses) should be included in the log.  If true,
    // payloads will be included as DEBUG messages.  Otherwise, they will not
    // be logged.  Defaults to false.
    CANOPY_LOG_PAYLOADS,
} CanopyGlobalOptEnum;

// CanopyOptEnum
//
// Identifiers for the options that can be provided to canopy_set_opt
typedef enum
{
    //  Invalid option. (Equal to NULL).
    CANOPY_INVALID_OPT=0,

    // Special value used as marker at end of argument lists.  (Also == NULL).
    CANOPY_OPT_LIST_END=0,

    // Configures the hostname and port of the Canopy Cloud Service to interact
    // with.  The value must be a string, such as "canopy.link" or
    // "localhost:8080".  Defaults to "canopy.link".
    CANOPY_CLOUD_SERVER,

    // Configures the UUID of the current device.  The value must be a string
    // containing a type-4 UUID, such as
    // "16eeca6a-e8dc-4c54-b78e-6a7416803ca8", or NULL if unconfigured.
    // Defaults to nULL.
    CANOPY_DEVICE_UUID,

    // Configures the protocol to use for reporting Cloud Variable changes to
    // the Canopy Cloud Service.  The value must be a CanopyProtocolEnum value.
    // Currently, the following protocols are supported:
    //
    //     CANOPY_PROTOCOL_NOOP
    //     CANOPY_PROTOCOL_HTTP
    //     CANOPY_PROTOCOL_WS
    //
    // Defaults to CANOPY_PROTOCOL_HTTP
    CANOPY_VAR_SEND_PROTOCOL,

    // Configures the protocol to use for receiving Cloud Variable changes from
    // the Canopy Cloud Service.  The value must be a CanopyProtocolEnum value.
    // Currently, the following protocols are supported:
    //
    //     CANOPY_PROTOCOL_NOOP
    //     CANOPY_PROTOCOL_WS
    //
    // Defaults to CANOPY_PROTOCOL_WS
    CANOPY_VAR_RECV_PROTOCOL,

    // Configures whether canopy_sync blocks the calling thread.  The value
    // must be a boolean.  If true, the calling thread will block until the
    // sync operation completes (either successfully, or with an error, or
    // times out).  If false, the call to canopy_sync will begin synchronizing
    // in another thread and them immediately return.
    CANOPY_SYNC_BLOCKING,

    // Configures the amount of time to allow canopy_sync synchronization to
    // take, in milliseconds.  Must be a nonnegative integer.  If
    // CANOPY_SYNC_BLOCKING is enabled, then this specifies the maximum amount
    // of time the canopy_sync command will block for.  If CANOPY_SYNC_BLOCKING
    // is disabled, then this specifies the maximum amount of time the spawned
    // synchronization thread will exist for.
    CANOPY_SYNC_TIMEOUT_MS
} CanopyOptEnum;

typedef enum
{
    CANOPY_VAR_INVALID_OPT=0,
    CANOPY_VAR_OPT_LIST_END=0,
    CANOPY_VAR_DATATYPE,
    CANOPY_VAR_DIRECTION,
    CANOPY_VAR_MIN_VALUE,
    CANOPY_VAR_MAX_VALUE,
    CANOPY_VAR_CHILD,
    CANOPY_VAR_DESCRIPTION,
} CanopyVarConfigEnum;

// CanopyProtocolEnum
//
// List of communication protocols.
typedef enum {
    // Don't actually communicate with the server.  This is sometimes useful
    // for testing/debugging.
    CANOPY_PROTOCOL_NOOP,

    // Communicate with Canopy Cloud Service using HTTP-based protocol.
    CANOPY_PROTOCOL_HTTP,

    // Communicate with Canopy Cloud Service using HTTPS-based protocol.
    CANOPY_PROTOCOL_HTTPS,

    // Communicate with Canopy Cloud Service using Websocket-based protocol.
    CANOPY_PROTOCOL_WS,

    // Communicate with Canopy Cloud Service using a secure Websocket-based
    // protocol.
    CANOPY_PROTOCOL_WSS,
} CanopyProtocolEnum;

// Initialize libcanopy and create a context.  
//
// This may be called multiple times to create multiple contexts, which may be
// useful for unit testing, or if you need to talk to multiple canopy servers.
//
// The new context is configured based on environment variables, falling back
// to default settings.
CanopyContext canopy_init_context();

// Load settings from configuration file.
//
// The configuration file must be 
CanopyResultEnum canopy_load_opts(CanopyContext context, const char *filename);

// Dump the current settings affecting <context>.  The destination is
// implementation specific (typically stdout is used).
void canopy_debug_dump_opts(CanopyContext context);

// Shutdown a libcanopy context.
//
// Call this at the end of your program to free resources used by libcanopy.
CanopyResultEnum canopy_shutdown_context(CanopyContext ctx);

// Set a global configuration option.
//
// Takes an even number of arguments.  The arguments must alternate between
// option identifiers and values.  The option pairs may appear in any
// order, but each option may be provided at most once.
//
// The options that can be set are:
//
//  CANOPY_LOG_ENABLED
//  CANOPY_LOG_FILE
//  CANOPY_LOG_LEVEL
//  CANOPY_LOG_PAYLOADS
//
// At least one option pair must be provided or a compilation error will occur.
#define canopy_set_global_opt(option, ...) \
    canopy_set_global_opt_impl(NULL, option, __VA_ARGS__, NULL)
CanopyResultEnum canopy_set_global_opt_impl(void *dummy, ...);


// Set a context-wide option.
//
// Takes an odd number of arguments.  After the first argument (<ctx>), the
// arguments must must alternate between option identifiers and values.  The
// option pars may appear in any order, but each option may be provided at most
// once.
//
// The options that can be set are:
//
// CANOPY_CLOUD_SERVER
//
//     Configures the hostname and port of the Canopy Cloud Service to interact
//     with.  The value must be a string, such as "canopy.link" or
//     "localhost:8080".  Defaults to "canopy.link".
//
// CANOPY_DEVICE_UUID
//
//     Configures the UUID of the current device.  The value must be a string
//     containing a type-4 UUID, such as
//     "16eeca6a-e8dc-4c54-b78e-6a7416803ca8", or NULL if unconfigured.
//     Defaults to nULL.
//
// CANOPY_VAR_SEND_PROTOCOL
//
//     Configures the protocol to use for reporting Cloud Variable changes to
//     the Canopy Cloud Service.  The value must be a CanopyProtocolEnum value.
//     Currently, the following protocols are supported:
//
//          CANOPY_PROTOCOL_NOOP
//          CANOPY_PROTOCOL_HTTP
//          CANOPY_PROTOCOL_WS
//
//      Defaults to CANOPY_PROTOCOL_HTTP
//
// CANOPY_VAR_RECV_PROTOCOL
//
//     Configures the protocol to use for receiving Cloud Variable changes from
//     the Canopy Cloud Service.  The value must be a CanopyProtocolEnum value.
//     Currently, the following protocols are supported:
//
//          CANOPY_PROTOCOL_NOOP
//          CANOPY_PROTOCOL_WS
//
//      Defaults to CANOPY_PROTOCOL_WS
//
// For example:
//
//      canopy_set_opt(ctx);
//      // does nothing.
//
//      canopy_set_opt(ctx,
//          CANOPY_CLOUD_SERVER, "localhost:8080",
//          CANOPY_DEVICE_UUID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
//          CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_NOOP);
//      // sets several options.
#define canopy_set_opt(ctx, option, ...) \
    canopy_set_opt_impl(ctx, option, __VA_ARGS__, NULL)
CanopyResultEnum canopy_set_opt_impl(CanopyContext ctx, ...);

// Initialize a Cloud Variable
//
// Cloud Variables must be initialized before they are used.
// <decl> is a string containing the datatype, variable name, and qualifiers.
//
// For example:
//
//      canopy_var_init(ctx, "out float32 temperature");
// 
// Additional options can be provided in (key, value) pairs following <decl>:
//
//      canopy_var_init(ctx, "out float32 humidity",
//          CANOPY_VAR_DESCRIPTION, "The current humidity level.",
//          CANOPY_VAR_MIN_VALUE, 0.0,
//          CANOPY_VAR_MAX_VALUE, 0.0,
//      );
//
// A fixed-length array can be initialized using:
//
//      canopy_var_init(ctx, "out float32 cpu_level[8]");
//
// A named tuple can be initialized using:
//
//      canopy_var_init(ctx, "in tuple desired_color",
//          CANOPY_INIT_CHILD("float32 r"),
//          CANOPY_INIT_CHILD("float32 g"),
//          CANOPY_INIT_CHILD("float32 b")
//      );
//
// A struct can be initialized using:
//
//      canopy_var_init(ctx, "struct status",
//          CANOPY_INIT_CHILD("out string msg"),
//          CANOPY_INIT_CHILD("out int16 status_code"),
//          CANOPY_INIT_CHILD("in void update_trigger"),
//      );
#define canopy_var_init(ctx, ...) \
    canopy_var_init_impl(ctx, __VA_ARGS__, NULL)
CanopyResultEnum canopy_var_init_impl(CanopyContext ctx, const char *decl, ...);

// Create a new CanopyVarValue object from a bool.
CanopyVarValue CANOPY_VALUE_BOOL(bool x);

// Create a new CanopyVarValue object from an 8-bit signed integer.
CanopyVarValue CANOPY_VALUE_INT8(int8_t x);

// Create a new CanopyVarValue object from an 8-bit unsigned integer.
CanopyVarValue CANOPY_VALUE_UINT8(uint8_t x);

// Create a new CanopyVarValue object from a 16-bit signed integer.
CanopyVarValue CANOPY_VALUE_INT16(int16_t x);

// Create a new CanopyVarValue object from a 16-bit unsigned integer.
CanopyVarValue CANOPY_VALUE_UINT16(uint16_t x);

// Create a new CanopyVarValue object from a 32-bit signed integer.
CanopyVarValue CANOPY_VALUE_INT32(int32_t x);

// Create a new CanopyVarValue object from a 32-bit unsigned integer.
CanopyVarValue CANOPY_VALUE_UINT32(uint32_t x);

// Create a new CanopyVarValue object from a 32-bit float.
CanopyVarValue CANOPY_VALUE_FLOAT32(float x);

// Create a new CanopyVarValue object from a 64-bit float.
CanopyVarValue CANOPY_VALUE_FLOAT64(double x);

// Create a new CanopyVarValue object from a string.
CanopyVarValue CANOPY_VALUE_STRING(const char *sz);

// Create a new CanopyVarValue object containing a structure.
CanopyVarValue CANOPY_VALUE_STRUCT(void * dummy, ...);

// Create a new CanopyVarValue object containing an array.
// ex:
//      <idx>, <value>
//
//      CANOPY_VALUE_ARRAY(
//          0, CANOPY_VALUE_INT8(40),
//          1, CANOPY_VALUE_INT8(-10),
//          7, CANOPY_VALUE_INT8(34)
//      )
//
#define CANOPY_VALUE_ARRAY(...) CANOPY_VALUE_ARRAY_IMPL(NULL, __VA_ARGS__, -1)
CanopyVarValue CANOPY_VALUE_ARRAY_IMPL(void * dummy, ...);

// Set the local value of a Cloud Variable.
//
// Locally creates a Cloud Variable named <varname> if it doesn't already
// exist.  The newly-created Cloud Variable is given the dataype of the
// supplied value, and is configured with default settings.
//
// Otherwise, if a Cloud Variable named <varname> already exists locally, then
// the supplied value's datatype must match the datatype of the Cloud Variable.
//
// The next time canopy_sync(...) is called, the Cloud Variable's value will be
// reported to the cloud server.  If the Cloud Variable doesn't exist on the
// cloud server, it will be created at this point.
//
// Examples:
//
//      canopy_var_set(ctx, "temperature", CANOPY_FLOAT32(43.0f));
//
//      canopy_var_set(ctx, 
//          "gps", CANOPY_STRUCT(
//              "latitude", CANOPY_FLOAT32(-381.41983),
//              "longitude", CANOPY_FLOAT32(74.4243)
//          )
//      );
//
//      canopy_var_set(ctx, "outlet[6].amperage", CANOPY_FLOAT64(19.383984));
//
//      // Don't forget to sync!
//      canopy_sync(ctx, CANOPY_SYNC_BLOCKING, true);
//
CanopyResultEnum canopy_var_set(CanopyContext ctx, const char *varname, CanopyVarValue value);
#define canopy_var_set_bool(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_BOOL(value))
#define canopy_var_set_float32(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_FLOAT32(value))
#define canopy_var_set_float64(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_FLOAT64(value))
#define canopy_var_set_int8(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_INT8(value))
#define canopy_var_set_int16(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_INT16(value))
#define canopy_var_set_int32(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_INT32(value))
#define canopy_var_set_string(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_STRING(value))
#define canopy_var_set_uint8(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_UINT8(value))
#define canopy_var_set_uint16(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_UINT16(value))
#define canopy_var_set_uint32(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_VALUE_UINT32(value))

// Create a new CanopyVarReader object that reads into a 32-bit float.
CanopyVarReader CANOPY_READ_FLOAT32(float *x);

// Create a new CanopyVarReader object that reads into a string.
CanopyVarReader CANOPY_READ_STRING(const char **sz);

// Create a new CanopyVarReaader object that reads multiple structure fields.
CanopyVarReader CANOPY_READ_STRUCT(void * dummy, ...);

// Get the local value of a Cloud Variable.
//
// Examples:
//
//      // sync first to get latest values from cloud.
//      canopy_sync(ctx, CANOPY_SYNC_BLOCKING, true);
//      
//      canopy_var_get_float32(ctx, "temperature", &temprature);
//
//      canopy_var_get(ctx, 
//          "gps", CANOPY_READ_STRUCT(
//              "latitude", CANOPY_READ_FLOAT32(&latitude),
//              "longitude", CANOPY_READ_FLOAT32(&longitude)
//          )
//      );
//
//      canopy_var_get(ctx, "outlet[6].amperage", CANOPY_READ_FLOAT64(&amps));
//
//      canopy_var_get_float64(ctx, "outlet[6].amperage", &amps);
// 
CanopyResultEnum canopy_var_get(CanopyContext ctx, const char *varname, CanopyVarReader dest);
#define canopy_var_get_bool(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_BOOL(outValue))
#define canopy_var_get_float32(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_FLOAT32(outValue))
#define canopy_var_get_float64(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_FLOAT64(outValue))
#define canopy_var_get_int8(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT8(outValue))
#define canopy_var_get_int16(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT16(outValue))
#define canopy_var_get_int32(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT32(outValue))
#define canopy_var_get_string(ctx,varname,  outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_STRING(outValue))
#define canopy_var_get_uint8(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_UINT8(outValue))
#define canopy_var_get_uint16(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_UINT16(outValue))
#define canopy_var_get_uint32(ctx, varname, outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_UINT32(outValue))


// Register a callback that triggers when a Cloud Variable changes.
//
// static int handle_temperature(CanopyContext ctx, const char *varname, void *userdata)
// {
//     float32 temperature;
//     canopy_var_get_float32(ctx, varname, &temperature);
//     return 0;
// }
//
// canopy_var_on_change(ctx, "temperature", handle_temperature, NULL);
//
// - Creates local variable if it doesn't already exist.
//
CanopyResultEnum canopy_var_on_change(CanopyContext ctx, const char *varname, CanopyOnChangeCallback cb, void *userdata);

// Synchronize with the cloud server.
//
// Updates the local and remote copies of each Cloud Variable with the latest
// values.
CanopyResultEnum canopy_sync(CanopyContext ctx, CanopyPromise promise);

#endif // CANOPY_INCLUDED
