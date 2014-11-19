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

#ifndef ST_VARS_INCLUDED
#define ST_VARS_INCLUDED

#include <canopy.h>
#include <stdbool.h>
#include "options/st_options.h"
#include <red_json.h>

typedef struct STCloudVar_t * STCloudVar;
typedef struct STCloudVarSystem_t * STCloudVarSystem;
typedef struct STCloudVarInitOptions_t * STCloudVarInitOptions;

// Create/initialize a new Cloud Var "system" which holds several cloud
// variables.
STCloudVarSystem st_cloudvar_system_new(CanopyContext ctx);

// Shutdown Cloud Var "system"
void st_cloudvar_system_free(STCloudVarSystem sys);

// Does a local Cloud Variable exist?
bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname);

// Lookup a local Cloud Variable by name
STCloudVar st_cloudvar_system_lookup_var(STCloudVarSystem sys, const char *varname);

// Have any Cloud Variables been touched since the last call to
// st_cloudvar_system_clear_dirty?
bool st_cloudvar_system_is_dirty(STCloudVarSystem sys);

// Clear system's dirty flag
void st_cloudvar_system_clear_dirty(STCloudVarSystem sys);

// Get number of dirty Cloud Variables
uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys);

// Access a particular dirty Cloud Variable by index.
STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx);

// Register a callback that gets triggered when a cloud variable's value
// changes.
// TODO: What if multiple callbakcs are registered for single variable?
CanopyResultEnum st_cloudvar_register_on_change_callback(STCloudVar var, CanopyOnChangeCallback cb, void *userdata);

// Sets Cloud Variable's value.  Consumes <value> (meaning <value> should never
// be used again)
CanopyResultEnum st_cloudvar_set_var(STCloudVar var, CanopyVarValue value);

// Get Cloud Variable's value using reader.
CanopyResultEnum st_cloudvar_read_var(STCloudVar var, CanopyVarReader dest);

CanopyResultEnum st_cloudvar_set_local_value_from_json(STCloudVarSystem vars, const char *varname, RedJsonValue value);

CanopyResultEnum st_cloudvar_value_to_json(RedJsonValue *out, STCloudVar var);

CanopyVarValue st_cloudvar_value_bool(bool x);
CanopyVarValue st_cloudvar_value_int8(int8_t x);
CanopyVarValue st_cloudvar_value_uint8(uint8_t x);
CanopyVarValue st_cloudvar_value_int16(int16_t x);
CanopyVarValue st_cloudvar_value_uint16(uint16_t x);
CanopyVarValue st_cloudvar_value_int32(int32_t x);
CanopyVarValue st_cloudvar_value_uint32(uint32_t x);
CanopyVarValue st_cloudvar_value_float32(float x);
CanopyVarValue st_cloudvar_value_float64(double x);
CanopyVarValue st_cloudvar_value_string(const char *sz);
CanopyVarValue st_cloudvar_value_struct(va_list ap);
CanopyVarValue st_cloudvar_value_array(va_list ap);

void st_cloudvar_value_free(CanopyVarValue value);

CanopyVarReader st_cloudvar_reader_bool(bool *dest);
CanopyVarReader st_cloudvar_reader_int8(int8_t *dest);
CanopyVarReader st_cloudvar_reader_uint8(uint8_t *dest);
CanopyVarReader st_cloudvar_reader_uint16(uint16_t *dest);
CanopyVarReader st_cloudvar_reader_int32(int32_t *dest);
CanopyVarReader st_cloudvar_reader_uint32(uint32_t *dest);
CanopyVarReader st_cloudvar_reader_float32(float *dest);
CanopyVarReader st_cloudvar_reader_float64(double *dest);
CanopyVarReader st_cloudvar_reader_string(char **dest);
CanopyVarReader st_cloudvar_reader_struct(va_list ap);
CanopyVarReader st_cloudvar_reader_array(va_list ap);

void st_cloudvar_reader_free(CanopyVarReader value);

float st_cloudvar_local_value_float32(STCloudVar var);
const char * st_cloudvar_name(STCloudVar var);
bool st_cloudvar_has_value(STCloudVar var);

bool st_cloudvar_value_already_used(CanopyVarValue value);
void st_cloudvar_value_mark_used(CanopyVarValue value);

// Get Cloud Variable's datatype as string
const char * st_cloudvar_datatype_string(STCloudVar var);

// Get Cloud Variable's direction as string
const char * st_cloudvar_direction_string(STCloudVar var);

// Get Cloud Variables' declaration string, i.e. "inout float32 temperature"
const char * st_cloudvar_decl_string(STCloudVar var);

CanopyResultEnum st_cloudvar_config_extend_varargs(STCloudVarSystem sys, const char *varname, va_list ap);

CanopyResultEnum st_cloudvar_init_var(STCloudVarSystem sys, const char *decl, va_list ap);

CanopyDirectionEnum st_cloudvar_direction(STCloudVar var);
CanopyDirectionEnum st_cloudvar_concrete_direction(STCloudVar var);

void st_cloudvar_system_mark_dirty(STCloudVarSystem sys, STCloudVar var);

CanopyDatatypeEnum st_cloudvar_datatype(STCloudVar var);

CanopyResultEnum st_cloudvar_basic_new(
        STCloudVar *out, 
        STCloudVarInitOptions options);

CanopyResultEnum st_cloudvar_array_new(
        STCloudVar *out, 
        STCloudVarInitOptions options);

CanopyResultEnum st_cloudvar_generic_new(
        STCloudVar *out,
        STCloudVarInitOptions options);

CanopyResultEnum st_cloudvar_generic_set(STCloudVar var, CanopyVarValue value);

void st_cloudvar_clear_sddl_dirty_flag(STCloudVar var);
bool st_cloudvar_is_sddl_dirty(STCloudVar var);

CanopyResultEnum st_cloudvar_basic_set(STCloudVar var, CanopyVarValue value);

CanopyResultEnum st_cloudvar_array_set(STCloudVar var, CanopyVarValue value);

bool st_cloudvar_is_basic(STCloudVar var);

CanopyResultEnum st_cloudvar_array_value_to_json(RedJsonValue *out, STCloudVar var);
CanopyResultEnum st_cloudvar_basic_value_to_json(RedJsonValue *out, STCloudVar var);

CanopyResultEnum st_cloudvar_basic_read_var(STCloudVar var, CanopyVarReader reader);
CanopyResultEnum st_cloudvar_array_read_var(STCloudVar var, CanopyVarReader reader);

#endif // ST_VARS_INCLUDED


