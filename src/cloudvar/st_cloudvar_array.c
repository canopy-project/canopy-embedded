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

        // Fake an options object for the array elements.
        // TODO: This doesn't seem like the cleanest way to handle array
        // elements.
        STCloudVarInitOptions_t childOptions;
        memcpy(&childOptions, options, sizeof(STCloudVarInitOptions_t));
        childOptions.datatype = childOptions.array_datatype;

        result = st_cloudvar_generic_new(&(var->array_items[i]), &childOptions); 

        if (result != CANOPY_SUCCESS)
        {
            return result;
        }
    }

    *out = var;
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_array_validate_value(STCloudVar var, CanopyVarValue value)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Sets an array cloud variable's value
CanopyResultEnum st_cloudvar_array_set(STCloudVar var, CanopyVarValue value)
{
    CanopyResultEnum result;
    assert(st_cloudvar_datatype(var) == CANOPY_DATATYPE_ARRAY);

    // Is variable writeable?
    if (st_cloudvar_concrete_direction(var) == CANOPY_DIRECTION_IN)
    {
        return CANOPY_ERROR_CANNOT_MODIFY_INPUT_VARIABLE;
    }

    // TODO: Validate value before assignment
    /*result = st_cloudvar_array_validate_value(var, value);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }*/

    // Assign value recursively
    // Loop over each value
    RedHashIterator_t iter;
    const void *key;
    const void *hashValue;
    size_t keySize;

    RED_HASH_FOREACH(iter, value->array_hash, &key, &keySize, &hashValue)
    {
        int idx = *((int *)key);
        CanopyVarValue elementValue = (CanopyVarValue)hashValue;

        // Check index bounds
        if (idx < 0)
        {
            return CANOPY_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS;
        }
        if (idx >= var->array_num_items)
        {
            return CANOPY_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS;
        }

        // Assign value
        result = st_cloudvar_generic_set(var->array_items[idx], elementValue);
        if (result != CANOPY_SUCCESS)
        {
            return result;
        }
    }

    return CANOPY_SUCCESS;
}
