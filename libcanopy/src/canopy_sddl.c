/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy_internal.h"
#include "red_json.h"
#include <assert.h>

static struct _CanopyProperty * _create_property(const char *propName, RedJsonObject jsonObj)
{
    struct _CanopyProperty * prop;
    unsigned numKeys = RedJsonObject_NumItems(jsonObj);
    char **keysArray = RedJsonObject_NewKeysArray(jsonObj);
    unsigned i;

    prop = calloc(1, sizeof(struct _CanopyProperty));
    assert(prop);

    prop->name = calloc(1, strlen(propName) + 1);
    assert(prop->name);
    strcpy(prop->name, propName);

    for (i = 0; i < numKeys; i++)
    {
        if (!strcmp(keysArray[i], "datatype"))
        {
            char *datatypeString = RedJsonObject_GetString(jsonObj, keysArray[i]);
            if (!strcmp(datatypeString, "int8"))
            {
                prop->datatype = CANOPY_DATATYPE_INT8;
            }
            else if (!strcmp(datatypeString, "uint8"))
            {
                prop->datatype = CANOPY_DATATYPE_UINT8;
            }
            else if (!strcmp(datatypeString, "int32"))
            {
                prop->datatype = CANOPY_DATATYPE_INT32;
            }
            else if (!strcmp(datatypeString, "uint32"))
            {
                prop->datatype = CANOPY_DATATYPE_UINT32;
            }
            else if (!strcmp(datatypeString, "float32"))
            {
                prop->datatype = CANOPY_DATATYPE_FLOAT32;
            }
            else if (!strcmp(datatypeString, "float64"))
            {
                prop->datatype = CANOPY_DATATYPE_FLOAT64;
            }
            else if (!strcmp(datatypeString, "string"))
            {
                prop->datatype = CANOPY_DATATYPE_STRING;
            }
            else if (!strcmp(datatypeString, "datatime"))
            {
                prop->datatype = CANOPY_DATATYPE_DATETIME;
            }
        }
        else if (!strcmp(keysArray[i], "category"))
        {
        }
        else if (!strcmp(keysArray[i], "description"))
        {
        }
        else
        {
            printf("Unexpected field %s\n", keysArray[i]);
        }
    }
    return prop;
}

static struct _CanopyInterface * _create_interface(RedJsonValue val)
{
    struct _CanopyInterface *itf;
    itf = calloc(1, sizeof(struct _CanopyInterface));
    if (!itf)
    {
        fprintf(stderr, "Falled to alloc _CanopyInterface\n");
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
        RedJsonObject jsonObj = RedJsonValue_GetObject(val);
        unsigned i;

        unsigned numKeys = RedJsonObject_NumItems(jsonObj);
        char ** keysArray = RedJsonObject_NewKeysArray(jsonObj);
        for (i = 0; i < numKeys; i++)
        {
            if (RedJsonObject_IsValueString(jsonObj, keysArray[i]))
            {
                /* This is a reference to an existing property description */
                fprintf(stderr, "Warning, unable to lookup %s", RedJsonObject_GetString(jsonObj, keysArray[i]));
            }
            else if (RedJsonObject_IsValueObject(jsonObj, keysArray[i]))
            {
                /* Create property */
                struct _CanopyProperty * prop;

                prop = _create_property(keysArray[i], RedJsonObject_GetObject(jsonObj, keysArray[i]));

                RedHash_InsertS(itf->properties, prop->name, prop);
            }

        }
    }

    return itf;
}

bool canopy_load_device_description_string(CanopyContext canopy, const char *szDesc, const char *descriptionName)
{
    char **keysArray;
    unsigned numKeys;
    unsigned i;
    struct _CanopyDevice *device;

    RedJsonObject jsonObj;

    device = calloc(1, sizeof(_CanopyDevice));
    /* TODO: error checking */
    device->interfaces = RedHash_New(0);

    jsonObj = RedJson_Parse(szDesc);

    numKeys = RedJsonObject_NumItems(jsonObj);
    keysArray = RedJsonObject_NewKeysArray(jsonObj);
    
    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(jsonObj, keysArray[i]);
        char *itfName = keysArray[i];
        struct _CanopyInterface *itf = _create_interface(val);
        RedHash_InsertS(device->interfaces, itfName, itf);
        canopy->properties = itf->properties; /* This is the main interface */
    }
    return true;
}
