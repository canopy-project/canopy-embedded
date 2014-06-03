/*
 * Copyright 2014 - Greg Prisament
 */
#include "sddl.h"
#include "red_string.h"
#include "red_json.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct SDDLDocument_t
{
    unsigned numAuthors;
    char **authors;
    char *description;
    unsigned numProperties;
    SDDLProperty *properties;
};

struct SDDLProperty_t
{
    char *name;
    SDDLPropertyTypeEnum type;
    char *description;
};

struct SDDLControl_t
{
    struct SDDLProperty_t base;
    SDDLDatatypeEnum datatype;
    SDDLControlTypeEnum controlType;
    double *maxValue;
    double *minValue;
    SDDLNumericDisplayHintEnum numericDisplayHint;
    char *regex;
    char *units;
};

struct SDDLSensor_t
{
    struct SDDLProperty_t base;
    SDDLDatatypeEnum datatype;
    double *maxValue;
    double *minValue;
    SDDLNumericDisplayHintEnum numericDisplayHint;
    char *regex;
    char *units;
};

struct SDDLClass_t
{
    struct SDDLProperty_t base;
    unsigned numAuthors;
    char **authors;
    char *description;
    unsigned numProperties;
    SDDLProperty *properties;
};

static SDDLNumericDisplayHintEnum _display_hint_from_string(const char *sz)
{
    if (!strcmp(sz, "normal"))
    {
        return SDDL_NUMERIC_DISPLAY_HINT_NORMAL;
    }
    else if (!strcmp(sz, "percentage"))
    {
        return SDDL_NUMERIC_DISPLAY_HINT_PERCENTAGE;
    }
    else if (!strcmp(sz, "hex"))
    {
        return SDDL_NUMERIC_DISPLAY_HINT_HEX;
    }
    else if (!strcmp(sz, "scientific"))
    {
        return SDDL_NUMERIC_DISPLAY_HINT_SCIENTIFIC;
    }
    return SDDL_NUMERIC_DISPLAY_HINT_INVALID;
}

static SDDLDatatypeEnum _datatype_from_string(const char *sz)
{
    if (!strcmp(sz, "string"))
    {
        return SDDL_DATATYPE_STRING;
    }
    else if (!strcmp(sz, "boolean"))
    {
        return SDDL_DATATYPE_BOOL;
    }
    else if (!strcmp(sz, "int8"))
    {
        return SDDL_DATATYPE_INT8;
    }
    else if (!strcmp(sz, "uint8"))
    {
        return SDDL_DATATYPE_UINT8;
    }
    else if (!strcmp(sz, "int16"))
    {
        return SDDL_DATATYPE_INT16;
    }
    else if (!strcmp(sz, "uint16"))
    {
        return SDDL_DATATYPE_UINT16;
    }
    else if (!strcmp(sz, "int32"))
    {
        return SDDL_DATATYPE_INT32;
    }
    else if (!strcmp(sz, "uint32"))
    {
        return SDDL_DATATYPE_UINT32;
    }
    else if (!strcmp(sz, "float32"))
    {
        return SDDL_DATATYPE_FLOAT32;
    }
    else if (!strcmp(sz, "float64"))
    {
        return SDDL_DATATYPE_FLOAT64;
    }
    return SDDL_CONTROL_TYPE_INVALID;
}

static SDDLControlTypeEnum _control_type_from_string(const char *sz)
{
    if (!strcmp(sz, "parameter"))
    {
        return SDDL_CONTROL_TYPE_PARAMETER;
    }
    else if (!strcmp(sz, "trigger"))
    {
        return SDDL_CONTROL_TYPE_TRIGGER;
    }
    return SDDL_CONTROL_TYPE_INVALID;
}

