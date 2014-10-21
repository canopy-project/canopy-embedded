#include "red_hash.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct RedHashNodeHeader
{
    struct RedHashNodeHeader *next;
    void *value;
    unsigned keySize;
    char keyStart; /* First byte of key */
} RedHashNodeHeader;

typedef struct RedHash_t
{
    unsigned sizeLevel;
    unsigned numEntries;
    unsigned numBuckets;
    RedHashNodeHeader ** buckets;
} RedHash_t;

#define _REDHASH_NODE_KEY(pnode) (&((pnode)->keyStart))

static const unsigned _RedHashValidBucketCounts[] =
{
    /* primes near powers of 2 */
    23,
    509,
    1021,
    2053,
    4093,
    8191,
    16301,
    32771,
    65521,
    131071,
    256049,
    512671,
    1281101,
    2562317,
    5194069,
    10991719  
};
#define _REDHASH_NUM_SIZE_LEVELS (sizeof(_RedHashValidBucketCounts) / sizeof(unsigned))

static unsigned _RedHash_Hash(const void *keyobj, size_t keylen, unsigned numBuckets) {
    /* Jenkins algoritm */
    char * key = (char *)keyobj;
    unsigned hash = 0;
    unsigned i;
    for (i = 0; i < keylen; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % numBuckets;
}

static bool _RedHash_KeysMatch(unsigned size1, const void *key1, unsigned size2, const void *key2)
{
    return ((size1 == size2) && !strncmp(key1, key2, size1)) ? true: false;
}


RedHash RedHash_New(unsigned numItemsHint)
{
    RedHash hNew;
    int i;
    hNew = malloc(sizeof(RedHash_t));
    hNew->numEntries = 0;
    i = 0;
    do
    {
        hNew->numBuckets = _RedHashValidBucketCounts[i];
        i++;
    } while ((i < _REDHASH_NUM_SIZE_LEVELS) &&
             (hNew->numBuckets < numItemsHint));
    hNew->buckets = calloc(hNew->numBuckets, sizeof(RedHashNodeHeader *));
    return hNew;
}

static void _RedHash_AutoResize(RedHash hash)
{
    RedHash_t oldHash;
    RedHashNodeHeader *pNode;
    unsigned i;

    /* Do we need to do anything? */
    if (hash->numEntries < hash->numBuckets)
        return;

    /* Do nothing if we've reached the maximum number of buckets */
    if (hash->sizeLevel >= _REDHASH_NUM_SIZE_LEVELS-1)
        return;

    /* Increase the number of buckets */
    memcpy(&oldHash, hash, sizeof(RedHash_t));
    hash->sizeLevel++;
    hash->numBuckets = _RedHashValidBucketCounts[hash->sizeLevel];

    /* Create larger bucket array */
    hash->buckets = calloc(hash->numBuckets, sizeof(RedHashNodeHeader *));

    /* Move nodes to new array */
    for (i = 0; i < oldHash.numBuckets; i++)
    {
        pNode = oldHash.buckets[i];
        while (pNode) {
            unsigned newhashval;

            /* remove from old bucket */
            oldHash.buckets[i] = pNode->next;

            /* rehash */
            newhashval = _RedHash_Hash(&pNode->keyStart, pNode->keySize, hash->numBuckets);
            pNode->next = hash->buckets[newhashval];
            hash->buckets[newhashval] = pNode;

            pNode = oldHash.buckets[i];
        }
    }
}

void 
    RedHash_Insert(
            RedHash hash, 
            const void *key, 
            unsigned keySize, 
            void *value)
{
    unsigned hashval;
    RedHashNodeHeader *pNewNode;
    RedHashNodeHeader *tmp;

    assert(!RedHash_HasKey(hash, key, keySize));
    assert(keySize > 0);

    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    tmp = hash->buckets[hashval];

    pNewNode = malloc(sizeof(RedHashNodeHeader) + keySize-1);
    pNewNode->next = tmp;
    pNewNode->value = value;
    pNewNode->keySize = keySize;
    memcpy(&pNewNode->keyStart, key, keySize);

    hash->buckets[hashval] = pNewNode;
    hash->numEntries++;

    _RedHash_AutoResize(hash);
}

void *
    RedHash_Get(
            const RedHash hash, 
            const void *key, 
            unsigned keySize)
{
    RedHashNodeHeader *pNode;
    unsigned hashval;
    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    pNode = hash->buckets[hashval];
    while (pNode)
    {
        if (_RedHash_KeysMatch(
                    pNode->keySize, _REDHASH_NODE_KEY(pNode), keySize, key))
            return pNode->value;
        pNode = pNode->next;
    };
    assert(!"RedHash_Get: key not found");
}

void *
    RedHash_GetWithDefault(
            const RedHash hash, 
            const void *key, 
            unsigned keySize,
            void *defaultValue)
{
    RedHashNodeHeader *pNode;
    unsigned hashval;
    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    pNode = hash->buckets[hashval];
    while (pNode)
    {
        if (_RedHash_KeysMatch(
                    pNode->keySize, _REDHASH_NODE_KEY(pNode), keySize, key))
            return pNode->value;
        pNode = pNode->next;
    };
    return defaultValue;
}

void *
    RedHash_Update(
            const RedHash hash, 
            const void *key, 
            unsigned keySize,
            void *value)
{
    RedHashNodeHeader *pNode;
    void *oldValue;
    unsigned hashval;
    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    pNode = hash->buckets[hashval];
    while (pNode)
    {
        if (_RedHash_KeysMatch(
                    pNode->keySize, _REDHASH_NODE_KEY(pNode), keySize, key))
        {
            oldValue = pNode->value;
            pNode->value = value;
            return oldValue;
        }
        pNode = pNode->next;
    };
    assert(!"RedHash_Update: key not found");
}

bool
    RedHash_UpdateOrInsert(
            const RedHash hash, 
            void **replacedValue,
            const void *key, 
            unsigned keySize,
            void *value)
{
    RedHashNodeHeader *pNode;
    void *oldValue;
    unsigned hashval;
    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    pNode = hash->buckets[hashval];
    while (pNode)
    {
        if (_RedHash_KeysMatch(
                    pNode->keySize, _REDHASH_NODE_KEY(pNode), keySize, key))
        {
            oldValue = pNode->value;
            pNode->value = value;
            *replacedValue = oldValue;
            return true;
        }
        pNode = pNode->next;
    };
    /* Key not found, do insert */
    RedHash_Insert(hash, key, keySize, value);
    return false;
}

bool RedHash_HasKey(const RedHash hash, const void *key, unsigned keySize)
{
    RedHashNodeHeader *pNode;
    unsigned hashval;

    hashval = _RedHash_Hash(key, keySize, hash->numBuckets);
    pNode = hash->buckets[hashval];
    while (pNode)
    {
        if (_RedHash_KeysMatch(
                    pNode->keySize, _REDHASH_NODE_KEY(pNode), keySize, key))
            return true;
        pNode = pNode->next;
    };
    return false;
}

unsigned RedHash_NumItems(const RedHash hash)
{
    return hash->numEntries;
}

void RedHash_Clear(RedHash hash)
{
    hash->numEntries = 0;
    hash->numBuckets = _RedHashValidBucketCounts[0];
    // TODO: major mem leak!
    hash->buckets = calloc(hash->numBuckets, sizeof(RedHashNodeHeader *));
}


bool RedHash_IsEmpty(const RedHash hash)
{
    return (hash->numEntries == 0);
}


static void _RedHashIterator_Advance(RedHashIterator_t *pIter)
{
    RedHashNodeHeader *node = pIter->_node;
    if (node->next)
    {
        pIter->_node = node->next;
        return;
    }

    pIter->_bucket++;
    while (pIter->_bucket < pIter->_hash->numBuckets)
    {
        if (pIter->_hash->buckets[pIter->_bucket] != NULL)
        {
            pIter->_node = pIter->_hash->buckets[pIter->_bucket];
            return;
        }
        pIter->_bucket++;
    }
    pIter->_node = NULL;
}

void RedHashIterator_Init(RedHashIterator_t *pIter, RedHash hash)
{
    pIter->_hash = hash; /* TODO: reference? */
    pIter->_bucket = 0;
    pIter->_node = NULL;

    while (pIter->_bucket < pIter->_hash->numBuckets)
    {
        if (pIter->_hash->buckets[pIter->_bucket] != NULL)
        {
            pIter->_node = pIter->_hash->buckets[pIter->_bucket];
            return;
        }
        pIter->_bucket++;
    }
}

bool RedHashIterator_Advance(RedHashIterator_t *pIter, const void **ppOutKey, size_t *pOutKeySize, const void **ppOutValue)
{
    RedHashNodeHeader *node = pIter->_node;
    if (!node)
    {
        return false;
    }
    *ppOutKey = &node->keyStart;
    *pOutKeySize = node->keySize;
    *ppOutValue = node->value;

    _RedHashIterator_Advance(pIter);
    return true;
}
