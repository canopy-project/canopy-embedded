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

// Parse options passed to canopy_var_init() into STCloudVarInitOptions_t
// structure.
//
// Sets <*out> to newly-allocated STCloudVarInitOptions_t structure.
CanopyResultEnum st_cloudvar_parse_init_options(
        STCloudVarInitOptions_t **out,
        char *declString, va_list ap)
{
    // Parse decl string (ex: "inout float32 humidity"):
    sddlResult = sddl_parse_decl(decl, &direction, &datatype, &name, &arrayElementDatatype, &arraySize);
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
    while ((param = va_arg(ap, CanopyVarConfigEnum)) != NULL)
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

    return options;
}


// Recursive routine for creating a Cloud Variable instance.
CanopyResultEnum st_cloudvar_generic_new(
        STCloudVar *out,
        STInitOptions options)
{
    STCloudVar var;
    if (sddl_datatype_is_basic(options.datatype))
    {
        return st_cloudvar_basic_new(&var, options);
    }
    else if (options.datatype == SDDL_DATATYPE_ARRAY)
    {
        return st_cloudvar_array_new(&var, options);
    }
}