static SDDLControl _sddl_parse_control(RedString decl, RedJsonObject def)
{
    SDDLControl out;
    unsigned numKeys;
    unsigned i;
    char **keysArray;

    out = calloc(1, sizeof(struct SDDLControl_t));
    if (!out)
    {
        return NULL;
    }

    RedStringList split = RedString_Split(decl, ' ');
    RedString name = RedStringList_GetString(split, 1);
    RedStringList_Free(split);

    out->base.name = RedString_strdup(RedString_GetChars(name));
    out->base.type = SDDL_PROPERTY_TYPE_CONTROL;
    /* TODO: set defaults */

    numKeys = RedJsonObject_NumItems(def);
    keysArray = RedJsonObject_NewKeysArray(def);

    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(def, keysArray[i]);
        RedString key = RedString_New(keysArray[i]);

        if (RedString_Equals(key, "control-type"))
        {
            char *controlTypeString;
            if (!RedJsonValue_IsString(val))
            {
                printf("control-type must be string\n");
                return NULL;
            }
            controlTypeString = RedJsonValue_GetString(val);
            out->controlType = _control_type_from_string(controlTypeString);
            if (out->controlType == SDDL_CONTROL_TYPE_INVALID)
            {
                printf("invalid control-type %s\n", controlTypeString);
                return NULL;
            }
        }
        else if (RedString_Equals(key, "datatype"))
        {
            char *datatypeString;
            if (!RedJsonValue_IsString(val))
            {
                printf("datatype must be string\n");
                return NULL;
            }
            datatypeString = RedJsonValue_GetString(val);
            out->datatype = _datatype_from_string(datatypeString);
            if (out->datatype == SDDL_DATATYPE_INVALID)
            {
                printf("invalid datatype %s\n", datatypeString);
                return NULL;
            }
        }
        else if (RedString_Equals(key, "description"))
        {
            char *description;
            if (!RedJsonValue_IsString(val))
            {
                printf("description must be string\n");
                return NULL;
            }
            description = RedJsonValue_GetString(val);
            out->base.description = RedString_strdup(description);
            if (!out->base.description)
            {
                printf("OOM duplicating description string\n");
                return NULL;
            }
        }
        else if (RedString_Equals(key, "max-value"))
        {
            if (RedJsonValue_IsNull(val))
            {
                out->maxValue = NULL;
            }
            else
            {
                double * pMaxValue;
                if (!RedJsonValue_IsNumber(val))
                {
                    printf("max-value must be number or null\n");
                    return NULL;
                }
                pMaxValue = malloc(sizeof(double));
                if (!pMaxValue)
                {
                    printf("OOM allocating max-value\n");
                    return NULL;
                }
                *pMaxValue = RedJsonValue_GetNumber(val);
                out->maxValue = pMaxValue;
            }
        }
        else if (RedString_Equals(key, "min-value"))
        {
            if (RedJsonValue_IsNull(val))
            {
                out->minValue = NULL;
            }
            else
            {
                double * pMinValue;
                if (!RedJsonValue_IsNumber(val))
                {
                    printf("min-value must be number or null\n");
                    return NULL;
                }
                pMinValue = malloc(sizeof(double));
                if (!pMinValue)
                {
                    printf("OOM allocating min-value\n");
                    return NULL;
                }
                *pMinValue = RedJsonValue_GetNumber(val);
                out->minValue = pMinValue;
            }
        }
        else if (RedString_Equals(key, "numeric-display-hint"))
        {
            char *displayHintString;
            if (!RedJsonValue_IsString(val))
            {
                printf("datatype must be string\n");
                return NULL;
            }
            displayHintString = RedJsonValue_GetString(val);
            out->numericDisplayHint = _display_hint_from_string(displayHintString);
            if (out->numericDisplayHint == SDDL_NUMERIC_DISPLAY_HINT_INVALID)
            {
                printf("invalid numeric-display-hint %s", displayHintString);
                return NULL;
            }
        }
        else if (RedString_Equals(key, "regex"))
        {
            char *regex;
            if (!RedJsonValue_IsString(val))
            {
                printf("regex must be string\n");
                return NULL;
            }
            regex = RedJsonValue_GetString(val);
            out->regex = RedString_strdup(regex);
            if (!out->regex)
            {
                printf("OOM duplicating regex string\n");
                return NULL;
            }
        }
        else if (RedString_Equals(key, "units"))
        {
            char *units;
            if (!RedJsonValue_IsString(val))
            {
                printf("units must be string\n");
                return NULL;
            }
            units = RedJsonValue_GetString(val);
            out->units = RedString_strdup(units);
            if (!out->units)
            {
                printf("OOM duplicating units string\n");
                return NULL;
            }
        }
        else
        {
            printf("Unexpected field: %s\n", RedString_GetChars(key));
        }
        RedString_Free(key);
    }

    return out;
}

