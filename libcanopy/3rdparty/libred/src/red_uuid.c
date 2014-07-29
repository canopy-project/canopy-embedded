/*
 *  red_uuid.c - UUID library
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
#include "red_uuid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static bool _uuid_initialized = false;

RedUUID_t RedUUID_GenV4()
{
    RedUUID_t out;
    int i;
    if (!_uuid_initialized)
    {
        struct timeval t1;
        gettimeofday(&t1, NULL);
        srand(t1.tv_usec + 1000000 * t1.tv_sec);
        _uuid_initialized = true;
    }
    for (i = 0; i < 16; i++)
    {
        out.v[i] = (rand() >> 8) & 0xFF;
    }
    out.v[6] = (out.v[6] & 0x0F) | 0x40; /* Version */
    out.v[8] = (out.v[8] & 0x3F) | 0x80; /* Reserved bits */
    return out;
}

/*
 * Check if a UUID is version 4 (random)
 */
bool RedUUID_IsV4(RedUUID_t uuid)
{
    return ((uuid.v[6] & 0xF0) == 0x40);
}

bool _hex_to_byte(uint8_t *out, char h)
{
    if (h >= '0' && h <= '9')
    {
        *out = h - '0';
        return true;
    }
    else if (h >= 'a' && h <= 'f')
    {
        *out = h - 'a' + 10;
        return true;
    }
    else if (h >= 'A' && h <= 'F')
    {
        *out = h - 'A' + 10;
        return true;
    }
    return false;
}

static bool _hex_pair_to_byte(uint8_t *out, char h, char l)
{
    uint8_t high, low;
    bool result;
    result = _hex_to_byte(&high, h);
    if (!result)
        return false;
    result = _hex_to_byte(&low, l);
    if (!result)
        return false;
    *out = (high << 4) + low;
    return true;
}

/*
 * Load a UUID value from string.
 *
 * Returns true on success, false otherwise.
 */
bool RedUUID_FromChars(RedUUID_t *dest, const char *src)
{
    int startIndices[16] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};
    int i;
    bool result = true;
    if (strlen(src) != 36)
        return false;
    if ((src[8] != '-') || (src[13] != '-') || (src[18] != '-') || (src[23] != '-'))
        return false;
    for (i = 0; i < 16; i++)
    {
        result &= _hex_pair_to_byte(&dest->v[i], src[startIndices[i]], src[startIndices[i]+1]);
    }
    return result;
}

static char _high_nibble_to_char(uint8_t v)
{
    v = (v >> 4) & 0xF;
    if (v <= 9)
        return '0' + v;
    return 'a' + (v - 10);
}

static char _low_nibble_to_char(uint8_t v)
{
    v &= 0xF;
    if (v <= 9)
        return '0' + v;
    return 'a' + (v - 10);
}
/*
 * Convert UUID to human-readable string.  Caller must free returned string.
 */
char *RedUUID_ToNewString(RedUUID_t uuid)
{
    int i;
    int startIndices[16] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};
    char *out = calloc(37, sizeof(char));
    for (i = 0; i < 16; i++)
    {
        out[startIndices[i]] = _high_nibble_to_char(uuid.v[i]);
        out[startIndices[i]+1] = _low_nibble_to_char(uuid.v[i]);
    }
    out[8] = out[13] = out[18] = out[23] = '-';
    return out;
}
