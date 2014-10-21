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

typedef struct STCloudVar_t * STCloudVar;
typedef struct STCloudVarSystem_t * STCloudVarSystem;

STCloudVarSystem st_cloudvar_system_new();

STCloudVar st_cloudvar_new();
bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname);
CanopyResultEnum st_cloudvar_system_add(STCloudVarSystem sys, const char *varname);
bool st_cloudvar_system_is_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_get_var(STCloudVarSystem sys, const char *varname);

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys);
STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx);

CanopyResultEnum st_cloudvar_set_local_value_float32(STCloudVar var, float value);
float st_cloudvar_local_value_float32(STCloudVar var);
const char * st_cloudvar_name(STCloudVar var);

#endif // ST_VARS_INCLUDED


