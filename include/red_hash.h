/*  
 *  red_hash.h -- Hash Table Interface
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
/*
 *  OVERVIEW
 *
 *      The RED_HASH module provides a key-value memory store with constant-time
 *      lookup by key, where:
 *          - The key can be any block of memory.  The key is fully copied.
 *          - The value is a pointer.  You can also use intptr_t, uintptr_t, or
 *          smaller integer types, by casting to/from (void*), to store small
 *          integer values.  Keep in mind that the integer size for which it is
 *          safe to do this is platform dependent.
 *
 *  PERFORMANCE NOTE
 *
 *      All hash table operations take amortized constant O(1) time, unless
 *      otherwise specified.  However, this O(1) performance is only true for
 *      small hash tables with < ~10 million items, after which point most
 *      single operations will take O(N) time (proportional to the size of the
 *      hash table).
 *
 *  BASIC OPERATIONS
 *
 *      The client is responsible for allocating the value.  For example:
 *
 *          typedef struct
 *          {
 *              float x, y, z, w;
 *          } Quaternion;
 *
 *          ...
 *          pVal = malloc(sizeof(Quaternion));
 *
 *      Create a RED_HASH:
 *
 *          RedHash hash;
 *          hash = RedHash_New(0);
 *
 *      Insert a key-value pair (string key):
 *
 *          RedHash_InsertS(hash, "My Quaternion", pVal);
 *
 *      Lookup by key (string key), defaulting to NULL:
 *
 *          pVal = RedHash_GetWithDefaultS(hash, "My Quaternion", NULL);
 *
 *      For non-string keys, use the RedHash_Insert/RedHash_Get/etc routines
 *      instead of the macros.
 *
 */
#ifndef RED_HASH_INCLUDED
#define RED_HASH_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * RedHash - Represents a hash table.
 */
typedef struct RedHash_t * RedHash;

typedef struct RedHashIterator_t
{
    RedHash _hash;
    unsigned _bucket;
    void *_node;
} RedHashIterator_t;

/*
 * RedHash_New - Create a new (emtpy) hash table and return handle to it.
 *
 *      <numItemsHint> is a hint to the implementation about how many entries
 *          the hash table will store, which can help the implementation choose
 *          an optimal number of buckets.  If unsure, just set to 0.
 */
RedHash RedHash_New(unsigned numItemsHint);

void RedHash_Free(RedHash hash);
/*
 * RedHash_Insert - Insert a key-value pair (general key).
 *
 *      This routine will assert if the key already exists in the hash table.
 *      If you would rather have it update the entry's value, use
 *      RedHash_InsertOrUpdate instead.
 *
 *      <pMap> is the hash table to insert into.
 *
 *      <key> is a pointer to a block of memory which will be copied and used
 *          as the entry's key.
 *
 *      <keySize> is the size of the key in bytes.
 *
 *      <value> is any pointer type, and will be used as the entry's value.
 *          that parameter.
 */
void 
    RedHash_Insert(
            RedHash hash, 
            const void *key, 
            unsigned keySize, 
            void *value);
/*
 * RedHash_InsertS - Insert a key-value pair (null-terminated-string key).
 *
 *      This routine will assert if the key already exists in the hash table.
 *      If you would rather have it update the entry's value, use
 *      RedHash_INSERT_OR_UPDATE instead.
 *
 *      <pMap> is the hash table to insert into.
 *
 *      <key> is a pointer to a null-terminated string which will be copied and
 *          used as the entry's key.
 *
 *      <value> is any pointer type, and will be used as the entry's value.
 *
 *      This macro references <key> twice, so avoid complex expressions for
 *      that parameter.
 */
#define RedHash_InsertS(hash, key, value) \
    RedHash_Insert((hash), (key), strlen(key)+1, (value))


/*
 * RedHash_Get - Get the value associated with a key (general key).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *      If you would rather have it return a default value, use
 *      RedHash_GetWithDefault instead.
 *
 *      <pMap> is the hash table to lookup into.
 *
 *      <key> is a pointer to a block of memory which will be used as the key
 *          for the lookup.
 *
 *      <keySize> is the size of the key in bytes.
 */
