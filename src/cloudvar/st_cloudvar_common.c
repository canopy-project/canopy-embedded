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

// Parse options passed to canopy_var_init() into STCloudVarInitOptions_t
// structure.
//
// Sets <*out> to newly-allocated STCloudVarInitOptions_t structure.
CanopyResultEnum st_cloudvar_parse_init_options(
        STCloudVarInitOptions *out,
        const char *declString, 
        va_list ap)
{

    SDDLDirectionEnum direction;
    SDDLDatatypeEnum datatype;
    char *name;
    SDDLDatatypeEnum arrayElementDatatype;
    size_t arraySize;
    SDDLResultEnum sddlResult;
    STCloudVarInitOptions_t *options;
    CanopyVarConfigEnum param;

    // Parse decl string (ex: "inout float32 humidity"):
    sddlResult = sddl_parse_decl(declString, &direction, &datatype, &name, &arrayElementDatatype, &arraySize);
    if (sddlResult != SDDL_SUCCESS)
    {
        return CANOPY_ERROR_BAD_VARIABLE_DECLARATION;
    }

    // Allocate output structure
    options = calloc(1, sizeof(STCloudVarInitOptions_t));
    if (!options)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    options->direction = direction;
    options->datatype = datatype;
    options->array_num_items = arraySize;
    options->array_datatype = arrayElementDatatype;
    options->name = RedString_strdup(name);

    if (datatype == SDDL_DATATYPE_STRUCT)
    {
        options->struct_hash = RedHash_New(0);
        if (!options->struct_hash)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }
    }

    // process varargs
    while ((param = va_arg(ap, CanopyVarConfigEnum)) != 0)
    {
        switch (param)
        {
            case CANOPY_VAR_FIELD:
            {
                if (datatype != SDDL_DATATYPE_STRUCT)
                {
                    return CANOPY_ERROR_INVALID_OPT;
                }

                CanopyVarInitObject childObj = va_arg(ap, CanopyVarInitObject);
                if (!childObj)
                {
                    return CANOPY_ERROR_INVALID_VALUE;
                }

                RedHash_InsertS(options->struct_hash, childObj->options->name, childObj->options);
                break;
            }
            case CANOPY_VAR_DESCRIPTION:
            {
                char *description = va_arg(ap, char *);
                options->description = RedString_strdup(description);
                break;
            }
            default:
            {
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }

    *out = options;
    return CANOPY_SUCCESS;
}


// Recursive routine for creating a Cloud Variable instance.
CanopyResultEnum st_cloudvar_generic_new(
        STCloudVar *out,
        STCloudVarInitOptions options)
{
    if (sddl_datatype_is_basic(options->datatype))
    {
        return st_cloudvar_basic_new(out, options);
    }
    else if (options->datatype == SDDL_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_new(out, options);
    }
    else if (options->datatype == SDDL_DATATYPE_STRUCT)
    {
        return st_cloudvar_struct_new(out, options);
    }
    return CANOPY_ERROR_UNKNOWN;
}

CanopyResultEnum st_cloudvar_init_var(STCloudVarSystem sys, const char *decl, va_list ap)
{
    STCloudVarInitOptions options;
    STCloudVar var;
    CanopyResultEnum result;

    // Parse <decl> and <ap>
    result = st_cloudvar_parse_init_options(&options, decl, ap);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    // Error if variable has already been initialized.
    var = st_cloudvar_system_lookup_var(sys, options->name);
    if (var)
    {
        return CANOPY_ERROR_VARIABLE_ALREADY_INITIALIZED;
    }

    // Create new top-level cloud variable and children.
    result = st_cloudvar_generic_new(&var, options);
    if (result != CANOPY_SUCCESS)
    {
        return result;
    }

    // Add it to the system
    RedHash_InsertS(sys->vars, options->name, var);
    st_cloudvar_system_mark_dirty(sys, var);
    var->sddl_dirty_flag = true;
    var->sys = sys;

    return CANOPY_SUCCESS;
}

// recursive
CanopyResultEnum st_cloudvar_generic_set(STCloudVar var, CanopyVarValue value)
{
    // Call appropriate set routine
    if (st_cloudvar_is_basic(var))
    {
        return st_cloudvar_basic_set(var, value);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_set(var, value);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_STRUCT)
    {
        return st_cloudvar_struct_set(var, value);
    }

   return CANOPY_ERROR_UNKNOWN;
}


CanopyResultEnum st_cloudvar_set_var(
        STCloudVar var, 
        CanopyVarValue value)
{
    // non-recursive part
    if (st_cloudvar_concrete_direction(var) == CANOPY_DIRECTION_IN)
    {
        return CANOPY_ERROR_CANNOT_MODIFY_INPUT_VARIABLE;
    }

    st_cloudvar_system_mark_dirty(var->sys, var);

    // recursive part
    return st_cloudvar_generic_set(var, value);
}

void st_cloudvar_clear_sddl_dirty_flag(STCloudVar var)
{
    var->sddl_dirty_flag = false;
}

bool st_cloudvar_is_sddl_dirty(STCloudVar var)
{
    return var->sddl_dirty_flag;
}

// Convert cloud variable's value to JSON, recursively
CanopyResultEnum st_cloudvar_value_to_json(RedJsonValue *out, STCloudVar var)
{
    // Call appropriate value_to_json routine
    if (st_cloudvar_is_basic(var))
    {
        return st_cloudvar_basic_value_to_json(out, var);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_value_to_json(out, var);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_STRUCT)
    {
        return st_cloudvar_struct_value_to_json(out, var);
    }

   return CANOPY_ERROR_UNKNOWN;
}

// Read cloud variable's value recursively, using CanopyVarReader
CanopyResultEnum st_cloudvar_read_var(STCloudVar var, CanopyVarReader reader)
{
    // Call appropriate value_to_json routine
    if (st_cloudvar_is_basic(var))
    {
        return st_cloudvar_basic_read_var(var, reader);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_read_var(var, reader);
    }
    else if (st_cloudvar_datatype(var) == CANOPY_DATATYPE_STRUCT)
    {
        return st_cloudvar_struct_read_var(var, reader);
    }

   return CANOPY_ERROR_UNKNOWN;
}

CanopyVarInitObject st_cloudvar_init_field(const char *decl, va_list ap)
{
    CanopyVarInitObject out;
    CanopyResultEnum result;
    out = calloc(1, sizeof(STCloudVarInitObject_t));
    if (!out)
    {
        return NULL;
    }
    result = st_cloudvar_parse_init_options(&out->options, decl, ap);
    if (result != CANOPY_SUCCESS)
    {
        return NULL;
    }
    return out;
}
