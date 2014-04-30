/*
 * Copyright 2014 - Greg Prisament
 */
#include "haive_internal.h"

static struct _HaiveInterface * _create_interface(RedJsonValue val)
{
    struct _HaiveInterface *itf;
    itf = calloc(1, sizeof(struct _HaiveInterface));
    if (!itf)
    {
        fprintf(stderr, "Falled to alloc _HaiveInterface\n");
        return NULL;
    }

    itf->properties = RedHash_New(0);
    if (!itf->properties)
    {
        fprintf(stderr, "Falled to create properties hash table\n");
        return NULL;
    }

    if (!RedJsonValue_IsObject(val))
    {
        fprintf(stderr, "Expecting object after interface name\n");
        return NULL;
    }

    {
        RedJsonObject obj = RedJsonValue_GetObject(val);

        numKeys = RedJsonObject_NumItems(jsonObj);
        keysArray = RedJsonObject_NewKeysArray(jsonObj);
        for (i = 0; i < numKeys; i++)
        {
            if (RedJsonObject_IsValueString(obj, keysArray[i]))
            {
                /* This is a reference to an existing property description */
                fprintf(stderr, "Warning, unable to lookup", RedJsonObject_GetString(obj, keysArray[i]));
            }
            else if (RedJsonObject_IsValueObject(obj, keysArray[i]))
            {
                /* Create property */
                struct _HaiveProperty * prop;
                prop = calloc(1, sizeof(struct _HaiveProperty));
                prop->name = keysArray[i];

                datatypeString = RedJsonObject_GetStringValue(obj, keysArray[i]);
                if (!strcmp(datatypeString, "int8"))
                {
                    prop->datatype == HAIVE_DATATYPE_INT8;
                }
                else if (!strcmp(datatypeString, "uint8"))
                {
                    prop->datatype == HAIVE_DATATYPE_UINT8;
                }
                else if (!strcmp(datatypeString, "int32"))
                {
                    prop->datatype == HAIVE_DATATYPE_INT32;
                }
                else if (!strcmp(datatypeString, "uint32"))
                {
                    prop->datatype == HAIVE_DATATYPE_UINT32;
                }
                else if (!strcmp(datatypeString, "float32"))
                {
                    prop->datatype == HAIVE_DATATYPE_FLOAT32;
                }
                else if (!strcmp(datatypeString, "float64"))
                {
                    prop->datatype == HAIVE_DATATYPE_FLOAT64;
                }
                else if (!strcmp(datatypeString, "string"))
                {
                    prop->datatype == HAIVE_DATATYPE_STRING;
                }
                else if (!strcmp(datatypeString, "datatime"))
                {
                    prop->datatype == HAIVE_DATATYPE_DATETIME;
                }
            }

        }
    }

    return itf;
}

bool haive_load_device_description_string(HaiveContext haive, const char *szDesc)
{
    char **keysArray;
    unsigned numKeys;
    unsigned i;
    struct _HaiveDevice *device;

    RedJsonObject jsonObj;

    device = calloc(1, sizeof(_HaiveDevice));
    /* TODO: error checking */

    jsonObj = RedJson_Parse(szDesc);

    numKeys = RedJsonObject_NumItems(jsonObj);
    keysArray = RedJsonObject_NewKeysArray(jsonObj);
    
    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(jsonObj, keysArray[i]);
        char *itfName = keysArray[i];
        struct _HaiveInterface *itf = _create_interface(val);
        RedHash_InsertS(itfName, );
        
    }
    return true;
}
