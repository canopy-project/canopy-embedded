/*
 *  red_json.h - JSON parsing/encoding
 *
 *  Author: Gregory Prsiament (greg@toruslabs.com)
 *
 *  ===========================================================================
 *  Creative Commons CC0 1.0 Universal - Public Domain 
 *
 *  To the extent possible under law, Gregory Prisament has waived all
 *  copyright and related or neighboring rights to RedTest. This work is
 *  published from: United States. 
 *
 *  For details please refer to either:
 *      - http://creativecommons.org/publicdomain/zero/1.0/legalcode
 *      - The LICENSE file in this directory, if present.
 *  ===========================================================================
 */
#ifndef RED_JSON_INCLUDED
#define RED_JSON_INCLUDED

#include <stdbool.h>

typedef enum
{
    RED_JSON_VALUE_TYPE_INVALID,
    RED_JSON_VALUE_TYPE_STRING,
    RED_JSON_VALUE_TYPE_NUMBER,
    RED_JSON_VALUE_TYPE_OBJECT,
    RED_JSON_VALUE_TYPE_ARRAY,
    RED_JSON_VALUE_TYPE_BOOLEAN,
    RED_JSON_VALUE_TYPE_NULL,
} RedJsonValueTypeEnum;

typedef struct RedJsonObject_t * RedJsonObject;

typedef struct RedJsonArray_t * RedJsonArray;

typedef struct RedJsonValue_t * RedJsonValue;

RedJsonValue RedJsonValue_FromString(char * sz); /* String is copied */
RedJsonValue RedJsonValue_FromNumber(double val);
RedJsonValue RedJsonValue_FromObject(RedJsonObject jsonObj);
RedJsonValue RedJsonValue_FromArray(RedJsonArray jsonArray);
RedJsonValue RedJsonValue_FromBoolean(bool val);
RedJsonValue RedJsonValue_Null();

char * RedJsonValue_GetString(RedJsonValue jsonVal); /* String is copied */
double RedJsonValue_GetNumber(RedJsonValue jsonVal);
RedJsonObject RedJsonValue_GetObject(RedJsonValue jsonVal);
RedJsonArray RedJsonValue_GetArray(RedJsonValue jsonVal);

bool RedJsonValue_IsString(RedJsonValue jsonVal);
bool RedJsonValue_IsNumber(RedJsonValue jsonVal);
bool RedJsonValue_IsObject(RedJsonValue jsonVal);
bool RedJsonValue_IsArray(RedJsonValue jsonVal);
bool RedJsonValue_IsNull(RedJsonValue jsonVal);

RedJsonObject RedJsonObject_New();

void RedJsonObject_Set(RedJsonObject jsonObj, const char * szKey, RedJsonValue jsonVal);
void RedJsonObject_SetNull(RedJsonObject jsonObj, const char * szKey);
void RedJsonObject_SetString(RedJsonObject jsonObj, const char * szKey, char *szVal);
void RedJsonObject_SetNumber(RedJsonObject jsonObj, const char * szKey, double val);
void RedJsonObject_SetObject(RedJsonObject jsonObj, const char * szKey, RedJsonObject jsonVal);
void RedJsonObject_SetArray(RedJsonObject jsonObj, const char * szKey, RedJsonArray jsonArray);
void RedJsonObject_SetBoolean(RedJsonObject jsonObj, const char * szKey, bool val);

RedJsonValue RedJsonObject_Get(RedJsonObject jsonObj, const char * szKey);
RedJsonValueTypeEnum RedJsonObject_GetType(RedJsonObject jsonObj, const char * szKey);
char * RedJsonObject_GetString(RedJsonObject jsonObj, const char * szKey);
double RedJsonObject_GetNumber(RedJsonObject jsonObj, const char * szKey);
RedJsonObject RedJsonObject_GetObject(RedJsonObject jsonObj, const char * szKey);
RedJsonArray RedJsonObject_GetArray(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_GetBoolean(RedJsonObject jsonObj, const char * szKey);

bool RedJsonObject_IsValueString(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_IsValueNumber(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_IsValueObject(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_IsValueArray(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_IsValueBoolean(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_IsValueNull(RedJsonObject jsonObj, const char * szKey);

void RedJsonObject_Unset(RedJsonObject jsonObj, const char * szKey);
bool RedJsonObject_HasKey(RedJsonObject jsonObj, const char * szKey);

unsigned RedJsonObject_NumItems(RedJsonObject jsonObj);
char ** RedJsonObject_NewKeysArray(RedJsonObject jsonObj);
void RedJsonObject_FreeKeysArray(char **keysArray);

RedJsonArray RedJsonArray_New();

unsigned RedJsonArray_NumItems(RedJsonArray hArray);

void RedJsonArray_Append(RedJsonArray jsonArray, RedJsonValue val);
void RedJsonArray_AppendString(RedJsonArray jsonArray, char * val);
void RedJsonArray_AppendNumber(RedJsonArray jsonArray, double val);
void RedJsonArray_AppendObject(RedJsonArray jsonArray, RedJsonObject val);
void RedJsonArray_AppendArray(RedJsonArray jsonArray, RedJsonArray val);
void RedJsonArray_AppendBoolean(RedJsonArray jsonArray, bool val);
void RedJsonArray_AppendNull(RedJsonArray jsonArray);

void RedJsonArray_SetEntry(RedJsonArray jsonArray, unsigned idx, RedJsonValue val);
void RedJsonArray_SetEntryString(RedJsonArray jsonArray, unsigned idx, char * val);
void RedJsonArray_SetEntryNumber(RedJsonArray jsonArray, unsigned idx, double val);
void RedJsonArray_SetEntryObject(RedJsonArray jsonArray, unsigned idx, RedJsonObject val);
void RedJsonArray_SetEntryArray(RedJsonArray jsonArray, unsigned idx, RedJsonArray val);
void RedJsonArray_SetEntryBoolean(RedJsonArray jsonArray, unsigned idx, bool val);
void RedJsonArray_SetEntryNull(RedJsonArray jsonArray, unsigned idx);

RedJsonValue RedJsonArray_GetEntry(RedJsonArray jsonArray, unsigned idx);
char * RedJsonArray_GetEntryString(RedJsonArray jsonArray, unsigned idx);
double RedJsonArray_GetEntryNumber(RedJsonArray jsonArray, unsigned idx);
RedJsonObject RedJsonArray_GetEntryObject(RedJsonArray jsonArray, unsigned idx);
RedJsonArray RedJsonArray_GetEntryArray(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_GetEntryBoolean(RedJsonArray jsonArray, unsigned idx);

bool RedJsonArray_IsEntryString(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_IsEntryNumber(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_IsEntryObject(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_IsEntryArray(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_IsEntryBoolean(RedJsonArray jsonArray, unsigned idx);
bool RedJsonArray_IsEntryNull(RedJsonArray jsonArray, unsigned idx);

char * RedJsonValue_ToJsonString(RedJsonValue jsonVal);
char * RedJsonObject_ToJsonString(RedJsonObject jsonObj);

/* TODO: Error reporting */
RedJsonObject RedJson_Parse(const char *text);
#endif
