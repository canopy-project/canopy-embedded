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

// A CanopyVar is a "Cloud Variable".
typedef struct CanopyVar_t * CanopyVar;

// A CanopyVarValue represents a value that can be assigned to Cloud Variable.
typedef struct CanopyVarValue_t * CanopyVarValue;

// Initialize libcanopy and create a context.  
//
// This may be called multiple times to create multiple contexts, which may be
// useful for unit testing, or if you need to talk to multiple canopy servers.
CanopyContext canopy_init_context();

// Destroy a libcanopy context.
//
// Call this at the end of your program to free resources used by libcanopy.
CanopyResultEnum canopy_destroy_context(CanopyContext ctx);

// Set a context-wide option.
//
// Takes an odd number of arguments.  After the first argument (<ctx>), the
// arguments must must alternate between option identifiers and values.  The
// option identifies may appear in any order, but each option may be provided
// at most once.
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
CanopyResultEnum canopy_set_opt(CanopyContext ctx, CanopyOptEnum option, ...);

// Create a new CanopyVarValue object from a 32-bit float.
CanopyVarValue CANOPY_FLOAT32(float x);

// Create a new CanopyVarValue object from a string.
CanopyVarValue CANOPY_STRING(const char *sz);

// Create a new CanopyVarValue object containing a structure.
CanopyVarValue CANOPY_STRUCT(void * dummy, ...);

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
#define canopy_var_set_bool(ctx. varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_BOOL(value))
#define canopy_var_set_float32(ctx, varname, value) \
    canopy_var_set((ctx), (varname), CANOPY_FLOAT32(value))
#define canopy_var_set_float64(ctx, varname, double value) \
    canopy_var_set((ctx), (varname), CANOPY_FLOAT64(value))
#define canopy_var_set_int8(ctx, varname, int8_t value) \
    canopy_var_set((ctx), (varname), CANOPY_INT8(value))
#define canopy_var_set_int16(ctx, varname, int16_t value) \
    canopy_var_set((ctx), (varname), CANOPY_INT16(value))
#define canopy_var_set_int32(ctx, varname, int32_t value) \
    canopy_var_set((ctx), (varname), CANOPY_INT32(value))
#define canopy_var_set_string(ctx, varname, const char * value) \
    canopy_var_set((ctx), (varname), CANOPY_STRING(value))
#define canopy_var_set_uint8(ctx, varname, uint8_t value) \
    canopy_var_set((ctx), (varname), CANOPY_UINT8(value))
#define canopy_var_set_uint16(ctx, varname, uint16_t value) \
    canopy_var_set((ctx), (varname), CANOPY_UINT16(value))
#define canopy_var_set_uint32(ctx, varname, uint32_t value) \
    canopy_var_set((ctx), (varname), CANOPY_UINT32(value))

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
#define canopy_var_get_bool(ctx, const char *varname, bool *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_BOOL(outValue))
#define canopy_var_get_float32(ctx, const char *varname, float *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_FLOAT32(outValue))
#define canopy_var_get_float64(ctx, const char *varname, double *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_FLOAT64(outValue))
#define canopy_var_get_int8(ctx, const char *varname, int8_t *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT8(outValue))
#define canopy_var_get_int16(ctx, const char *varname, int16_t *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT16(outValue))
#define canopy_var_get_int32(ctx, const char *varname, int32_t *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_INT32(outValue))
#define canopy_var_get_string(ctx, const char *varname, const char * *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_STRING(outValue))
#define canopy_var_get_uint8(ctx, const char *varname, uint8_t *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_UINT8(outValue))
#define canopy_var_get_uint16(ctx, const char *varname, uint16_t *outValue) \
    canopy_var_get((ctx), (varname), CANOPY_READ_UINT16(outValue))
#define canopy_var_get_uint32(ctx, const char *varname, uint32_t *outValue) \
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
CanopyResultEnum canopy_var_on_change(CanopyContext ctx, const char *varname, CanopyOnChangeCallback cb, void *userdata);

// Configure a Cloud Variable.
//
// Cloud variables have a bunch of metadata and configuration options that can
// be modified at any time.
CanopyResultEnum canopy_var_config(CanopyContext ctx, const char *varname, ...)

// Synchronize with the cloud server.
//
// Updates the local and remote copies of each Cloud Variable with the latest
// values.
CanopyResultEnum canopy_sync(CanopyContext ctx, CanopyPromise &promise);


#endif
