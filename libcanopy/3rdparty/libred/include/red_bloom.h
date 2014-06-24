/*
 *  red_bloom.h - Bloom Filter Data Structure
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
#ifndef RED_BLOOM_INCLUDED
#define RED_BLOOM_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*
 * RedBloom datatype -- ADT representing a bloom filter data structure.
 */
typedef struct RedBloom_t * RedBloom;

/*
 * RedBloom_New - Create a new (empty) bloom filter.
 *
 *      <estimatedNumItems> is an estimate of the number of elements that will
 *          be inserted.  Overestimating will result in wasted storage space.
 *          Underestimating will result in additional false positives.
 *
 *      <falsePostiveRate> is the target false positive rate.
 *          For example:
 *              0.01   -- 1% false positives (~9.5 bits per element)
 *              0.001  -- 0.1% false positives (~14.3 bits per element)
 *              0.0001 -- 0.01% false positives (~19.1 bits per element)
 *
 *          The target rate will only be achieved if the number of elements
 *          inserted approximately equals <estimatedNumItems>.
 *
 *      Returns a newly allocated RedBloom object handle, or NULL if memory
 *      allocation failed.
 */
RedBloom RedBloom_New(uint32_t estimatedNumItems, double falsePositiveRate);

/*
 * RedBloom_Free - Free memory used by <bloom>.
 *
 *      Does nothing if <bloom> is NULL.
 */
void RedBloom_Free(RedBloom bloom);

/*
 * RedBloom_Insert - Add element to bloom filter.
 *
 *      <bloom> is the bloom filter object to add to.
 *
 *      <pItem> is a pointer to the item data to insert.  The data can be any
 *          sequence of bytes <itemSize> in length.
 *
 *      <itemSize> is the number of bytes of data that <pItem> points to.
 *
 *      If element has already been added to the bloom filter, this routine has
 *      no effect.
 */
void RedBloom_Insert(RedBloom bloom, const void *pItem, size_t itemSize);

/*
 * RedBloom_InsertS - Add string element to bloom filter.
 *
 *      This is equivalent to:
 *      ----------------------------------------------------------------------
 *      RedBloom_Insert(bloom, szItem, strlen(szItem) + 1)
 *      ----------------------------------------------------------------------
 *
 *      <bloom> is the bloom filter object to add to.
 *
 *      <szItem> is a NULL-terminated string to add.
 *
 *      If element has already been added to the bloom filter, this routine has
 *      no effect.
 */
void RedBloom_InsertS(RedBloom bloom, const char *szItem);

/*
 * RedBloom_MayContain - Determine if bloom filter may contain an item.
 *
 *      <bloom> is the bloom filter object to check.
 *
 *      <pItem> is a pointer to the item data to look for.  The data can be any
 *          sequence of bytes <itemSize> in length.
 *
 *      <itemSize> is the number of bytes of data that <pItem> points to.
 *
 *      Return value:
 *          true -- The bloom filter POSSIBLY CONTAINS the item.
 *          false -- The bloom filter DEFINITELY DOES NOT contain the item.
 */
bool RedBloom_MayContain(RedBloom bloom, const void *pItem, size_t itemSize);

/*
 * RedBloom_MayContainS - Determine if bloom filter may contain a string.
 *
 *      This is equivalent to:
 *      ----------------------------------------------------------------------
 *      RedBloom_MayContain(bloom, szItem, strlen(szItem) + 1)
 *      ----------------------------------------------------------------------
 *
 *      <bloom> is the bloom filter object to check.
 *
 *      <szItem> is a NULL-terminated string to look for.
 *
 *      Return value:
 *          true -- The bloom filter POSSIBLY CONTAINS the item.
 *          false -- The bloom filter DEFINITELY DOES NOT contain the item.
 */
bool RedBloom_MayContainS(RedBloom bloom, const char *szItem);

/* THE FOLLOWING ARE NOT SUPPORTED AT THIS TIME
 * Reason: Concerned about versioning issues, if we change the hash function.
 */
/*
 * RedBloom_DataSizeInBytes - Get the number of bytes used for storage of the
 *      bloom filter data.
 *
 *      The data size of a bloom filter never changes, so it is safe to call
 *      this routine once and store the value for later use.
 *
 *      <bloom> is the bloom filter object.
 */
/*size_t RedBloom_DataSizeInBytes(RedBloom bloom); */
/*
 * RedBloom_WriteDataBytes - Obtain the actual byte data representation of this
 *      bloom filter.
 *
 *      This allows you to, for example, save it disk and load it later.
 *
 *      <bloom> is the bloom filter object.
 *
 *      <out> is the destination memory buffer where the byte data will be
 *          written to.  It must be at least RedBloom_DataSizeInBytes(<bloom>)
 *          bytes large.
 *
 *      NOTE: The written byte data does contain some header metadata.
 *      Therefore, it is not safe to manipulate it (for example, do not attempt
 *      to manually merge bloom tables by ORing the raw data).
 */
/*void RedBloom_WriteDataBytes(RedBloom bloom, uint8_t *out); */

#endif
