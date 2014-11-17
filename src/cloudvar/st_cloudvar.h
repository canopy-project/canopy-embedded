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

STCloudVarSystem st_cloudvar_system_new(CanopyContext ctx);

STCloudVar st_cloudvar_new();
bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname);
CanopyResultEnum st_cloudvar_system_add(STCloudVarSystem sys, const char *varname);
bool st_cloudvar_system_is_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_get_var(STCloudVarSystem sys, const char *varname);
void st_cloudvar_system_clear_dirty(STCloudVarSystem sys);
void st_cloudvar_system_free(STCloudVarSystem sys);

// TODO: What if multiple callbakcs are registered for single variable?
CanopyResultEnum st_cloudvar_register_on_change_callback(STCloudVarSystem sys, const char *varname, CanopyOnChangeCallback cb, void *userdata);

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx);

CanopyResultEnum st_cloudvar_set_local_value(STCloudVarSystem vars, const char *varname, CanopyVarValue value);
CanopyResultEnum st_cloudvar_get_local_value(STCloudVarSystem vars, const char *varname, CanopyVarReader dest);

CanopyResultEnum st_cloudvar_set_local_value_from_json(STCloudVarSystem vars, const char *varname, RedJsonValue value);

RedJsonValue st_cloudvar_value_to_json(STCloudVar var);

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

void st_cloudvar_value_free(CanopyVarValue value);

CanopyVarReader st_cloudvar_reader_float32(float *dest);
CanopyVarReader st_cloudvar_reader_string(const char **dest);
CanopyVarReader st_cloudvar_reader_struct(va_list ap);

void st_cloudvar_reader_free(CanopyVarReader value);

float st_cloudvar_local_value_float32(STCloudVar var);
const char * st_cloudvar_name(STCloudVar var);
bool st_cloudvar_has_value(STCloudVar var);

bool st_cloudvar_value_already_used(CanopyVarValue value);
void st_cloudvar_value_mark_used(CanopyVarValue value);

bool st_cloudvar_is_configured(STCloudVar var);
void st_cloudvar_mark_configured(STCloudVar var);

const char * st_cloudvar_datatype_string(STCloudVar var);
#endif // ST_VARS_INCLUDED