void *
    RedHash_Get(
            const RedHash hash, 
            const void *key, 
            unsigned keySize);

/*
 * RedHash_GetS - Get the value associated with a key (null-terminated string key).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *      If you would rather have it return a default value, use
 *      RedHash_GetWithDefault instead.
 *
 *      <pMap> is the hash table to lookup into.
 *
 *      <key> is a pointer to a null-terminated string key to be looked up.
 *
 *      This macro references <key> twice, so avoid complex expressions for
 *      that parameter.
 */
#define RedHash_GetS(hash, key) RedHash_Get((hash), (key), strlen(key)+1)

/*
 * RedHash_GetWithDefault - Get the value associated with a key (general key),
 *      or a default value if that key is not in the hash table.
 *
 *      <pMap> is the hash table to lookup into.
 *
 *      <key> is a pointer to a block of memory which will be used as the key
 *          for the lookup.
 *
 *      <keySize> is the size of the key in bytes.
 *
 *      <defaultValue> is a pointer value to return if the key is not found.
 *          It is common to use NULL here (although NULL is also a valid
 *          key-value value, so, in general, it is not safe to assume that a
 *          returned NULL value (or any default value, for that matter)  means
 *          the key was not found).
 */
void *
    RedHash_GetWithDefault(
            const RedHash hash, 
            const void *key, 
            unsigned keySize,
            void *defaultValue);
/*
 * RedHash_GetWithDefaultS - Get the value associated with a key
 *      (null-terminated string key), or a default value if that key is not in
 *      the hash table.
 *
 *      <pMap> is the hash table to lookup into.
 *
 *      <key> is a pointer to a null-terminated string key to be looked up.
 *
 *      <defaultValue> is a pointer value to return if the key is not found.
 *          It is common to use NULL here (although NULL is also a valid
 *          key-value value, so, in general, it is not safe to assume that a
 *          returned NULL value (or any default value, for that matter)  means
 *          the key was not found).
 *
 *      This macro references <key> twice, so avoid complex expressions for
 *      that parameter.
 */
#define RedHash_GetWithDefaultS(hash, key, defaultValue) \
    RedHash_GetWithDefault((hash), (key), strlen(key)+1, defaultValue)


/*
 * RedHash_Update - Update the value associated with a key (general key).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *      If you would rather have it insert the key-value, use
 *      RedHash_UpdateOrInsert instead.
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a block of memory which will be used as the key
 *          to update.
 *
 *      <keySize> is the size of the key in bytes.
 *
 *      <value> is any pointer type, and will replace the entry's old value.
 *
 *      Returns the old value that was replaced (which is helpful if you need
 *      to free or dereference it).
 */
void *
    RedHash_Update(
            const RedHash hash, 
            const void *key, 
            unsigned keySize,
            void *value);

/*
 * RedHash_UpdateS - Update the value associated with a key (null-terminated
 *      string keys).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *      If you would rather have it insert the key-value, use
 *      RedHash_UPDATE_OR_INSERT instead.
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a null-terminated string key to update.
 *
 *      <value> is any pointer type, and will replace the entry's old value.
 *
 *      Returns the old value that was replaced (which is helpful if you need
 *      to free or dereference it).
 */
#define RedHash_UpdateS(hash, key, value) \
    RedHash_Update((hash), (key), strlen(key)+1, value)


/*
 * RedHash_UpdateOrInsert - Update the value associated with a key, inserting
 *      the value if the key does not exist (generalized key).
 *
 *      <pMap> is the hash table to update.
 *
 *      <replacedValue> is a pointer to a pointer that will be set to the
 *          replaced value if an update occured.  If an insert occured, this
 *          parameter is ignored.  Use the boolean return value to determine
 *          which operation occured.  If <replacedValue> is NULL it is ignored.
 *
 *      <key> is a pointer to a block of memory which will be used as the key
 *          to update.
 *
 *      <keySize> is the size of the key in bytes.
 *
 *      <value> is any pointer type, and will replace the entry's old value.
 *
 *      Returns a boolean, which is TRUE if an update occured or FALSE if an
 *      insert occured.
 */
