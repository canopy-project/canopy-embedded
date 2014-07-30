/*
 * Copyright 2014 Gregory Prisament
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SDDL_INCLUDED
#define SDDL_INCLUDED

#include "red_json.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    SDDL_SUCCESS,

    /*
     * Parsing SDDL content failed.
     */
    SDDL_ERROR_PARSING,
} SDDLResultEnum;

typedef enum
{
    SDDL_PROPERTY_TYPE_INVALID,
    SDDL_PROPERTY_TYPE_CONTROL,
    SDDL_PROPERTY_TYPE_SENSOR,
    SDDL_PROPERTY_TYPE_CLASS
} SDDLPropertyTypeEnum;

typedef enum
{
    SDDL_DATATYPE_INVALID,
    SDDL_DATATYPE_VOID,
    SDDL_DATATYPE_STRING,
    SDDL_DATATYPE_BOOL,
    SDDL_DATATYPE_INT8,
    SDDL_DATATYPE_UINT8,
    SDDL_DATATYPE_INT16,
    SDDL_DATATYPE_UINT16,
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
    SDDL_CONTROL_TYPE_TRIGGER
} SDDLControlTypeEnum;

typedef enum
{
    SDDL_NUMERIC_DISPLAY_HINT_INVALID,
    SDDL_NUMERIC_DISPLAY_HINT_NORMAL,
    SDDL_NUMERIC_DISPLAY_HINT_PERCENTAGE,
    SDDL_NUMERIC_DISPLAY_HINT_SCIENTIFIC,
    SDDL_NUMERIC_DISPLAY_HINT_HEX
} SDDLNumericDisplayHintEnum;
typedef struct SDDLDocument_t * SDDLDocument;

typedef struct SDDLProperty_t * SDDLProperty;

typedef struct SDDLControl_t * SDDLControl;

typedef struct SDDLSensor_t * SDDLSensor;

typedef struct SDDLClass_t * SDDLClass;

typedef struct SDDLParseResult_t * SDDLParseResult;

SDDLParseResult sddl_load_and_parse(const char *filename);
SDDLParseResult sddl_load_and_parse_file(FILE *file);
SDDLParseResult sddl_parse(const char *sddl);

bool sddl_parse_result_ok(SDDLParseResult result);
SDDLDocument sddl_parse_result_document(SDDLParseResult result);
SDDLDocument sddl_parse_result_ref_document(SDDLParseResult result);
unsigned sddl_parse_result_num_errors(SDDLParseResult result);
const char * sddl_parse_result_error(SDDLParseResult result, unsigned index);
unsigned sddl_parse_result_num_warnings(SDDLParseResult result);
const char * sddl_parse_result_warning(SDDLParseResult result, unsigned index);
void sddl_free_parse_result(SDDLParseResult result);

unsigned sddl_document_num_authors(SDDLDocument doc);
const char * sddl_document_author(SDDLDocument doc, unsigned index);
SDDLProperty sddl_document_lookup_property(SDDLDocument doc, const char*propName);
SDDLClass sddl_document_lookup_class(SDDLDocument doc, const char*propName);

unsigned sddl_document_num_properties(SDDLDocument doc);
SDDLProperty sddl_document_property(SDDLDocument doc, unsigned index);

bool sddl_is_control(SDDLProperty prop);
bool sddl_is_sensor(SDDLProperty prop);
bool sddl_is_class(SDDLProperty prop);

SDDLControl SDDL_CONTROL(SDDLProperty prop);
SDDLSensor SDDL_SENSOR(SDDLProperty prop);
SDDLClass SDDL_CLASS(SDDLProperty prop);
#define SDDL_PROPERTY(v) ((SDDLProperty)(v))

const char * sddl_control_name(SDDLControl control);
SDDLControlTypeEnum sddl_control_type(SDDLControl control);
SDDLDatatypeEnum sddl_control_datatype(SDDLControl control);
const char * sddl_control_description(SDDLControl control);
const double * sddl_control_max_value(SDDLControl control);
const double * sddl_control_min_value(SDDLControl control);
SDDLNumericDisplayHintEnum sddl_control_numeric_display_hint(SDDLControl control);
const char * sddl_control_regex(SDDLControl control);
const char * sddl_control_units(SDDLControl control);

void sddl_control_set_extra(SDDLControl control, void *extra);
void * sddl_control_extra(SDDLControl control);

const char * sddl_sensor_name(SDDLSensor sensor);
SDDLDatatypeEnum sddl_sensor_datatype(SDDLSensor sensor);
const char * sddl_sensor_description(SDDLSensor sensor);
const double * sddl_sensor_max_value(SDDLSensor sensor);
const double * sddl_sensor_min_value(SDDLSensor sensor);
SDDLNumericDisplayHintEnum sddl_sensor_numeric_display_hint(SDDLSensor sensor);
const char * sddl_sensor_regex(SDDLSensor sensor);
const char * sddl_sensor_units(SDDLSensor sensor);
void sddl_sensor_set_extra(SDDLSensor sensor, void *extra);
void * sddl_sensor_extra(SDDLSensor sensor);

const char * sddl_class_name(SDDLClass cls);
RedJsonObject sddl_class_json(SDDLClass cls);
unsigned sddl_class_num_authors(SDDLClass cls);
const char * sddl_class_author(SDDLClass cls, unsigned index);
const char * sddl_class_description(SDDLClass cls);
unsigned sddl_class_num_properties(SDDLClass cls);
SDDLProperty sddl_class_property(SDDLClass cls, unsigned index);
SDDLProperty sddl_class_lookup_property(SDDLClass cls, const char *name);
SDDLControl sddl_class_lookup_control(SDDLClass cls, const char *name);
SDDLSensor sddl_class_lookup_sensor(SDDLClass cls, const char *name);

#endif
