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


// Convert basic cloud variable's value to JSON
CanopyResultEnum st_cloudvar_basic_value_to_json(RedJsonValue *out, STCloudVar var)
{
    CanopyDatatypeEnum datatype = st_cloudvar_datatype(var);
    *out = NULL;
    switch (datatype)
    {
        case CANOPY_DATATYPE_VOID:
            *out = RedJsonValue_Null();
            break;
        case CANOPY_DATATYPE_BOOL:
            *out = RedJsonValue_FromBoolean(var->basic_value->val.val_bool);
            break;
        case CANOPY_DATATYPE_FLOAT32:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_float32);
            break;
        case CANOPY_DATATYPE_FLOAT64:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_float64);
            break;
        case CANOPY_DATATYPE_INT8:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_int8);
            break;
        case CANOPY_DATATYPE_INT16:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_int16);
            break;
        case CANOPY_DATATYPE_INT32:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_int32);
            break;
        case CANOPY_DATATYPE_STRING:
            *out = RedJsonValue_FromString(var->basic_value->val.val_string);
            break;
        case CANOPY_DATATYPE_UINT8:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_uint8);
            break;
        case CANOPY_DATATYPE_UINT16:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_uint16);
            break;
        case CANOPY_DATATYPE_UINT32:
            *out = RedJsonValue_FromNumber(var->basic_value->val.val_uint32);
            break;
        default:
            return CANOPY_ERROR_UNKNOWN;
            break;
    }
    if (*out == NULL)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }

    return CANOPY_SUCCESS;
}

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

    *out = var;
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

    // TODO: rethink the dirty flag now that things are recursive
    if (var->sys)
        st_cloudvar_system_mark_dirty(var->sys, var);

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_basic_read_var(STCloudVar var, CanopyVarReader reader)
{
    if (st_cloudvar_datatype(var) != reader->datatype)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }

    if (!st_cloudvar_has_value(var))
    {
        return CANOPY_ERROR_VARIABLE_NOT_SET;
    }

    switch (reader->datatype)
    {
        case CANOPY_DATATYPE_BOOL:
            *reader->dest.dest_bool = var->basic_value->val.val_bool;
            break;
        case CANOPY_DATATYPE_FLOAT32:
            *reader->dest.dest_float32 = var->basic_value->val.val_float32;
            break;
        case CANOPY_DATATYPE_FLOAT64:
            *reader->dest.dest_float64 = var->basic_value->val.val_float64;
            break;
        case CANOPY_DATATYPE_INT8:
            *reader->dest.dest_int8 = var->basic_value->val.val_int8;
            break;
        case CANOPY_DATATYPE_INT16:
            *reader->dest.dest_int16 = var->basic_value->val.val_int16;
            break;
        case CANOPY_DATATYPE_INT32:
            *reader->dest.dest_int32 = var->basic_value->val.val_int32;
            break;
        case CANOPY_DATATYPE_STRING:
            *reader->dest.dest_string = RedString_strdup(var->basic_value->val.val_string);
            break;
        case CANOPY_DATATYPE_UINT8:
            *reader->dest.dest_uint8 = var->basic_value->val.val_uint8;
            break;
        case CANOPY_DATATYPE_UINT16:
            *reader->dest.dest_uint16 = var->basic_value->val.val_uint16;
            break;
        case CANOPY_DATATYPE_UINT32:
            *reader->dest.dest_uint32 = var->basic_value->val.val_uint32;
            break;
        default:
            return CANOPY_ERROR_UNKNOWN;
            break;
    }
    return CANOPY_SUCCESS;
}

