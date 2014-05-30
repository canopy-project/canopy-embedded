/*
 * Copyright 2014 - Greg Prisament
 */
#include "sddl.h"

struct SDDLDocument_t
{
    unsigned numAuthors;
    char **authors;
    char *description;
    unsigned numProperties;
    SDDLProperty *properties;
};

struct SDDLProperty
{
    char *name;
    SDDLPropertyTypeEnum type;
    char *description;
};

struct SDDLControl
{
    SDDLProperty base;
    SDDLControlTypeEnum controlType;
    double *maxValue;
    double *minValue;
    SDDLNumericDisplayHintEnum numericDisplayHint;
    char *regex;
    char *units;
};

struct SDDLSensor
{
    SDDLProperty base;
    double *maxValue;
    double *minValue;
    SDDLNumericDisplayHintEnum numericDisplayHint;
    char *regex;
    char *units;
};

struct SDDLClass_t
{
    SDDLProperty base;
    unsigned numAuthors;
    char **authors;
    char *description;
    unsigned numProperties;
    SDDLProperty *properties;
};


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
    return doc->properties[index]
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

SDDLControl SDDL_CONTROL(SDDProperty prop)
{
    assert(sddl_is_control(prop));
    return (SDDLControl)prop;
}
SDDLSensor SDDL_SENSOR(SDDProperty prop)
{
    assert(sddl_is_sensor(prop));
    return (SDDLSensor)prop;
}
SDDLClass SDDL_CLASS(SDDProperty prop)
{
    assert(sddl_is_class(prop));
    return (SDDLClass)prop;
}

const char * sddl_control_name(SDDLControl control)
{
    return control->prop.name;
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
    return control->prop.description;
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
    return sensor->prop.name;
}
SDDLSensorTypeEnum sddl_sensor_type(SDDLSensor sensor)
{
    return sensor->sensorType;
}
SDDLDatatypeEnum sddl_sensor_datatype(SDDLSensor sensor)
{
    return sensor->datatype;
}
const char * sddl_sensor_description(SDDLSensor sensor)
{
    return sensor->prop.description;
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
    return cls->prop.description;
}
unsigned sddl_class_num_properties(SDDLClass cls)
{
    return cls->numProperties;
}
SDDLProperty sddl_class_property(SDDLClass cls, unsigned index) 
{
    return cls->properties[index]
}
