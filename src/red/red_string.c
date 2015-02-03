/*
 * string.c -- Copyright 2009-2013 Greg Prisament
 *
 * Implements string interface.
 */

#include "red_string.h"
#include "zarray.h"
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* TODO: is strlen correct with unicode characters? */
#define _MIN(x, y) ((x) < (y) ? (x) : (y))
#define _MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct RedString_t
{
    // data consists of length chars followed by null-terminator.
    char *data;
    unsigned length; /* in bytes? characters? */
} RedString_t; 

typedef struct RedStringList_t
{
    ZARRAY(RedString) array;
} RedStringList_t;

RedString RedString_New(const char *src)
{
    RedString hNew = malloc(sizeof(RedString_t));
    hNew->length = src ? strlen(src) : 0;
    hNew->data = malloc(hNew->length + 1);
    if (src)
        strncpy(hNew->data, src, hNew->length);
    hNew->data[hNew->length] = 0;
    return hNew;
}

RedString RedString_NewLength(const char *src, unsigned length)
{
    RedString hNew = malloc(sizeof(RedString_t));
    hNew->length = src ? _MIN(length, strlen(src)) : 0;
    hNew->data = malloc(hNew->length + 1);
    if (src)
        strncpy(hNew->data, src, hNew->length);
    hNew->data[hNew->length] = 0;
    return hNew;
}

RedString RedString_NewPrintf(const char *fmt, unsigned size, ...)
{
    char * tmpResult;
    va_list args;
    RedString hNew;

    tmpResult = malloc(size+1);

    va_start(args, size);
    vsnprintf(tmpResult, (size_t)(size + 1), fmt, args);
    va_end(args);

    hNew = RedString_New(tmpResult);

    free(tmpResult);

    return hNew;
}

char *RedString_PrintfToNewChars(const char *fmt, ...)
{
    va_list arg;
    int len;
    char *out;

    va_start(arg, fmt);
    len = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);

    out = (char *)malloc((len+1)*sizeof(char));
    if (!out)
    {
        return NULL;
    }

    va_start(arg, fmt);
    vsnprintf(out, len+1, fmt, arg);
    va_end(arg);

    return out;
}

void RedString_Free(RedString s)
{
    if (s)
    {
        free(s->data);
        free(s);
    }
}

void RedString_Set(RedString hOut, const char *in)
{
    free(hOut->data);
    hOut->length = in ? strlen(in) : 0;
    hOut->data = malloc(hOut->length + 1);
    if (in)
        strcpy(hOut->data, in);
    hOut->data[hOut->length] = 0;
}

void RedString_Clear(RedString hOut)
{
    RedString_Set(hOut, "");
}

unsigned RedString_Length(const RedString hRedString)
{
    return hRedString->length;
}

/* TODO: unicode support */
unsigned RedString_Bytes(const RedString hRedString)
{
    return hRedString->length + 1;
}

const char * RedString_GetChars(const RedString hRedString)
{
    return hRedString->data;
}

void RedString_Copy(RedString hResult, const RedString hSrc)
{
    if (hSrc == hResult)
        return;

    free(hResult->data);
    hResult->length = hSrc->length;
    hResult->data = malloc(hSrc->length+1);
    strncpy(hResult->data, hSrc->data, hResult->length);
    hResult->data[hResult->length] = 0;
}

void RedString_CaseConvert(RedString hRedString, RedStringCase newCase)
{
    unsigned i;
    switch (newCase)
    {
        case STRING_CASE_UPPER:
        {
            for (i = 0; i < hRedString->length; i++)
                hRedString->data[i] = toupper(hRedString->data[i]);
            break;
        }
        case STRING_CASE_LOWER:
        {
            for (i = 0; i < hRedString->length; i++)
                hRedString->data[i] = tolower(hRedString->data[i]);
            break;
        }
        default:
        {
            assert(0);
        }
    }
}

int RedString_Compare(
        const RedString hRedStringA, 
        const RedString hRedStringB,
        unsigned flags)
{
    // not supported yet
    assert(!(flags & STRING_COMPARE_IGNORE_WHITESPACE_FLAG));
    
    if (flags & STRING_COMPARE_IGNORE_CASE_FLAG)
    {
        return strcasecmp(hRedStringA->data, hRedStringB->data);
    }
    return strcmp(hRedStringA->data, hRedStringB->data);
}

bool RedString_Equals(
    RedString hRedString,
    const char * otherString)
{
    return 
        !RedString_CompareChars(
            RedString_GetChars(hRedString),
            otherString,
            0);
}

char *RedString_strdup(const char *src)
{
    size_t len = strlen(src);
    char *out = malloc(len*sizeof(char) + 1);
    if (!out)
        return NULL;
    strcpy(out, src);
    out[len] = 0;
    return out;
}

