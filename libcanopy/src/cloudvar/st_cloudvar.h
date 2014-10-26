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

typedef struct STCloudVar_t * STCloudVar;
typedef struct STCloudVarSystem_t * STCloudVarSystem;

STCloudVarSystem st_cloudvar_system_new();

STCloudVar st_cloudvar_new();
bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname);
CanopyResultEnum st_cloudvar_system_add(STCloudVarSystem sys, const char *varname);
bool st_cloudvar_system_is_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_get_var(STCloudVarSystem sys, const char *varname);
void st_cloudvar_system_clear_dirty(STCloudVarSystem sys);
void st_cloudvar_system_free(STCloudVarSystem sys);

// TODO: What if multiple callbakcs are registered for single variable?
CanopyResultEnum st_cloudvar_register_on_change_callback(STCloudVar var, STOptions options);

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx);

CanopyResultEnum st_cloudvar_set_local_value_bool(STCloudVar var, bool value);
CanopyResultEnum st_cloudvar_set_local_value_int8(STCloudVar var, int8_t value);
CanopyResultEnum st_cloudvar_set_local_value_uint8(STCloudVar var, uint8_t value);
CanopyResultEnum st_cloudvar_set_local_value_int16(STCloudVar var, int16_t value);
CanopyResultEnum st_cloudvar_set_local_value_uint16(STCloudVar var, uint16_t value);
CanopyResultEnum st_cloudvar_set_local_value_int32(STCloudVar var, int32_t value);
CanopyResultEnum st_cloudvar_set_local_value_uint32(STCloudVar var, uint32_t value);
CanopyResultEnum st_cloudvar_set_local_value_uint32(STCloudVar var, uint32_t value);
CanopyResultEnum st_cloudvar_set_local_value_float32(STCloudVar var, float value);

float st_cloudvar_local_value_float32(STCloudVar var);
const char * st_cloudvar_name(STCloudVar var);

#endif // ST_VARS_INCLUDED