static SDDLSensor _sddl_parse_sensor(RedString decl, RedJsonObject def)
{
    SDDLSensor out;
    unsigned numKeys;
    unsigned i;
    char **keysArray;

    out = calloc(1, sizeof(struct SDDLSensor_t));
    if (!out)
    {
        return NULL;
    }

    RedStringList split = RedString_Split(decl, ' ');
    RedString name = RedStringList_GetString(split, 1);
    RedStringList_Free(split);

    out->base.name = RedString_strdup(RedString_GetChars(name));
    out->base.type = SDDL_PROPERTY_TYPE_SENSOR;
    /* TODO: set defaults */

    numKeys = RedJsonObject_NumItems(def);
    keysArray = RedJsonObject_NewKeysArray(def);

    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(def, keysArray[i]);
        RedString key = RedString_New(keysArray[i]);

        if (RedString_Equals(key, "datatype"))
        {
            char *datatypeString;
            if (!RedJsonValue_IsString(val))
            {
                printf("datatype must be string\n");
                return NULL;
            }
            datatypeString = RedJsonValue_GetString(val);
            out->datatype = _datatype_from_string(datatypeString);
            if (out->datatype == SDDL_DATATYPE_INVALID)
            {
                printf("invalid datatype %s", datatypeString);
                return NULL;
            }
        }
        else if (RedString_Equals(key, "description"))
        {
            char *description;
            if (!RedJsonValue_IsString(val))
            {
                printf("description must be string\n");
                return NULL;
            }
            description = RedJsonValue_GetString(val);
            out->base.description = RedString_strdup(description);
            if (!out->base.description)
            {
                printf("OOM duplicating description string\n");
                return NULL;
            }
        }
        else if (RedString_Equals(key, "max-value"))
        {
            if (RedJsonValue_IsNull(val))
            {
                out->maxValue = NULL;
            }
            else
            {
                double * pMaxValue;
                if (!RedJsonValue_IsNumber(val))
                {
                    printf("max-value must be number or null\n");
                    return NULL;
                }
                pMaxValue = malloc(sizeof(double));
                if (!pMaxValue)
                {
                    printf("OOM allocating max-value\n");
                    return NULL;
                }
                *pMaxValue = RedJsonValue_GetNumber(val);
                out->maxValue = pMaxValue;
            }
        }
        else if (RedString_Equals(key, "min-value"))
        {
            if (RedJsonValue_IsNull(val))
            {
                out->minValue = NULL;
            }
            else
            {
                double * pMinValue;
                if (!RedJsonValue_IsNumber(val))
                {
                    printf("min-value must be number or null\n");
                    return NULL;
                }
                pMinValue = malloc(sizeof(double));
                if (!pMinValue)
                {
                    printf("OOM allocating min-value\n");
                    return NULL;
                }
                *pMinValue = RedJsonValue_GetNumber(val);
                out->minValue = pMinValue;
            }
        }
        else if (RedString_Equals(key, "numeric-display-hint"))
        {
            char *displayHintString;
            if (!RedJsonValue_IsString(val))
            {
                printf("datatype must be string\n");
                return NULL;
            }
            displayHintString = RedJsonValue_GetString(val);
            out->numericDisplayHint = _display_hint_from_string(displayHintString);
            if (out->numericDisplayHint == SDDL_NUMERIC_DISPLAY_HINT_INVALID)
            {
                printf("invalid numeric-display-hint %s", displayHintString);
                return NULL;
            }
        }
        else if (RedString_Equals(key, "regex"))
        {
            char *regex;
            if (!RedJsonValue_IsString(val))
            {
                printf("regex must be string\n");
                return NULL;
            }
            regex = RedJsonValue_GetString(val);
            out->regex = RedString_strdup(regex);
            if (!out->regex)
            {
                printf("OOM duplicating regex string\n");
                return NULL;
            }
        }
        else if (RedString_Equals(key, "units"))
        {
            char *units;
            if (!RedJsonValue_IsString(val))
            {
                printf("units must be string\n");
                return NULL;
            }
            units = RedJsonValue_GetString(val);
            out->units = RedString_strdup(units);
            if (!out->units)
            {
                printf("OOM duplicating units string\n");
                return NULL;
            }
        }
        else
        {
            printf("Unexpected field: %s", RedString_GetChars(key));
        }
        RedString_Free(key);
    }
    
    return out;
}

