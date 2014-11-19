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
#include "cloudvar/st_cloudvar_internal.h"
#include "red_string.h"
#include <assert.h>

// Create a new basic cloud variable instance.
// Caller is responsible for setting up relationships to parent & cloudvar
// system.
CanopyResultEnum st_cloudvar_basic_new(
        STCloudVar *out, 
        STCloudVarInitOptions options)
{
    STCloudVar var;

    // Create STCloudVar object
    var = calloc(1, sizeof(STCloudVar_t));
    if (!var)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    // Create SDDL declaration
    char *name = RedString_strdup(options->name);
    var->decl = sddl_var_new_basic(options->datatype, options->direction, name);
    if (!var->decl)
    {
        return CANOPY_ERROR_UNKNOWN;
    }

    // TODO: other properties

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_basic_validate_value(STCloudVar var, CanopyVarValue value)
{
    // Do datatypes match?
    if (sddl_var_datatype(var->decl) != (SDDLDatatypeEnum)value->datatype)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }

    // TODO: other validation
    return CANOPY_SUCCESS;
}

// Sets a basic cloud variable's value
CanopyResultEnum st_cloudvar_basic_set(STCloudVar var, CanopyVarValue value)
{
    CanopyResultEnum result;

    assert(sddl_var_is_basic(var->decl));

    // Is variable writeable?
    if (st_cloudvar_concrete_direction(var) == CANOPY_DIRECTION_IN)
    {
        return CANOPY_ERROR_CANNOT_MODIFY_INPUT_VARIABLE;
    }

    result = st_cloudvar_basic_validate_value(var, value);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    // Copy value
    var->basic_value = calloc(1, sizeof(STCloudVarBasicValue_t));
    if (!var->basic_value)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    memcpy(var->basic_value, &value->basic_value, sizeof(STCloudVarBasicValue_t));

    st_cloudvar_system_mark_dirty(var->sys, var);

    return CANOPY_SUCCESS;
}
