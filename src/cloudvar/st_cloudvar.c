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
#include "options/st_options.h"
#include "red_hash.h"
#include "red_string.h"
#include <assert.h>
#include <time.h>

struct STCloudVarSystem_t {
    bool dirty;
    CanopyContext context;
    RedHash vars; // maps (char *varname) -> (STCloudVar var)
    RedHash dirty_vars; // maps (char *varname) -> void 
    RedHash callbacks; // maps (char *varname) -> (STOptions)
};


typedef struct STCloudVarStruct_t
{
    RedHash hash; // string -> CanopyVarValue
} STCloudVarStruct_t;

typedef struct STCloudVarValue_t {
    CanopyDatatypeEnum datatype;
    bool used;
    union
    {
        bool val_bool;
        char * val_string;
        int8_t val_int8;
        uint8_t val_uint8;
        int16_t val_int16;
        uint16_t val_uint16;
        int32_t val_int32;
        uint32_t val_uint32;
        float val_float32;
        double val_float64;
        struct tm val_datetime;
        STCloudVarStruct_t val_strct;
    } val;
} STCloudVarValue_t;

typedef struct STCloudVarReader_t {
    CanopyDatatypeEnum datatype;
    bool used;
    union
    {
        bool *dest_bool;
        char ** dest_string;
        int8_t dest_int8;
        uint8_t *dest_uint8;
        int16_t *dest_int16;
        uint16_t *dest_uint16;
        int32_t *dest_int32;
        uint32_t *dest_uint32;
        float *dest_float32;
        double *dest_float64;
        struct tm *dest_datetime;
        STCloudVarStruct_t *dest_strct;
    } dest;
} STCloudVarReader_t;

struct STCloudVar_t {
    STCloudVarSystem sys;
    bool dirty;
    bool configured; // has this cloud variable been configured yet?
    char *name;
    char *decl_string;
    STVarOptions options;
    CanopyVarValue value;
};

STCloudVarSystem st_cloudvar_system_new(CanopyContext ctx)
{
    STCloudVarSystem sys;

    sys = calloc(1, sizeof(struct STCloudVarSystem_t));
    sys->dirty = true;
    sys->context = ctx;
    sys->vars = RedHash_New(0);
    sys->dirty_vars = RedHash_New(0);
    sys->callbacks = RedHash_New(0);
    return sys;
}

void st_cloudvar_system_free(STCloudVarSystem sys)
{
    if (sys)
    {
        // TODO: free all entries in hash table
        //RedHash_Free(sys->vars);
        //RedHash_Free(sys->dirty_vars);
        free(sys);
    }
}

bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname)
{
    return RedHash_HasKeyS(sys->vars, varname);
}

void st_cloudvar_system_clear_dirty(STCloudVarSystem sys)
{
    sys->dirty = false;
    RedHash_Clear(sys->dirty_vars);
}

static void _mark_dirty(STCloudVarSystem sys, const char *varname)
{
    RedHash_UpdateOrInsertS(sys->dirty_vars, NULL, varname, (void *)true);
    sys->dirty = true;
}

bool st_cloudvar_system_is_dirty(STCloudVarSystem sys)
{
    return sys->dirty;
}

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys)
{
    return RedHash_NumItems(sys->dirty_vars);
}

STCloudVar st_cloudvar_system_lookup_var(STCloudVarSystem sys, const char *varname)
{
    return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
}

CanopyResultEnum st_cloudvar_system_lookup_or_create_var(STCloudVar *out, STCloudVarSystem sys, const char *varname)
{
    STCloudVar var;
    var = RedHash_GetWithDefaultS(sys->vars, varname, NULL);
    if (!var)
    {
        // Var doesn't exist locally.
        // Create it.
        var = calloc(1, sizeof(struct STCloudVar_t));
        if (!var)
        {
            return CANOPY_ERROR_OUT_OF_MEMORY;
        }

        // Create new Cloud Variable (default configuration)
        var->sys = sys;
        var->name = RedString_strdup(varname);
        var->options = st_var_options_new_default();

        RedHash_InsertS(sys->vars, varname, var);
        _mark_dirty(sys, varname);
    }

    *out = var;

    return CANOPY_SUCCESS;
}

STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx)
{
    // TODO: Inefficient!
    RedHashIterator_t iter;
    const void *key;
    size_t keySize;
    uint32_t i = 0;
    RED_HASH_FOREACH(iter, sys->dirty_vars, &key, &keySize, NULL)
    {
        const char *varname = (const char *)key;
        if (i == idx)
        {
            return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
        }
        i++;
    }
    return NULL;
}

STCloudVar st_cloudvar_system_get_var(STCloudVarSystem sys, const char *varname)
{
    return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
}

CanopyVarValue st_cloudvar_value_bool(bool x)
{
    CanopyVarValue out;
    out = calloc(1, sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_BOOL;
    out->val.val_bool = x;
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
    out->val.val_float32 = x;
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
    out->val.val_float64 = x;
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
    out->val.val_int8 = x;
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
    out->val.val_int16 = x;
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
    out->val.val_int32 = x;
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
    out->val.val_string = RedString_strdup(sz);
    if (!out->val.val_string)
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
    out->val.val_int8 = x;
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
    out->val.val_uint16 = x;
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
    out->val.val_uint32 = x;
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
    out->val.val_strct.hash = RedHash_New(0);
    if (!out->val.val_strct.hash)
    {
        free(out);
        return NULL;
    }

    // Process each parameter
    while ((fieldname = va_arg(ap, char *)) != NULL)
    {
        CanopyVarValue val = va_arg(ap, CanopyVarValue);
        RedHash_InsertS(out->val.val_strct.hash, fieldname, val);
    }
    va_end(ap);

    return out;
}

void st_cloudvar_value_free(CanopyVarValue value)
{
    switch (value->datatype)
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
    }
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

CanopyVarReader st_cloudvar_reader_string(const char **dest)
{
    return NULL;
}

CanopyVarReader st_cloudvar_reader_struct(va_list ap)
{
    return NULL;
}

const char * st_cloudvar_name(STCloudVar var)
{
    return var->name;
}

bool st_cloudvar_has_value(STCloudVar var)
{
    return (var->value != NULL);
}

float st_cloudvar_local_value_float32(STCloudVar var)
{
    assert(var->value->datatype == CANOPY_DATATYPE_FLOAT32);
    return var->value->val.val_float32;
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
    entry->varname = RedString_strdup(var->name);
    entry->sys = var->sys; // TODO: reference?

    RedHash_InsertS(var->sys->callbacks, var->name, entry);

    // TODO: needed?
    //var->dirty = true;
    //var->sys->dirty = true;

    // TODO: trigger callback when value changes locally
    // TODO: trigger callback when value changes remotely
    return CANOPY_SUCCESS;
}

CanopyDirectionEnum st_cloudvar_direction(STCloudVar var)
{
    assert(var->options->has_CANOPY_VAR_DIRECTION);
    return var->options->val_CANOPY_VAR_DIRECTION;
}

CanopyResultEnum st_cloudvar_set_local_value(STCloudVar var, CanopyVarValue value)
{
    if (st_cloudvar_direction(var) == CANOPY_DIRECTION_IN)
    {
        return CANOPY_ERROR_CANNOT_MODIFY_INPUT_VARIABLE;
    }

    var->value = value;
    /* TODO: copy? Delete old value? */

    _mark_dirty(var->sys, var->name);

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_set_local_value_from_json(STCloudVarSystem sys, const char *varname, RedJsonValue jsonValue)
{
    // Convert JSON object to STCloudVarValue_t
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
    
    return CANOPY_SUCCESS;
}

RedJsonValue st_cloudvar_value_to_json(STCloudVar var)
{
    switch (var->value->datatype)
    {
        case CANOPY_DATATYPE_VOID:
            return RedJsonValue_Null();
        case CANOPY_DATATYPE_BOOL:
            return RedJsonValue_FromBoolean(var->value->val.val_bool);
        case CANOPY_DATATYPE_FLOAT32:
            return RedJsonValue_FromNumber(var->value->val.val_float32);
        case CANOPY_DATATYPE_FLOAT64:
            return RedJsonValue_FromNumber(var->value->val.val_float64);
        case CANOPY_DATATYPE_INT8:
            return RedJsonValue_FromNumber(var->value->val.val_int8);
        case CANOPY_DATATYPE_INT16:
            return RedJsonValue_FromNumber(var->value->val.val_int16);
        case CANOPY_DATATYPE_INT32:
            return RedJsonValue_FromNumber(var->value->val.val_int32);
        case CANOPY_DATATYPE_STRING:
            return RedJsonValue_FromString(var->value->val.val_string);
        case CANOPY_DATATYPE_UINT8:
            return RedJsonValue_FromNumber(var->value->val.val_uint8);
        case CANOPY_DATATYPE_UINT16:
            return RedJsonValue_FromNumber(var->value->val.val_uint16);
        case CANOPY_DATATYPE_UINT32:
            return RedJsonValue_FromNumber(var->value->val.val_uint32);
        default:
            assert(!"Unsupported datatype");
            return NULL;

    }
}

CanopyResultEnum st_cloudvar_get_local_value(STCloudVar var, CanopyVarReader dest)
{
    if (!var->value)
    {
        return CANOPY_ERROR_VARIABLE_NOT_SET;
    }
    if (dest->datatype != var->value->datatype)
    {
        return CANOPY_ERROR_INCORRECT_DATATYPE;
    }

    switch (dest->datatype)
    {
        case CANOPY_DATATYPE_FLOAT32:
            *dest->dest.dest_float32 = var->value->val.val_float32;
            break;
        default:
            return CANOPY_ERROR_NOT_IMPLEMENTED;
    }

    return CANOPY_SUCCESS;
}

bool st_cloudvar_value_already_used(CanopyVarValue value)
{
    return value->used;
}
void st_cloudvar_value_mark_used(CanopyVarValue value)
{
    value->used = true;
}

bool st_cloudvar_is_configured(STCloudVar var)
{
    return var->configured;
}

void st_cloudvar_mark_configured(STCloudVar var)
{
    var->configured = true;
}
const char * st_cloudvar_direction_string(STCloudVar var)
{
    assert(var->options->has_CANOPY_VAR_DIRECTION);
    switch (var->options->val_CANOPY_VAR_DIRECTION)
    {
        case CANOPY_DIRECTION_IN:
            return "in";
        case CANOPY_DIRECTION_OUT:
            return "out";
        case CANOPY_DIRECTION_INOUT:
            return "inout";
        default:
            return "invalid_direction";
    }
}

const char * st_cloudvar_datatype_string(STCloudVar var)
{
    assert(var->options->has_CANOPY_VAR_DATATYPE);
    switch (var->options->val_CANOPY_VAR_DATATYPE)
    {
        case CANOPY_DATATYPE_VOID:
            return "void";
        case CANOPY_DATATYPE_BOOL:
            return "bool";
        case CANOPY_DATATYPE_DATETIME:
            return "datetime";
        case CANOPY_DATATYPE_FLOAT32:
            return "float32";
        case CANOPY_DATATYPE_FLOAT64:
            return "float64";
        case CANOPY_DATATYPE_INT8:
            return "int8";
        case CANOPY_DATATYPE_INT16:
            return "int16";
        case CANOPY_DATATYPE_INT32:
            return "int32";
        case CANOPY_DATATYPE_STRING:
            return "string";
        case CANOPY_DATATYPE_UINT8:
            return "uint8";
        case CANOPY_DATATYPE_UINT16:
            return "uint16";
        case CANOPY_DATATYPE_UINT32:
            return "uint32";
        default:
            return "invalid_datatype";
    }
}

CanopyResultEnum st_cloudvar_config_extend_varargs(STCloudVarSystem sys, const char *varname, va_list ap)
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
}

const char * st_cloudvar_decl_string(STCloudVar var)
{
    // TODO: cache
    free(var->decl_string);
    
    var->decl_string = RedString_PrintfToNewChars("%s %s %s", 
            st_cloudvar_direction_string(var),
            st_cloudvar_datatype_string(var),
            st_cloudvar_name(var));

    return var->decl_string;
}