static SDDLClass _sddl_parse_class(RedString decl, RedJsonObject def)
{
    SDDLClass cls;
    unsigned numKeys;
    unsigned i;
    char **keysArray;

    cls = calloc(1, sizeof(struct SDDLClass_t));
    if (!cls)
    {
        return NULL;
    }

    RedStringList split = RedString_Split(decl, ' ');
    RedString name = RedStringList_GetString(split, 1);
    RedStringList_Free(split);

    cls->base.name = RedString_strdup(RedString_GetChars(name));
    cls->base.type = SDDL_PROPERTY_TYPE_CLASS;
    /* TODO: set defaults */

    numKeys = RedJsonObject_NumItems(def);
    keysArray = RedJsonObject_NewKeysArray(def);
    
    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(def, keysArray[i]);
        RedString key = RedString_New(keysArray[i]);
        if (RedString_BeginsWith(key, "control "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for control definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLControl control = _sddl_parse_control(key, obj);
            if (!control)
            {
                return NULL;
            }
            cls->numProperties++;
            cls->properties = realloc(
                    cls->properties, 
                    cls->numProperties*
                    sizeof(SDDLProperty));
            if (!cls->properties)
            {
                printf("OOM expanding cls->properties\n");
                return NULL;
            }
            cls->properties[cls->numProperties - 1] = SDDL_PROPERTY(control);
        }
        else if  (RedString_BeginsWith(key, "sensor "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for sensor definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLSensor sensor = _sddl_parse_sensor(key, obj);
            if (!sensor)
            {
                return NULL;
            }
            cls->numProperties++;
            cls->properties = realloc(
                    cls->properties, 
                    cls->numProperties*
                    sizeof(SDDLProperty));
            if (!cls->properties)
            {
                printf("OOM expanding cls->properties\n");
                return NULL;
            }
            cls->properties[cls->numProperties - 1] = SDDL_PROPERTY(sensor);
        }
        else if  (RedString_BeginsWith(key, "class "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for class definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLClass class = _sddl_parse_class(key, obj);
            if (!class)
            {
                return NULL;
            }
            cls->numProperties++;
            cls->properties = realloc(
                    cls->properties, 
                    cls->numProperties*
                    sizeof(SDDLProperty));
            if (!cls->properties)
            {
                printf("OOM expanding cls->properties\n");
                return NULL;
            }
            cls->properties[cls->numProperties - 1] = SDDL_PROPERTY(class);
        }
        else if (RedString_Equals(key, "authors"))
        {
            if (!RedJsonValue_IsArray(val))
            {
                printf("Expected list for authors\n");
                return NULL;
            }
            
            RedJsonArray authorList = RedJsonValue_GetArray(val);
            cls->numAuthors = RedJsonArray_NumItems(authorList);
            cls->authors = calloc(cls->numAuthors, sizeof(char *));
            if (!cls->authors)
            {
                printf("OOM - Failed to allocate author list\n");
                return NULL;
            }
            for (i = 0; i < cls->numAuthors; i++)
            {
                char * author;
                if (!RedJsonArray_IsEntryString(authorList, i))
                {
                    printf("Expected string for authors list entry\n");
                    return NULL;
                }
                author = RedJsonArray_GetEntryString(authorList, i);
                cls->authors[i] = RedString_strdup(author);
                if (!cls->authors)
                {
                    printf("OOM - Failed to duplicate author\n");
                    return NULL;
                }
            }
        }
        else if (RedString_Equals(key, "description"))
        {
            char *description;
            if (!RedJsonValue_IsString(val))
            {
                printf("description must be string\n");
                return NULL;
            }
            description = RedJsonValue_GetString(val);
            cls->description = RedString_strdup(description);
            if (!cls->description)
            {
                printf("OOM duplicating description string\n");
                return NULL;
            }
        }

        RedString_Free(key);
    }

    return cls;
}

SDDLDocument sddl_load_and_parse(const char *filename)
{
    FILE *fp;
    SDDLDocument out;
    fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }
    out = sddl_load_and_parse_file(fp);
    fclose(fp);
    return out;
}

SDDLDocument sddl_load_and_parse_file(FILE *file)
{
    /* Read entire file into memory */
    long filesize;
    char *buffer;
    SDDLDocument out;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file); 
    fseek(file, 0, SEEK_SET);
    buffer = calloc(1, filesize+1);
    fread(buffer, 1, filesize, file);
    out = sddl_parse(buffer);
    free(buffer);
    return out;
}

