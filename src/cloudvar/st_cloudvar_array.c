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

// Create a new array cloud variable instance.
// Caller is responsible for setting up relationships to parent & cloudvar
// system.
CanopyResultEnum st_cloudvar_array_new(
        STCloudVar *out, 
        STCloudVarInitOptions options)
{
    STCloudVar var;
    size_t i;

    // Create STCloudVar object for array itself
    var = calloc(1, sizeof(STCloudVar_t));
    if (!var)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    // TODO: validate numItems
    var->array_num_items = options->array_num_items;

    // Create SDDL declaration
    var->decl = sddl_var_new_array(
            options->array_datatype, 
            options->array_num_items, 
            options->direction, 
            options->name);
    if (!var->decl)
    {
        return CANOPY_ERROR_UNKNOWN;
    }

    // Create child STCloudVar objects for each array element
    var->array_items = calloc(var->array_num_items, sizeof(STCloudVar));
    for (i = 0; i < var->array_num_items; i++)
    {
        CanopyResultEnum result;

        result = st_cloudvar_generic_new(&(var->array_items[i]), options); 
        // TODO: This above line is probably wrong.
        // We need to create a new options object for the children.  Yes?
        if (result != CANOPY_SUCCESS)
        {
            return result;
        }
    }

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_array_validate_value(STCloudVar var, CanopyVarValue value)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Sets a basic cloud variable's value
CanopyResultEnum st_cloudvar_array_set(STCloudVar var, CanopyVarValue value)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}
