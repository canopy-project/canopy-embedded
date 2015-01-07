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

// Convert tuple cloud variable's value to JSON, recursively
CanopyResultEnum st_cloudvar_tuple_value_to_json(RedJsonValue *out, STCloudVar var)
{
    RedJsonObject jsonObj = RedJsonObject_New();
    RedHashIterator_t iter;
    const void *key;
    const void *hashValue;
    size_t keySize;
    RED_HASH_FOREACH(iter, var->tuple_hash, &key, &keySize, &hashValue)
    {
        CanopyResultEnum result;
        RedJsonValue childValueJson;
        STCloudVar childVar = (STCloudVar)hashValue;
        if (st_cloudvar_has_value(childVar))
        {
            result = st_cloudvar_value_to_json(&childValueJson, childVar);
            if (result != CANOPY_SUCCESS)
            {
                return result;
            }
            RedJsonObject_Set(jsonObj, key, childValueJson);
        }
    }

    *out = RedJsonValue_FromObject(jsonObj);
    return CANOPY_SUCCESS;
}

// Create a new tuple cloud variable instance.
// Caller is responsible for setting up relationships to parent & cloudvar
// system.
CanopyResultEnum st_cloudvar_tuple_new(
        STCloudVar *out, 
        STCloudVarInitOptions options)
{
    STCloudVar var;

    // Create STCloudVar object for tuple itself
    var = calloc(1, sizeof(STCloudVar_t));
    if (!var)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    // TODO: validate numItems
    var->tuple_hash = RedHash_New(0);
    if (!var->tuple_hash)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    // Create SDDL declaration
    var->decl = sddl_var_new_tuple(options->direction, options->name);
    if (!var->decl)
    {
        return CANOPY_ERROR_UNKNOWN;
    }

    // Create child STCloudVar objects for each tuple member
    RedHashIterator_t iter;
    const void *key;
    const void *hashValue;
    size_t keySize;
    RED_HASH_FOREACH(iter, options->tuple_hash, &key, &keySize, &hashValue)
    {
        CanopyResultEnum result;
        STCloudVar childVar;
        bool ok;
        STCloudVarInitOptions childOptions = (STCloudVarInitOptions)hashValue;

        result = st_cloudvar_generic_new(&childVar, childOptions); 

        if (result != CANOPY_SUCCESS)
        {
            return result;
        }

        // add newly created variable to SDDL
        // TODO: Is this the proper way to hook things up?
        ok = sddl_var_tuple_add_member(var->decl, childVar->decl);
        if (!ok)
        {
            return CANOPY_ERROR_UNKNOWN;
        }

        // add newly created variable to CloudVar
        RedHash_InsertS(var->tuple_hash, st_cloudvar_name(childVar), childVar);

    }

    *out = var;
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_tuple_validate_value(STCloudVar var, CanopyVarValue value)
{
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

// Sets a tuple cloud variable's value
CanopyResultEnum st_cloudvar_tuple_set(STCloudVar var, CanopyVarValue value)
{
    CanopyResultEnum result;
    assert(st_cloudvar_datatype(var) == CANOPY_DATATYPE_TUPLE);

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

    RED_HASH_FOREACH(iter, value->tuple_hash, &key, &keySize, &hashValue)
    {
        char *fieldName = (char *)key;
        CanopyVarValue fieldValue = (CanopyVarValue)hashValue;
        STCloudVar fieldVar;

        fieldVar = RedHash_GetWithDefaultS(var->tuple_hash, fieldName, NULL);
        if (!fieldVar)
        {
            return CANOPY_ERROR_VARIABLE_NOT_INITIALIZED;
        }

        // Assign value
        result = st_cloudvar_generic_set(fieldVar, fieldValue);
        if (result != CANOPY_SUCCESS)
        {
            return result;
        }
    }

    return CANOPY_SUCCESS;
}

// Gets tuple cloud variable's value
CanopyResultEnum st_cloudvar_tuple_read_var(STCloudVar var, CanopyVarReader reader)
{
    CanopyResultEnum result;
    if (reader->datatype != CANOPY_DATATYPE_TUPLE)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }
    if (st_cloudvar_datatype(var) != CANOPY_DATATYPE_TUPLE)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }

    // Read values recursively
    // Loop over each reader
    RedHashIterator_t iter;
    const void *key;
    const void *hashValue;
    size_t keySize;

    RED_HASH_FOREACH(iter, reader->dest.tuple_hash, &key, &keySize, &hashValue)
    {
        char *fieldName = (char *)key;
        CanopyVarReader fieldReader = (CanopyVarReader)hashValue;
        STCloudVar fieldVar;

        fieldVar = RedHash_GetWithDefaultS(var->tuple_hash, fieldName, NULL);
        if (!fieldVar)
        {
            return CANOPY_ERROR_VARIABLE_NOT_INITIALIZED;
        }

        // Assign value
        result = st_cloudvar_read_var(fieldVar, fieldReader);
        if (result != CANOPY_SUCCESS)
        {
            return result;
        }
    }

    return CANOPY_SUCCESS;
}

