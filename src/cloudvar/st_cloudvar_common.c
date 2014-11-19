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
    options->name = RedString_strdup(name);

    // process varargs
    while ((param = va_arg(ap, CanopyVarConfigEnum)) != 0)
    {
        switch (param)
        {
            case CANOPY_VAR_CHILD:
            {
                if (datatype != SDDL_DATATYPE_STRUCT)
                {
                    return CANOPY_ERROR_INVALID_OPT;
                }
                // TODO recursive processing
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
    STCloudVar var;
    if (sddl_datatype_is_basic(options->datatype))
    {
        return st_cloudvar_basic_new(&var, options);
    }
    else if (options->datatype == SDDL_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_new(&var, options);
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

    return CANOPY_SUCCESS;
}

void st_cloudvar_clear_sddl_dirty_flag(STCloudVar var)
{
    var->sddl_dirty_flag = false;
}

bool st_cloudvar_is_sddl_dirty(STCloudVar var)
{
    return var->sddl_dirty_flag;
}
