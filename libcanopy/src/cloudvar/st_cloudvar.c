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

#include "cloudvar/st_cloudvar.h"
#include "red_hash.h"
#include <sddl.h>

struct STCloudVarSystem_t {
    bool dirty;
    RedHash vars; // maps (char *varname) -> (STCloudVar var)
};

struct STCloudVarValue_t {
    SDDLDatatypeEnum datatype;
    union
    {
        bool val_bool;
        char * val_string;
        int8_t val_int8;
        uint8_t val_uint8;
        int16_t val_int16;
        uint16_t val_uint16;
        int32_t val_int32;
        uint32_t val_uint32;
        float val_float32;
        double val_float64;
        struct tm val_datetime;
    } val;
};

struct STCloudVar_t {
    STCloudVarSystem sys;
    bool dirty;
    char *name;
    struct STCloudVarValue_t value;
};

STCloudVarSystem st_cloudvar_system_new()
{
    STCloudVarSystem sys;

    sys = calloc(1, sizeof(struct STCloudVarSystem_t));
    sys->dirty = true;
    return sys;
}

bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname)
{
    return RedHash_HasKeyS(sys->vars, varname);
}

CanopyResultEnum st_cloudvar_system_add(STCloudVarSystem sys, const char *varname)
{
    STCloudVar var;
    var = calloc(1, sizeof(struct STCloudVar_t));

    var->sys = sys;

    RedHash_InsertS(sys->vars, varname, var);
    return CANOPY_SUCCESS;
}

STCloudVar st_cloudvar_system_get_var(STCloudVarSystem sys, const char *varname)
{
    return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
}

CanopyResultEnum st_cloudvar_set_local_value_float32(STCloudVar var, float value)
{
    var->value.val.val_float32 = value;
    var->dirty = true;
    var->sys->dirty = true;
    return CANOPY_SUCCESS;
}
