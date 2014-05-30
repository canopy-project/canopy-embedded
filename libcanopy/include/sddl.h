/*
 * Copyright 2014 - Greg Prisament
 */

#ifndef SDDL_INCLUDED
#define SDDL_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef enum
{
    SDDL_PROPERTY_TYPE_INVALID,
    SDDL_PROPERTY_TYPE_CONTROL,
    SDDL_PROPERTY_TYPE_SENSOR,
    SDDL_PROPERTY_TYPE_CLASS,
} SDDLPropertyTypeEnum;

typedef enum
{
    SDDL_DATATYPE_INVALID,
    SDDL_DATATYPE_STRING,
    SDDL_DATATYPE_BOOL,
    SDDL_DATATYPE_INT8,
    SDDL_DATATYPE_UINT8,
    SDDL_DATATYPE_INT32,
    SDDL_DATATYPE_UINT32,
    SDDL_DATATYPE_FLOAT32,
    SDDL_DATATYPE_FLOAT64,
    SDDL_DATATYPE_DATETIME
} SDDLDatatypeEnum;

typedef enum
{
    SDDL_CONTROL_TYPE_INVALID,
    SDDL_CONTROL_TYPE_PARAMETER,
    SDDL_CONTROL_TYPE_TRIGGER,
} SDDLControlTypeEnum;

typedef struct SDDLDocument_t * SDDLDocument;

typedef struct SDDLProperty_t * SDDLProperty;

typedef struct SDDLControl_t * SDDLControl;

typedef struct SDDLSensor_t * SDDLSensor;

typedef struct SDDLClass_t * SDDLClass;

SDDLDocument sddl_parse(
        CanopyContext canopy, 
        const char *szDesc, 
        const char *descriptionName);

unsigned sddl_document_num_authors(SDDLDocument doc);
const char * sddl_document_author(SDDLDocument doc, unsigned index);

unsigned sddl_document_num_properties(SDDLDocument doc);
SDDLProperty sddl_document_property(SDDLDocument doc, unsigned index);

bool sddl_is_control(SDDLProperty prop);
bool sddl_is_sensor(SDDLProperty prop);
bool sddl_is_class(SDDLProperty prop);

SDDLControl SDDL_CONTROL(SDDProperty prop);
SDDLSensor SDDL_SENSOR(SDDProperty prop);
SDDLClass SDDL_CLASS(SDDProperty prop);

const char * sddl_control_name(SDDLControl control);
SDDLControlTypeEnum sddl_control_type(SDDLControl control);
SDDLDatatypeEnum sddl_control_datatype(SDDLControl control);
const char * sddl_control_description(SDDLControl control);
const double * sddl_control_max_value(SDDLControl control);
const double * sddl_control_min_value(SDDLControl control);
SDDLNumericDisplayHintEnum sddl_control_numeric_display_hint(SDDLControl control);
const char * sddl_control_regex(SDDLControl control);
const char * sddl_control_units(SDDLControl control);

const char * sddl_sensor_name(SDDLSensor sensor);
SDDLDatatypeEnum sddl_sensor_datatype(SDDLSensor sensor);
const char * sddl_sensor_description(SDDLSensor sensor);
double sddl_sensor_max_value(SDDLSensor sensor);
double sddl_sensor_min_value(SDDLSensor sensor);
SDDLNumericDisplayHintEnum sddl_sensor_numeric_display_hint(SDDLSensor sensor);
const char * sddl_sensor_regex(SDDLSensor sensor);
const char * sddl_sensor_units(SDDLSensor sensor);

unsigned sddl_class_num_authors(SDDLClass cls);
const char * sddl_class_author(SDDLClass cls, unsigned index);
const char * sddl_class_description(SDDLClass cls);
unsigned sddl_class_num_properties(SDDLClass cls);
SDDLProperty sddl_class_property(SDDLClass cls, unsigned index);

#endif