int RedString_CompareChars(
        const char * strA, 
        const char * strB,
        unsigned flags )
{
    // not supported yet
    assert(!(flags & STRING_COMPARE_IGNORE_WHITESPACE_FLAG));
    
    if (flags & STRING_COMPARE_IGNORE_CASE_FLAG)
    {
        return strcasecmp(strA, strB);
    }
    return strcmp(strA, strB);
}

bool RedString_ContainsChars(ConstRedString hHaystack, const char *needle)
{
    if (strstr(hHaystack->data, needle))
    {
        return true;
    }
    return false;
}

bool RedString_BeginsWith(ConstRedString hRedString, const char *needle)
{
    if (!memcmp(hRedString->data, needle, strlen(needle)))
    {
        return true;
    }
    return false;
}

int RedString_Search(ConstRedString hHaystack, const char c)
{
    char *p;
    char needle[2] = {c, 0};
    p = strstr(hHaystack->data, needle);
    if (!p)
    {
        return -1;
    }
    return p - hHaystack->data;
}

float RedString_RemoveFloat(RedString hRedString)
{
    float out;
    char *endptr;
    
    out = strtof(hRedString->data, &endptr);
    RedString_SubString(hRedString, hRedString, endptr - hRedString->data, -1);

    return out;
}

char RedString_FirstNonWhitespaceChar(ConstRedString hRedString)
{
    char *s = (char *)hRedString->data;

    while (*s && isspace(*s))
        s++;

    return *s;

}

void RedString_Trim(RedString hRedString)
{
    char *s = (char *)hRedString->data;
    char *e = (char *)&hRedString->data[hRedString->length - 1];

    while (*s && isspace(*s))
        s++;

    while (*e && isspace(*e))
        e--;

    RedString_SubString(hRedString, hRedString, s - hRedString->data, e - hRedString->data);
}

void RedString_SubString(
        RedString hResult, 
        const RedString hSrc, 
        int start, 
        int end)
{
    char *newData;

    if (start < 0)
        start = (int)hSrc->length + start;

    if (end < 0)
        end = (int)hSrc->length + end;

    start = _MIN(_MAX(start, 0), hSrc->length - 1);
    end = _MIN(_MAX(end, 0), hSrc->length - 1);

    if (end < start)
    {
        newData = malloc(1);
        newData[0] = 0;
        free(hResult->data);
        hResult->data = newData;
        hResult->length = 0;
    }
    else
    {
        newData = malloc(end - start + 2);
        memcpy(newData, &hSrc->data[start], end - start + 1);
        newData[end - start + 1] = 0;
        free(hResult->data);
        hResult->data = newData;
        hResult->length = end - start + 1;
    }
    return;
}

unsigned RedString_Tounsigned(ConstRedString hRedString)
{
    return (unsigned)atoi(hRedString->data);
}

void RedString_Append(RedString hOriginal, const RedString hAppend)
{
    unsigned sumLength;
    sumLength = hOriginal->length + hAppend->length;
    hOriginal->data = realloc(hOriginal->data, sumLength + 1);
    memcpy(&hOriginal->data[hOriginal->length], hAppend->data, hAppend->length);
    hOriginal->data[sumLength] = 0;
    hOriginal->length = sumLength;
}

void RedString_AppendChars(RedString hOriginal, const char *pAppend)
{
    unsigned sumLength;
    sumLength = hOriginal->length + strlen(pAppend);
    hOriginal->data = realloc(hOriginal->data, sumLength + 1);
    memcpy(&hOriginal->data[hOriginal->length], pAppend, strlen(pAppend));
    hOriginal->data[sumLength] = 0;
    hOriginal->length = sumLength;
}

void RedString_AppendPrintf(RedString hOriginal, const char *fmt, unsigned size, ...)
{
    char * tmpResult;
    va_list args;

    tmpResult = malloc(size+1);

    va_start(args, size);
    vsnprintf(tmpResult, (size_t)size, fmt, args);
    va_end(args);

    RedString_AppendChars(hOriginal, tmpResult);

    free(tmpResult);
}

void RedString_RemoveToChar(RedString hRedString, char c)
{
    char *s = (char *)hRedString->data;

    while (*s && (*s != c))
        s++;

    s++;
    RedString_SubString(hRedString, hRedString, s - hRedString->data, -1);
}

static uint64_t ror_13_bits(uint64_t in)
{
    uint64_t bit = in & 0x1fff;
    uint64_t out;
    in >>= 13;
    out = in | (bit << (64 - 13));
    return out;
    
}

void RedString_Hash(RedString hResult, ConstRedString hSrc)
{
    uint64_t sum = 0x1234567801234567ULL;
    unsigned i, j;
    char result[13];

    for (j = 0; j < 50; j++)
    {
        for (i = 0; i < hSrc->length; i++)
        {
            sum += (uint64_t)((uint8_t)hSrc->data[i] ^ (uint8_t)i);
            sum = ror_13_bits(sum);
        }
    }

    for (i = 0; i < 13; i++)
    {
        result[i] = (char)(sum & 0x1f);
        if (result[i] < 26)
            result[i] += 'a';
        else 
            result[i] = result[i] + '0' - 26;
        sum >>= 5;
    }

    hResult->data = realloc(hResult->data, 14);
    memcpy(hResult->data, result, 13);
    hResult->data[13] = 0;
    hResult->length = 13;
}

