#include "canopy_new.h"

typedef struct CanopyVarStruct_t
{
    RedHash hash; // string -> CanopyVarValue
} CanopyVarStruct_t;

typedef struct CanopyVarValue_t
{
    CanopyDatatypeEnum datatype;
    union
    {
        float val_float32;
        char *val_string;
        CanopyVarStruct_t strct;
    } val;
} CanopyVarValue_t;


CanopyVarValue CANOPY_FLOAT32(float x)
{
    CanopyVarValue out;
    out = malloc(sizeof(CanopyVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out.datatype = CANOPY_DATATYPE_FLOAT32;
    out.val.val_float32 = x;
    return out;
}

CanopyVarValue CANOPY_STRING(const char *sz)
{
    CanopyVarValue out;
    out = malloc(sizeof(CanopyVarValue_t));
    if (!out)
    {
        return NULL;
    }
    out.datatype = CANOPY_DATATYPE_STRING;
    out.val.val_string = RedString_strdup(sz);
    if (!out.val.val_string)
    {
        free(out);
        return NULL;
    }
    return out;
}

CanopyVarValue CANOPY_STRUCT(start, ...)
{
    CanopyVarValue out;
    out = malloc(sizeof(CanopyVarValue_t));
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
    va_list ap;
    ap = va_start(start);
    while (fieldname = va_arg(ap, char *))
    {
        CanopyVarValue val = va_arg(ap, CanopyVarValue);
        RedHash_Add(fieldname, val);
    }
    va_end();

    return out;
}

void canopy_var_value_free(CanopyVarValue value)
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