SDDLDocument sddl_parse(const char *sddl)
{
    RedJsonObject jsonObj;
    SDDLDocument doc;
    unsigned numKeys;
    unsigned i;
    char **keysArray;

    doc = calloc(1, sizeof(struct SDDLDocument_t));
    if (!doc)
    {
        return NULL;
    }

    jsonObj = RedJson_Parse(sddl);
    numKeys = RedJsonObject_NumItems(jsonObj);
    keysArray = RedJsonObject_NewKeysArray(jsonObj);

    for (i = 0; i < numKeys; i++)
    {
        RedJsonValue val = RedJsonObject_Get(jsonObj, keysArray[i]);
        RedString key = RedString_New(keysArray[i]);
        if (RedString_BeginsWith(key, "control "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for control definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLControl control = _sddl_parse_control(key, obj);
            if (!control)
            {
                return NULL;
            }
            doc->numProperties++;
            doc->properties = realloc(
                    doc->properties, 
                    doc->numProperties*
                    sizeof(SDDLProperty));
            if (!doc->properties)
            {
                printf("OOM expanding doc->properties\n");
                return NULL;
            }
            doc->properties[doc->numProperties - 1] = SDDL_PROPERTY(control);
        }
        else if  (RedString_BeginsWith(key, "sensor "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for sensor definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLSensor sensor = _sddl_parse_sensor(key, obj);
            if (!sensor)
            {
                return NULL;
            }
            doc->numProperties++;
            doc->properties = realloc(
                    doc->properties, 
                    doc->numProperties*
                    sizeof(SDDLProperty));
            if (!doc->properties)
            {
                printf("OOM expanding doc->properties\n");
                return NULL;
            }
            doc->properties[doc->numProperties - 1] = SDDL_PROPERTY(sensor);
        }
        else if  (RedString_BeginsWith(key, "class "))
        {
            if (!RedJsonValue_IsObject(val))
            {
                printf("Expected object for class definition\n");
                return NULL;
            }
            RedJsonObject obj = RedJsonValue_GetObject(val);
            SDDLClass class = _sddl_parse_class(key, obj);
            if (!class)
            {
                return NULL;
            }
            doc->numProperties++;
            doc->properties = realloc(
                    doc->properties, 
                    doc->numProperties*
                    sizeof(SDDLProperty));
            if (!doc->properties)
            {
                printf("OOM expanding doc->properties\n");
                return NULL;
            }
            doc->properties[doc->numProperties - 1] = SDDL_PROPERTY(class);
        }
        else if (RedString_Equals(key, "authors"))
        {
            char * author;
            if (!RedJsonValue_IsArray(val))
            {
                printf("Expected list for authors\n");
                return NULL;
            }
            
            RedJsonArray authorList = RedJsonValue_GetArray(val);
            doc->numAuthors = RedJsonArray_NumItems(authorList);
            doc->authors = calloc(doc->numAuthors, sizeof(char *));
            if (!doc->authors)
            {
                printf("OOM - Failed to allocate author list\n");
                return NULL;
            }
            for (i = 0; i < doc->numAuthors; i++)
            {
                if (!RedJsonArray_IsEntryString(authorList, i))
                {
                    printf("Expected string for authors list entry\n");
                    return NULL;
                }
                author = RedJsonArray_GetEntryString(authorList, i);
                doc->authors[i] = RedString_strdup(author);
                if (!doc->authors)
                {
                    printf("OOM - Failed to duplicate author\n");
                    return NULL;
                }
            }
        }
        else if (RedString_Equals(key, "description"))
        {
            char *description;
            if (!RedJsonValue_IsString(val))
            {
                printf("description must be string\n");
                return NULL;
            }
            description = RedJsonValue_GetString(val);
            doc->description = RedString_strdup(description);
            if (!doc->description)
            {
                printf("OOM duplicating description string\n");
                return NULL;
            }
        }

        RedString_Free(key);
    }

    return doc;
}

unsigned sddl_document_num_authors(SDDLDocument doc)
{
    return doc->numAuthors;
}

const char * sddl_document_author(SDDLDocument doc, unsigned index)
{
    return doc->authors[index];
}

unsigned sddl_document_num_properties(SDDLDocument doc)
{
    return doc->numProperties;
}

SDDLProperty sddl_document_property(SDDLDocument doc, unsigned index) 
{
    return doc->properties[index];
}

bool sddl_is_control(SDDLProperty prop)
{
    return (prop->type == SDDL_PROPERTY_TYPE_CONTROL);
}
bool sddl_is_sensor(SDDLProperty prop)
{
    return (prop->type == SDDL_PROPERTY_TYPE_SENSOR);
}
bool sddl_is_class(SDDLProperty prop)
{
    return (prop->type == SDDL_PROPERTY_TYPE_CLASS);
}

SDDLControl SDDL_CONTROL(SDDLProperty prop)
{
    assert(sddl_is_control(prop));
    return (SDDLControl)prop;
}
SDDLSensor SDDL_SENSOR(SDDLProperty prop)
{
    assert(sddl_is_sensor(prop));
    return (SDDLSensor)prop;
}
SDDLClass SDDL_CLASS(SDDLProperty prop)
{
    assert(sddl_is_class(prop));
    return (SDDLClass)prop;
}

const char * sddl_control_name(SDDLControl control)
{
    return control->base.name;
}
SDDLControlTypeEnum sddl_control_type(SDDLControl control)
{
    return control->controlType;
}
SDDLDatatypeEnum sddl_control_datatype(SDDLControl control)
{
    return control->datatype;
}
const char * sddl_control_description(SDDLControl control)
{
    return control->base.description;
}
const double * sddl_control_max_value(SDDLControl control)
{
    return control->maxValue;
}
const double * sddl_control_min_value(SDDLControl control)
{
    return control->minValue;
}
SDDLNumericDisplayHintEnum sddl_control_numeric_display_hint(SDDLControl control)
{
    return control->numericDisplayHint;
}
const char * sddl_control_regex(SDDLControl control)
{
    return control->regex;
}
const char * sddl_control_units(SDDLControl control)
{
    return control->units;
}

const char * sddl_sensor_name(SDDLSensor sensor)
{
    return sensor->base.name;
}
SDDLDatatypeEnum sddl_sensor_datatype(SDDLSensor sensor)
{
    return sensor->datatype;
}
const char * sddl_sensor_description(SDDLSensor sensor)
{
    return sensor->base.description;
}
const double * sddl_sensor_max_value(SDDLSensor sensor)
{
    return sensor->maxValue;
}
const double * sddl_sensor_min_value(SDDLSensor sensor)
{
    return sensor->minValue;
}
SDDLNumericDisplayHintEnum sddl_sensor_numeric_display_hint(SDDLSensor sensor)
{
    return sensor->numericDisplayHint;
}
const char * sddl_sensor_regex(SDDLSensor sensor)
{
    return sensor->regex;
}
const char * sddl_sensor_units(SDDLSensor sensor)
{
    return sensor->units;
}

const char * sddl_class_name(SDDLClass cls)
{
    return cls->base.name;
}
unsigned sddl_class_num_authors(SDDLClass cls)
{
    return cls->numAuthors;
}
const char * sddl_class_author(SDDLClass cls, unsigned index)
{
    return cls->authors[index];
}
const char * sddl_class_description(SDDLClass cls)
{
    return cls->base.description;
}
unsigned sddl_class_num_properties(SDDLClass cls)
{
    return cls->numProperties;
}
SDDLProperty sddl_class_property(SDDLClass cls, unsigned index) 
{
    return cls->properties[index];
}