bool
    RedHash_UpdateOrInsert(
            const RedHash hash,
            void ** replacedValue,
            const void *key, 
            unsigned keySize,
            void *value);

/*
 * RedHash_UpdateOrInsertS - Update the value associated with a key, inserting
 *      the value if the key does not exist (null-terminated string keys).
 *
 *      <pMap> is the hash table to update.
 *
 *      <replacedValue> is a pointer to a pointer that will be set to the
 *          replaced value if an update occured.  If an insert occured, this
 *          parameter is ignored.  Use the boolean return value to determine
 *          which operation occured.  If <replacedValue> is NULL it is ignored.
 *
 *      <key> is a pointer to a null-terminated string key to update.
 *
 *      <value> is any pointer type, and will replace the entry's old value.
 *
 *      Returns a boolean, which is TRUE if an update occured or FALSE if an
 *      insert occured.
 */
#define RedHash_UpdateOrInsertS(hash, replacedValue, key, value) \
    RedHash_UpdateOrInsert(\
            (hash), \
            (replacedValue), \
            (key), \
            strlen(key)+1, \
            value)
/*
 * RedHash_Remove - Remove a key-value pair from hash table (general key).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a block of memory which is the key identifying
 *          which entry to remove.
 *
 *      <keySize> is the size of the key in bytes.
 *
 *      Returns the value for the entry that was removed.
 */
void *
    RedHash_Remove(
            RedHash hash, 
            const void *key, 
            unsigned keySize);

/*
 * RedHash_RemoveS - Remove a key-value pair from hash table (null-terminated
 *      string keys).
 *
 *      This routine will assert if the key does not exist in the hash table.
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a null-terminated string key identifying the
 *          entry to remove.
 *
 *      Returns the value for the entry that was removed.
 */
#define RedHash_RemoveS(hash, key) \
    RedHash_Remove((hash), (key), strlen(key)+1)

/*
 * RedHash_HasKey - Determine if hash table contains a key (general key).
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a null-terminated string key to check.
 *
 *      Returns TRUE if the key was found in <pMap>, FALSE otherwise.
 */
bool 
    RedHash_HasKey(
            const RedHash hash, 
            const void *key, 
            unsigned keySize);

/*
 * RedHash_HasKeyS - Determine if hash table contains a key (null-terminated
 *      string keys).
 *
 *      <pMap> is the hash table to update.
 *
 *      <key> is a pointer to a null-terminated string key to check.
 *
 *      Returns TRUE if the key was found in <pMap>.
 */
#define RedHash_HasKeyS(hash, key) \
    RedHash_HasKey((hash), (key), strlen(key)+1)

/*
 * RedHash_NumItems - Get the number of entries in a hash table.
 *
 *      <pMap> is the hash table to check.
 */
unsigned RedHash_NumItems(const RedHash hash);

/*
 * RedHash_IsEmpty - Determine if hash table is empty
 *
 *      <pMap> is the hash table to check.
 */
bool RedHash_IsEmpty(const RedHash hash);

/*
 * RedHash_Clear - Removes all key-value pairs from a hash table.
 *
 *      <pMap> is the hash table to clear.
 */
void RedHash_Clear(RedHash hash);

/* TODO: document */
void RedHashIterator_Init(RedHashIterator_t *pIter, RedHash hash);

bool RedHashIterator_Advance(RedHashIterator_t *pIter, const void **ppOutKey, size_t *pOutKeySize, const void **ppOutValue);

#define RED_HASH_FOREACH(iter, hash, ppOutKey, pOutKeySize, ppOutValue) \
    for (RedHashIterator_Init(&(iter), (hash)); RedHashIterator_Advance(&(iter), (ppOutKey), (pOutKeySize), (ppOutValue)); )

#ifdef __cplusplus
}
#endif

#endif
