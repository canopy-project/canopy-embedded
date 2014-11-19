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
#include "options/st_options.h"
#include "red_hash.h"
#include "red_string.h"
#include <sddl.h>
#include <assert.h>
#include <time.h>

typedef struct STCloudVarStruct_t
{
    RedHash hash; // string -> CanopyVarValue
} STCloudVarStruct_t;

typedef struct STCloudVarArray_t
{
    RedHash hash; // index -> CanopyVarValue
} STCloudVarArray_t;



CanopyVarValue st_cloudvar_value_bool(bool x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_BOOL;
    out->basic_value.val.val_bool = x;
    return out;
}

CanopyVarValue st_cloudvar_value_float32(float x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT32;
    out->basic_value.val.val_float32 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_float64(double x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT64;
    out->basic_value.val.val_float64 = x;
    return out;
}


CanopyVarValue st_cloudvar_value_int8(int8_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT8;
    out->basic_value.val.val_int8 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_int16(int16_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT16;
    out->basic_value.val.val_int16 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_int32(int32_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT32;
    out->basic_value.val.val_int32 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_string(const char *sz)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRING;
    out->basic_value.val.val_string = RedString_strdup(sz);
    if (!out->basic_value.val.val_string)
    {
        free(out);
        return NULL;
    }
    return out;
}

CanopyVarValue st_cloudvar_value_uint8(uint8_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT8;
    out->basic_value.val.val_uint8 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_uint16(uint16_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT16;
    out->basic_value.val.val_uint16 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_uint32(uint32_t x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT32;
    out->basic_value.val.val_uint32 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_struct(va_list ap)
{
    CanopyVarValue out;
    char *fieldname;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRUCT;
    out->struct_hash = RedHash_New(0);
    if (!out->struct_hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    while ((fieldname = va_arg(ap, char *)) != NULL)
    {
        CanopyVarValue val = va_arg(ap, CanopyVarValue);
        RedHash_InsertS(out->struct_hash, fieldname, val);
    }
    va_end(ap);

    return out;
}

CanopyVarValue st_cloudvar_value_array(va_list ap)
{
    CanopyVarValue out;
    int index;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_ARRAY;
    out->array_hash = RedHash_New(0);
    if (!out->array_hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    while ((index = va_arg(ap, int)) != -1)
    {
        CanopyVarValue val = va_arg(ap, CanopyVarValue);
        RedHash_Insert(out->array_hash, &index, sizeof(index), val);
    }
    va_end(ap);

    return out;
}

void st_cloudvar_value_free(CanopyVarValue value)
{
    /*switch (value->datatype)
    {
        case CANOPY_DATATYPE_FLOAT32:
            free(value);
            break;
        case CANOPY_DATATYPE_STRING:
            free(value->val.val_string);
            free(value);
            break;
        default:
            assert(1);
            break;
    }*/
    assert(!"Not implemented");
}

CanopyVarReader st_cloudvar_reader_bool(bool *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_BOOL;
    out->dest.dest_bool = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_int8(int8_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT8;
    out->dest.dest_int8 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_uint8(uint8_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT8;
    out->dest.dest_uint8 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_int16(int16_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT16;
    out->dest.dest_int16 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_uint16(uint16_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT16;
    out->dest.dest_uint16 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_int32(int32_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_INT32;
    out->dest.dest_int32 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_uint32(uint32_t *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_UINT32;
    out->dest.dest_uint32 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_float32(float *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT32;
    out->dest.dest_float32 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_float64(double *dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT64;
    out->dest.dest_float64 = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_string(char **dest)
{
    CanopyVarReader out;
    out = malloc(sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRING;
    out->dest.dest_string = dest;
    return out;
}

CanopyVarReader st_cloudvar_reader_struct(va_list ap)
{
    CanopyVarReader out;
    char *fieldname;

    out = calloc(1, sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_STRUCT;
    out->dest.struct_hash = RedHash_New(0);
    if (!out->dest.struct_hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    while ((fieldname = va_arg(ap, char *)) != NULL)
    {
        CanopyVarReader reader = va_arg(ap, CanopyVarReader);
        RedHash_InsertS(out->dest.struct_hash, fieldname, reader);
    }
    va_end(ap);

    return out;
}

CanopyVarReader st_cloudvar_reader_array(va_list ap)
{
    CanopyVarReader out;
    int index;
    out = calloc(1, sizeof(STCloudVarReader_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_ARRAY;
    out->dest.array_hash = RedHash_New(0);
    if (!out->dest.array_hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    while ((index = va_arg(ap, int)) != -1)
    {
        CanopyVarReader reader = va_arg(ap, CanopyVarReader);
        RedHash_Insert(out->dest.array_hash, &index, sizeof(index), reader);
    }
    va_end(ap);

    return out;
}

const char * st_cloudvar_name(STCloudVar var)
{
    return sddl_var_name(var->decl);
}

bool st_cloudvar_has_value(STCloudVar var)
{
    // TODO: should this be recursive routine?
    return (var->basic_value != NULL) || !(st_cloudvar_is_basic(var));
}

typedef struct
{
    CanopyOnChangeCallback cb;
    void *userdata;
    char *varname;
    STCloudVarSystem sys;
} _CallbackEntry_t;

CanopyResultEnum st_cloudvar_register_on_change_callback(STCloudVar var, CanopyOnChangeCallback cb, void *userdata)
{
    _CallbackEntry_t * entry;

    entry = calloc(1, sizeof(_CallbackEntry_t));
    if (!entry)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    entry->cb = cb;
    entry->userdata = userdata;
    entry->varname = RedString_strdup(st_cloudvar_name(var));
    entry->sys = var->sys; // TODO: reference?

    RedHash_InsertS(var->sys->callbacks, entry->varname, entry);

    // TODO: needed?
    //var->dirty = true;
    //var->sys->dirty = true;

    // TODO: trigger callback when value changes locally
    // TODO: trigger callback when value changes remotely
    return CANOPY_SUCCESS;
}

CanopyDirectionEnum st_cloudvar_direction(STCloudVar var)
{
    return sddl_var_direction(var->decl);
}

CanopyDirectionEnum st_cloudvar_concrete_direction(STCloudVar var)
{
    return sddl_var_concrete_direction(var->decl);
}

/*static CanopyResultEnum _validate_value(SDDLVarDecl decl, CanopyVarValue value)
{
    // Do datatypes match?
    if (sddl_var_datatype(decl) != (SDDLDatatypeEnum)value->datatype)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }

    // Array datatype.  Verify that all indices are set properly.
    if (sddl_var_datatype(decl) == (SDDLDatatypeEnum)CANOPY_DATATYPE_ARRAY)
    {
        RedHashIterator_t iter;
        void *key, *hashValue;
        size_t keySize;
        SDDLVarDecl elementDecl = sddl_var_array_element();

        RED_HASH_FOREACH(iter, value->val_array.hash, &key, &keySize, &hashValue)
        {
            int idx = *((int *)key);
            STCloudVar elementVar = (STCloudVar)hashValue;

            // Check index bounds
            if (idx < 0)
            {
                return CANOPY_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS;
            }
            if (idx >= var->array_num_items)
            {
                return CANOPY_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS;
            }

            // Verify each value recursively
            result = _validate_value(elementDecl, elementVar->value);
            if (result != CANOPY_SUCCESS)
            {
                return result;
            }
        }
    }

    // TODO: other validation
    return CANOPY_SUCCESS;
}*/

CanopyResultEnum st_cloudvar_set_local_value_from_json(STCloudVarSystem sys, const char *varname, RedJsonValue jsonValue)
{
    /*// Convert JSON object to STCloudVarValue_t
    STCloudVarValue_t * val = calloc(1, sizeof(STCloudVarValue_t));

    // TODO: other datatypes
    if (RedJsonValue_IsNumber(jsonValue))
    {
        float newValue;
        // TODO: Take into consideration the local variable's configured
        // datatype.
        val->datatype = CANOPY_DATATYPE_FLOAT32;
        newValue = (float)RedJsonValue_GetNumber(jsonValue);
        //if (newValue != val) TODO: only tigger if value actually changed
        {
            // Value changed.  Trigger callback.
            _CallbackEntry_t * entry;
           
            entry = RedHash_GetWithDefaultS(sys->callbacks, varname, NULL);
            if (entry)
            {
                entry->cb(sys->context, varname, entry->userdata);
            }
        }
        val->val.val_float32 = newValue;
    }
    
    return CANOPY_SUCCESS;*/
    return CANOPY_ERROR_NOT_IMPLEMENTED;
}

bool st_cloudvar_value_already_used(CanopyVarValue value)
{
    return value->used;
}
void st_cloudvar_value_mark_used(CanopyVarValue value)
{
    value->used = true;
}

const char * st_cloudvar_direction_string(STCloudVar var)
{
    CanopyDirectionEnum direction = (CanopyDirectionEnum)sddl_var_direction(var->decl);
    return sddl_direction_string(direction);
}

const char * st_cloudvar_datatype_string(STCloudVar var)
{
    CanopyDatatypeEnum datatype = st_cloudvar_datatype(var);
    return sddl_datatype_string(datatype);
}

/*CanopyResultEnum st_cloudvar_config_extend_varargs(STCloudVarSystem sys, const char *varname, va_list ap)
{
    STCloudVar var;
    CanopyResultEnum result;

    var = RedHash_GetWithDefaultS(sys->vars, varname, NULL);
    if (!var)
    {
        // Var doesn't exist locally.
        // Create it.
        var = calloc(1, sizeof(struct STCloudVar_t));

        // Create new Cloud Variable (default configuration)
        var->sys = sys;
        var->name = RedString_strdup(varname);
        var->options = st_var_options_new_default();
        // TODO: set other properties

        RedHash_InsertS(sys->vars, varname, var);
    }

    // TODO: Input validation!
    result = st_var_options_extend_varargs(var->options, ap);

    _mark_dirty(sys, varname);
    return result;
}*/

const char * st_cloudvar_decl_string(STCloudVar var)
{
    return sddl_var_decl_string(var->decl);
}

CanopyDatatypeEnum st_cloudvar_datatype(STCloudVar var)
{
    return (CanopyDatatypeEnum)sddl_var_datatype(var->decl);
}

bool st_cloudvar_is_basic(STCloudVar var)
{
    return sddl_var_is_basic(var->decl);
}

RedJsonObject st_cloudvar_definition_json(STCloudVar var)
{
    return sddl_var_json(var->decl);
}
