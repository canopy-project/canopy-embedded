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
    char *name;
    CanopyVarValue value;
};

STCloudVarSystem st_cloudvar_system_new()
{
    STCloudVarSystem sys;

    sys = calloc(1, sizeof(struct STCloudVarSystem_t));
    sys->dirty = true;
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

static void _mark_dirty2(STCloudVarSystem sys, const char *varname)
{
    RedHash_InsertS(sys->dirty_vars, varname, (void *)true);
    sys->dirty = true;
}

CanopyResultEnum st_cloudvar_system_add(STCloudVarSystem sys, const char *varname)
{
    // Deprecated?
    STCloudVar var;
    var = calloc(1, sizeof(struct STCloudVar_t));

    // Create new Cloud Variable (default configuration)
    var->sys = sys;
    var->name = RedString_strdup(varname);
    var->value = NULL;

    RedHash_InsertS(sys->vars, varname, var);
    _mark_dirty2(sys, varname);

    return CANOPY_SUCCESS;
}

bool st_cloudvar_system_is_dirty(STCloudVarSystem sys)
{
    return sys->dirty;
}

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys)
{
    return RedHash_NumItems(sys->dirty_vars);
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

CanopyVarValue st_cloudvar_value_float32(float x)
{
    CanopyVarValue out;
    out = malloc(sizeof(STCloudVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out->datatype = CANOPY_DATATYPE_FLOAT32;
    out->val.val_float32 = x;
    return out;
}

CanopyVarValue st_cloudvar_value_string(const char *sz)
{
    CanopyVarValue out;
    out = malloc(sizeof(STCloudVarValue_t));
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

CanopyVarValue st_cloudvar_value_struct(va_list ap)
{
    CanopyVarValue out;
    char *fieldname;
    out = malloc(sizeof(STCloudVarValue_t));
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

static CanopyResultEnum _mark_dirty(STCloudVar var)
{
    var->dirty = true;
    var->sys->dirty = true;
    return CANOPY_SUCCESS;
}
CanopyResultEnum st_cloudvar_set_local_value_int8(STCloudVar var, int8_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT8);
    var->value->val.val_int8 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_int16(STCloudVar var, int16_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT16);
    var->value->val.val_int16 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_int32(STCloudVar var, int32_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT32);
    var->value->val.val_int32 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_uint8(STCloudVar var, uint8_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT8);
    var->value->val.val_uint8 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_uint16(STCloudVar var, uint16_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT16);
    var->value->val.val_uint16 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_uint32(STCloudVar var, uint32_t value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_INT32);
    var->value->val.val_uint32 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_float32(STCloudVar var, float value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_FLOAT32);
    var->value->val.val_float32 = value;
    return _mark_dirty(var);
}
CanopyResultEnum st_cloudvar_set_local_value_float64(STCloudVar var, float value)
{
    assert(var->value->datatype == CANOPY_DATATYPE_FLOAT64);
    return _mark_dirty(var);
}

const char * st_cloudvar_name(STCloudVar var)
{
    return var->name;
}

float st_cloudvar_local_value_float32(STCloudVar var)
{
    assert(var->value->datatype == CANOPY_DATATYPE_FLOAT32);
    return var->value->val.val_float32;
}

CanopyResultEnum st_cloudvar_register_on_change_callback(STCloudVar var, STOptions options)
{
    STOptions optionsCopy = st_options_dup(options);
    RedHash_InsertS(var->sys->callbacks, var->name, optionsCopy);
    var->dirty = true;
    var->sys->dirty = true;

    // TODO: trigger callback when value changes locally
    // TODO: trigger callback when value changes remotely
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_set_local_value(STCloudVarSystem sys, const char *varname, CanopyVarValue value)
{
    STCloudVar var;
    var = RedHash_GetWithDefaultS(sys->vars, varname, NULL);
    if (!var)
    {
        // Var doesn't exist locally.
        // Create it.
        var = calloc(1, sizeof(struct STCloudVar_t));

        // Create new Cloud Variable (default configuration)
        var->sys = sys;
        var->name = RedString_strdup(varname);
        // TODO: set other properties

        RedHash_InsertS(sys->vars, varname, var);
        _mark_dirty2(sys, varname);
    }

    var->value = value;
    /* TODO: copy? Delete old value? */

    return CANOPY_SUCCESS;
}

CanopyResultEnum st_cloudvar_get_local_value(STCloudVarSystem sys, const char *varname, CanopyVarReader dest)
{
    STCloudVar var;
    var = RedHash_GetWithDefaultS(sys->vars, varname, NULL);
    if (!var)
    {
        return CANOPY_ERROR_VARIABLE_NOT_FOUND;
    }
    
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
