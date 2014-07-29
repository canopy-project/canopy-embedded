#include "red_bloom.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

struct RedBloom_t
{
    unsigned numHashes;
    size_t dataSizeInBytes;
    uint8_t *data;
};

uint32_t _ComputeHash(
        uint32_t hashNumber, 
        const void *keyObj, 
        size_t keySize, 
        uint32_t numBuckets)
{
    /* TODO: more efficient generation of multiple hashes */
    /* Jenkins algoritm */
    const char * key = (const char *)keyObj;
    uint32_t hash = 0;
    size_t i;
    hash += hashNumber;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    for (i = 0; i < keySize; i++)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % numBuckets;
}

RedBloom RedBloom_New(uint32_t estimatedNumItems, double falsePositiveRate)
{
    /* 
     * From Wikipedia:
     *  m = -(n ln(p))/((ln 2)^2)
     *  k = (m/n) ln(2)
     *
     *  m := number of bits
     *  k := number of hash functions
     */
    RedBloom bloom;

    bloom = malloc(sizeof(struct RedBloom_t));
    if (!bloom)
        return NULL;

    bloom->dataSizeInBytes = 
        (size_t)(ceil(-(estimatedNumItems*logf(falsePositiveRate))/(logf(2.0f)*logf(2.0f))) + 7) / 8;
    bloom->numHashes = 
        (unsigned int)ceil(((float)8*bloom->dataSizeInBytes/(float)estimatedNumItems)*logf(2.0f));
    bloom->data = calloc(bloom->dataSizeInBytes, 1);
    if (!bloom->data)
    {
        free(bloom);
        return NULL;
    }
    return bloom;
}

void RedBloom_Free(RedBloom bloom)
{
    if (bloom)
    {
        free(bloom->data);
        free(bloom);
    }
}

void RedBloom_Insert(RedBloom bloom, const void *pItem, size_t itemSize)
{
    unsigned i;
    for (i = 0; i < bloom->numHashes; i++)
    {
        uint32_t index = _ComputeHash(i, pItem, itemSize, bloom->dataSizeInBytes*8);
        bloom->data[index/8] |= 1 << (index % 8);
    }
}

void RedBloom_InsertS(RedBloom bloom, const char *szItem)
{
    RedBloom_Insert(bloom, szItem, strlen(szItem) + 1);
}

bool RedBloom_MayContain(RedBloom bloom, const void *pItem, size_t itemSize)
{
    unsigned i;
    for (i = 0; i < bloom->numHashes; i++)
    {
        uint32_t index = _ComputeHash(i, pItem, itemSize, bloom->dataSizeInBytes*8);
        if ((bloom->data[index/8] & (1 << (index % 8))) == 0)
            return false;
    }
    return true;
}

bool RedBloom_MayContainS(RedBloom bloom, const char *szItem)
{
    return RedBloom_MayContain(bloom, szItem, strlen(szItem) + 1);
}