void RedString_Rot13(RedString hResult, RedString hOriginal)
{
    unsigned i;
    char *newData;
    newData = malloc(hOriginal->length);
    for (i = 0; i < hOriginal->length; i++)
    {
        if (isupper(hOriginal->data[i]))
        {
            newData[i] = (((hOriginal->data[i] - 'A') + 13) % 26) + 'A';
        }
        if (islower(hOriginal->data[i]))
        {
            newData[i] = (((hOriginal->data[i] - 'a') + 13) % 26) + 'a';
        }
        else
        {
            newData[i] = hOriginal->data[i];
        }
    }
    free(hResult->data);
    hResult->length = hOriginal->length;
    hResult->data = malloc(hOriginal->length+1);
    strncpy(hResult->data, newData, hResult->length);
    hResult->data[hResult->length] = 0;
    free(newData);
}

RedStringList RedString_Split(RedString hRedString, char delimiter)
{
    char *start = hRedString->data;
    char *end = start;
    RedString hNewRedString;
    RedStringList hNew;

    hNew = malloc(sizeof(RedStringList));

    hNew->array = ZARRAY_NEW(RedString, 0);

    while (*end)
    {
        if (*end == delimiter)
        {
            hNewRedString = RedString_NewLength(start, end - start);
            ZARRAY_APPEND(hNew->array, hNewRedString);
            start = end + 1;
        }
        end++;
    }
    hNewRedString = RedString_NewLength(start, end - start);
    ZARRAY_APPEND(hNew->array, hNewRedString);

    return hNew;
}

RedStringList RedString_SplitChars(const char *chars, char delimiter)
{
    RedString s = RedString_New(chars);
    RedStringList out;
    out = RedString_Split(s, delimiter);
    RedString_Free(s);
    return out;
}

unsigned RedStringList_NumStrings(RedStringList hList)
{
    return ZARRAY_NUM_ITEMS(hList->array);
}

RedString RedStringList_GetString(RedStringList hList, unsigned idx)
{
    return ZARRAY_AT(hList->array, idx);
}

const char * RedStringList_GetStringChars(RedStringList hList, unsigned idx)
{
    return RedString_GetChars(ZARRAY_AT(hList->array, idx));
}

void RedStringList_Join(RedString hString, RedStringList hList, const char *joiner)
{
    /* TODO: optimize */
    int numItems = ZARRAY_NUM_ITEMS(hList->array);
    int i;
    RedString_Clear(hString);
    for (i = 0; i < numItems; i++)
    {
        RedString_Append(hString, ZARRAY_AT(hList->array, i));
        if (joiner && (i < numItems - 1))
            RedString_AppendChars(hString, joiner);
    }
}

char * RedStringList_JoinToNewChars(RedStringList list, const char *joiner)
{
    RedString joined = RedString_New(NULL);
    char *out;
    RedStringList_Join(joined, list, joiner);
    out = RedString_ToNewChars(joined);
    RedString_Free(joined);
    return out;
}

RedStringList RedStringList_New()
{
    RedStringList newList;
    newList = malloc(sizeof(RedStringList));
    newList->array = ZARRAY_NEW(RedString, 0);
    return newList;
}

void RedStringList_Free(RedStringList list)
{
    unsigned numRedStrings = ZARRAY_NUM_ITEMS(list->array);
    RedString hRedString;
    unsigned i;

    for (i = 0; i < numRedStrings; i++)
    {
        hRedString = ZARRAY_AT(list->array, i);
        RedString_Free(hRedString);
    }
    ZARRAY_FREE(list->array);
    free(list);
}

void RedStringList_AppendChars(RedStringList list, const char *chars)
{
    RedString newString;
    newString = RedString_New(chars);
    ZARRAY_APPEND(list->array, newString);
}

void RedStringList_AppendPrintf(RedStringList list, const char *fmt, ...)
{
    char * tmpResult;
    va_list args;
    int len;
    RedString newString;

    va_start(args, fmt);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    tmpResult = malloc(len+1);

    va_start(args, fmt);
    vsprintf(tmpResult, fmt, args);
    va_end(args);

    newString = RedString_New(tmpResult);

    ZARRAY_APPEND(list->array, newString);
    free(tmpResult);
}

char *RedString_ToNewChars(RedString s)
{
    char *out = calloc(1, s->length+1);
    strncpy(out, s->data, s->length);
    return out;
}

char * RedStringList_ToNewChars(RedStringList hList)
{
    RedString s;
    char *out;
    s = RedString_New(NULL);
    RedStringList_Join(s, hList, NULL);
    out = RedString_ToNewChars(s);
    RedString_Free(s);
    return out;
}
