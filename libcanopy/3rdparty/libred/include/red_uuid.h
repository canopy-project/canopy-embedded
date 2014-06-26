/*
 *  red_uuid.h - UUID library
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
#ifndef RED_UUID
#define RED_UUID

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct RedUUID_t
{
    uint8_t v[16];
} RedUUID_t;

/*
 * Generate a random UUID.
 */
RedUUID_t RedUUID_GenV4();

/*
 * Load a UUID value from string.
 *
 * Returns true on success, false otherwise.
 */
bool RedUUID_FromChars(RedUUID_t *dest, const char *src);

/*
 * Check if a UUID is version 4 (random)
 */
bool RedUUID_IsV4(RedUUID_t uuid);

/*
 * Convert UUID to human-readable string.  Caller must free returned string.
 */
char *RedUUID_ToNewString(RedUUID_t uuid);
#endif
