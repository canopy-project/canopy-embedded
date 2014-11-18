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


// Create a new array cloud variable instance.
// Caller is responsible for setting up relationships to parent & cloudvar
// system.
CanopyResultEnum st_cloudvar_array_new(
        STCloudVar *out, 
        size_t numItems,
        SDDLDatatypeEnum childDatatype, 
        SDDLDirectionEnum direction,
        STInitOptions options)
{
    STCloudVar var;

    // Create STCloudVar object for array itself
    var = calloc(1, sizeof(STCloudVar_t));
    if (!var)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    // TODO: validate numItems
    var->array_num_items = numItems;

    // Create SDDL declaration
    var->decl = sddl_var_new_array(childDatatype, direction, options);
    if (!var->decl)
    {
        return CANOPY_ERROR_UNKOWN;
    }

    // Create child STCloudVar objects for each array element
    var->array_items = calloc(numItems, sizeof(STCloudVar));
    for (i = 0; i < numItems; i++)
    {
        var->array_items[i] = st_cloudvar_common_new(options);
        var->array_items[i]->parent = var;
    }

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_basic_validate_value(STCloudVar var, CanopyVarValue value)
{
    // Do datatypes match?
    if (sddl_var_datatype(decl) != value->datatype)
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

    var->value = value;
    /* TODO: copy? Delete old value? */

    _mark_dirty(var->sys, var->name);

    return CANOPY_SUCCESS;
}
