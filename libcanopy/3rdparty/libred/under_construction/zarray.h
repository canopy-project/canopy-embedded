/*
 *  ZARRAY -- Dynamic arrays
 *
 *  Author: Gregory Prsiament (greg@toruslabs.com)
 *
 *  ===========================================================================
 *  Creative Commons CC0 1.0 Universal - Public Domain 
 *
 *  To the extent possible under law, Gregory Prisament has waived all
 *  copyright and related or neighboring rights to ZARRAY. This work is
 *  published from: United States. 
 *
 *  For details please refer to either:
 *      - http://creativecommons.org/publicdomain/zero/1.0/legalcode
 *      - The LICENSE file in this directory, if present.
 *  ===========================================================================
 */
#ifndef ZARRAY_INCLUDED
#define ZARRAY_INCLUDED

#include <stdlib.h>

/**
 * @page Debug features
 *  @brief Disable debug features
 *
 *  By default, ZARRAY routines make several assertions and other runtime
 *  checks to help you quickly catch bugs if you're use ZARRAY improperly.
 *
 *  These runtime checks come with a small amount of performance and memory
 *  overhead.  If you're trying to maximize performance, and are confident that
 *  your software is using ZARRAY properly, you may wish to disable debug
 *  features for release builds.
 *
 *  You can disable the checks when compiling, for example:
 *
 *      gcc -DZ4C_NO_DEBUG mysource.c
 *
 *  Alternatively, you can define it in your source files like so:
 *
 *      #define DZ4C_NO_DEBUG
 *      #include <zarray.h>
 *      ...
 */
#ifndef Z4C_NO_DEBUG
#include <assert.h>
#include <stdio.h>
#define _ZARRAY_DEBUG 1
#define _ZARRAY_DEBUG_ASSERT(cond) assert(cond)
#define _ZARRAY_DEBUG_VERIFY(cond, msg)  \
    ( \
        !(cond) ? \
            ( \
                fprintf(stderr, msg "\n"), \
                fflush(0), \
                assert(!msg) \
            ) : (void)0 \
    )

#define _ZARRAY_DEBUG_MAGIC_MATCHES(zarray) \
    ((zarray)->_magicid[0] == 'z' \
        && (zarray)->_magicid[1] == 'a' \
        && (zarray)->_magicid[2] == 'r' \
        && (zarray)->_magicid[3] == 'y')

#define _ZARRAY_DEBUG_CLEAR_MAGIC(zarray) \
    ((zarray)->_magicid[0] = 0, \
     (zarray)->_magicid[1] = 0, \
     (zarray)->_magicid[2] = 0, \
     (zarray)->_magicid[3] = 0)

#define _ZARRAY_DEBUG_IS_ZARRAY(zarray)  \
    ((zarray) && _ZARRAY_DEBUG_MAGIC_MATCHES(zarray))

#define _ZARRAY_DEBUG_VERIFY_IS_ZARRAY(macroname, zarray)  \
    ( \
        _ZARRAY_DEBUG_VERIFY(zarray, \
                "ERROR in " macroname ": <zarray> must be non-NULL."), \
        _ZARRAY_DEBUG_VERIFY(_ZARRAY_DEBUG_MAGIC_MATCHES(zarray), \
                "ERROR in " macroname ": <zarray> does not appear to point to a ZArray object. " \
                "Please double-check that you allocated it with ZARRAY_ALLOC") \
    )
#else
#define _ZARRAY_DEBUG 0
#define _ZARRAY_DEBUG_ASSERT(cond) (void)0
#define _ZARRAY_DEBUG_VERIFY(cond, msg) (void) 0
#define _ZARRAY_DEBUG_IS_ZARRAY(zarray)  (void) 0
#define _ZARRAY_DEBUG_VERIFY_IS_ZARRAY(macroname, zarray)  (void) 0
#define _ZARRAY_DEBUG_CLEAR_MAGIC(zarray) (void) 0
#endif

#ifdef __cplusplus
template <typename T>
struct _ZArrayStruct
{
#if _ZARRAY_DEBUG
    char _magicid[4];
#endif
    unsigned numItems;
    unsigned actualNumItems;
    T * item;
public:
    void Realloc(void) {
        item = (T *)realloc(item, actualNumItems * sizeof(T));
    }
};
#define ZARRAY_STRUCT(typ) _ZArrayStruct<typ>
#else   /* __cplusplus (ELSE) */
#if _ZARRAY_DEBUG
#define ZARRAY_STRUCT(typ) \
    struct \
    { \
        char _magicid[4]; \
        unsigned numItems; \
        unsigned actualNumItems; \
        typ * item; \
    }
#else
#define ZARRAY_STRUCT(typ) \
    struct \
    { \
        unsigned numItems; \
        unsigned actualNumItems; \
        typ * item; \
    }
#endif
#endif /* __cplusplus (ENDIF) */


#define ZARRAY(typ) ZARRAY_STRUCT(typ) *

static unsigned _ZArray_NextPowerOfTwo(unsigned x)
{
    /* From http://acius2.blogspot.com/2007/11/calculating-next-power-of-2.html */
    x--;
    x = (x >> 1) | x;
    x = (x >> 2) | x;
    x = (x >> 4) | x;
    x = (x >> 8) | x;
    x = (x >> 16) | x;
    return x + 1;
}

#ifdef __cplusplus
#define _ZARRAY_RETURN_CAST(typ) (ZARRAY(typ))
#define _ZARRAY_REALLOC(zarray) zarray->Realloc();
#else
#define _ZARRAY_REALLOC(zarray) \
    (zarray)->item = realloc(((zarray)->item), \
         sizeof(*((zarray)->item))*(zarray)->actualNumItems)

#define _ZARRAY_RETURN_CAST(typ) 
#endif

#define ZARRAY_NEW(typ, startNumItems) \
    _ZARRAY_RETURN_CAST(typ) _ZArray_AllocGeneric(sizeof(typ), startNumItems)

static void * _ZArray_AllocGenericZeroed(unsigned elemSize, unsigned startNumItems);
static void * _ZArray_AllocGeneric(unsigned elemSize, unsigned startNumItems)
{
    unsigned actualNumItems;
    typedef ZARRAY_STRUCT(void) _Struct;
    _Struct * out;
    out = (_Struct *)malloc(sizeof(*out));
    actualNumItems = _ZArray_NextPowerOfTwo(startNumItems+1);
    out->numItems = startNumItems;
    out->actualNumItems = actualNumItems;
#if _ZARRAY_DEBUG
    out->_magicid[0] = 'z';
    out->_magicid[1] = 'a';
    out->_magicid[2] = 'r';
    out->_magicid[3] = 'y';
#endif
    out->item = malloc(elemSize*actualNumItems);
    if (!out->item)
    {
        free(out);
        return NULL;
    }
    if (0)
        return _ZArray_AllocGenericZeroed(0, 0);
    return (void *)out;
}

#define ZARRAY_NEW_ZEROED(typ, startNumItems) \
    _ZARRAY_RETURN_CAST(typ) _ZArray_AllocGenericZeroed(sizeof(typ), startNumItems)
static void * _ZArray_AllocGenericZeroed(unsigned elemSize, unsigned startNumItems)
{
    unsigned actualNumItems;
    typedef ZARRAY_STRUCT(void) _Struct;
    _Struct * out;
    out = (_Struct *)malloc(sizeof(*out));
    actualNumItems = _ZArray_NextPowerOfTwo(startNumItems+1);
    out->numItems = startNumItems;
    out->actualNumItems = actualNumItems;
#if _ZARRAY_DEBUG
    out->_magicid[0] = 'z';
    out->_magicid[1] = 'a';
    out->_magicid[2] = 'r';
    out->_magicid[3] = 'y';
#endif
    out->item = calloc(elemSize, actualNumItems);
    if (!out->item)
    {
        free(out);
        return NULL;
    }
    return (void *)out;
}

#define ZARRAY_FREE(zarray) \
    do { \
        if (zarray) { \
            _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_FREE", zarray);  \
            _ZARRAY_DEBUG_CLEAR_MAGIC(zarray); \
            free((zarray)->item); \
            free((zarray)); \
        } \
    } while (0)

#if _ZARRAY_DEBUG
/**
 * Get the number of elements in a dynamic array.
 *
 * @param zarray specifies the dynamic array to check.  This must be a pointer
 *          returned by ZARRAY_ALLOC or the most recent resizing operation
 *          (whichever happened most recently).
 *
 * @retval unsigned number of elements in the array
 */
#define ZARRAY_NUM_ITEMS(zarray) _ZArray_NumItemsGeneric((void *)zarray)
static unsigned _ZArray_NumItemsGeneric(void * zarray)
{
    _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_NUM_ITEMS", (ZARRAY(void))zarray);
    return ((ZARRAY(void))(zarray))->numItems;
}
#else
#define ZARRAY_NUM_ITEMS(zarray) \
        ((unsigned)((zarray)->numItems))
#endif

/**
 * Macro for accessing the last element of a dynamic array.
 *
 * This macro indexes the last element of the array.
 *
 * It expands to a modifieable lvalue that can be used in assignements.  For
 * example:
 *
 *      ZARRAY_TAIL(myIntDynArray) = 43;
 *
 * @param zarray specifies the dynamic array to index.  This should be a
 *          pointer returned by ZARRAY_ALLOC or the most recent resizing
 *          operation (whichever happened most recently).  The array must
 *          contain at least 1 element or your program will corrupt memory and
 *          crash.
 *
 * @retval lvalue Modifiable l-value that is the last element in the array.
 */
#define ZARRAY_TAIL(zarray) \
    (ZARRAY_AT((zarray), ZARRAY_NUM_ITEMS(zarray) - 1))

/**
 * Increase the size of the array by one (1) and update the array pointer.
 *
 * One (1) new element is appended to the end unitialized.
 *
 * This macro automatically reasigns <zarray> to the new array address (and
 * does not return anything).  The macro references <zarray> twice, so avoid
 * complex expressions for that parameter.
 *
 * This routine is slightly more efficient than calling ZARRAY_GROW(zarray, 1).
 *
 * @param [in,out] zarray specifies the dynamic array to grow.  This should be a
 *          pointer returned by ZARRAY_ALLOC or the most recent resizing
 *          operation (whichever happened most recently).
 *
 * @return None
 */
#define ZARRAY_GROW_BY_ONE(zarray) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_GROW_BY_ONE", zarray);  \
        (zarray)->numItems++; \
        if ((zarray)->numItems > (zarray)->actualNumItems) \
        { \
            (zarray)->actualNumItems <<= 1; \
            _ZARRAY_REALLOC(zarray); \
        } \
    } while (0)

/**
 * Decrease the size of the array by one (1) and update the array pointer.
 *
 * One (1) element is removed from the the end, and its value lost.
 *
 * This macro automatically reasigns <zarray> to the new array address (and
 * does not return anything).  The macro references <zarray> twice, so avoid
 * complex expressions for that parameter.
 *
 * This routine is slightly more efficient than calling ZARRAY_SHRINK(zarray, 1).
 *
 * @param [in,out] zarray specifies the dynamic array to grow.  This should be a
 *          pointer returned by ZARRAY_ALLOC or the most recent resizing
 *          operation (whichever happened most recently).
 *
 * @return None
 */
#define ZARRAY_SHRINK_BY_ONE(zarray) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_SHRINK_BY_ONE", zarray);  \
        _ZARRAY_DEBUG_VERIFY((zarray)->numItems, \
                "ERROR in ZARRAY_SHRINK_BY_ONE: Cannot shrink <zarray> below 0 elements."); \
        (zarray)->numItems--; \
        if ((zarray)->numItems < (zarray)->actualNumItems>>2) \
        { \
            (zarray)->actualNumItems >>= 1; \
            _ZARRAY_REALLOC(zarray); \
        } \
    } while (0)
 
#define ZARRAY_GROW(zarray, numItemsToAdd) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_GROW", zarray);  \
        _ZARRAY_DEBUG_VERIFY(((numItemsToAdd) >= 0), \
                "ERROR in ZARRAY_GROW: <numItemsToAdd> must be positive."); \
        (zarray)->numItems += (numItemsToAdd); \
        if ((zarray)->numItems > (zarray)->actualNumItems) \
        { \
            (zarray)->actualNumItems = _ZArray_NextPowerOfTwo((zarray)->numItems); \
            _ZARRAY_REALLOC(zarray); \
        } \
    } while (0)

#define ZARRAY_SHRINK(zarray, numItemsToRemove) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_SHRINK", zarray);  \
        _ZARRAY_DEBUG_VERIFY((zarray)->numItems >= numItemsToRemove, \
                "ERROR in ZARRAY_SHRINK: Cannot shrink <zarray> below 0 elements."); \
        (zarray)->numItems -= (numItemsToRemove); \
        if ((zarray)->numItems < (zarray)->actualNumItems>>2) \
        { \
            (zarray)->actualNumItems = _ZArray_NextPowerOfTwo((zarray)->numItems + 1) << 1 ; \
            _ZARRAY_REALLOC(zarray); \
        } \
    } while (0)

#define ZARRAY_RESIZE(zarray, newSize) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_RESIZE", zarray);  \
        _ZARRAY_DEBUG_VERIFY(((newSize) >= 0), \
                "ERROR in ZARRAY_RESIZE: <numItems> must be positive."); \
        (zarray)->numItems = (newSize); \
        if ((zarray)->numItems > (zarray)->actualNumItems) \
        { \
            (zarray)->actualNumItems = _ZArray_NextPowerOfTwo((zarray)->numItems); \
            _ZARRAY_REALLOC(zarray); \
        } \
        else if ((zarray)->numItems < (zarray)->actualNumItems>>2) \
        { \
            (zarray)->actualNumItems = _ZArray_NextPowerOfTwo((zarray)->numItems+1) << 1; \
            _ZARRAY_REALLOC(zarray); \
        } \
    } while (0)

#define ZARRAY_APPEND(zarray, value) \
    do { \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_APPEND", zarray);  \
        ZARRAY_GROW_BY_ONE(zarray); \
        ZARRAY_TAIL(zarray) = (value); \
    } while (0)


#define ZARRAY_POP(zarray) \
    ( \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_POP", zarray), \
        _ZARRAY_DEBUG_VERIFY((zarray)->numItems > 0, \
                "ERROR in ZARRAY_POP: <zarray> is empty"), \
        (zarray)->numItems--, \
        (zarray->numItems < (zarray)->actualNumItems>>2) ? \
            ( \
                (zarray)->actualNumItems >>= 1, \
                _ZARRAY_REALLOC(zarray) \
            ) : (void)0, \
        (zarray)->item[(zarray)->numItems] \
    )

     

#define _ZARRAY_AT_INDEX(zarray, idx) \
    ( \
        _ZARRAY_DEBUG_VERIFY_IS_ZARRAY("ZARRAY_AT", zarray), \
        _ZARRAY_DEBUG_VERIFY(idx >= 0, "ERROR in ZARRAY_AT: <index> out of bounds"), \
        _ZARRAY_DEBUG_VERIFY(idx < (zarray)->numItems, "ERROR in ZARRAY_AT: <index> out of bounds"), \
        idx \
    )

#define ZARRAY_AT(zarray, idx) \
    ((zarray)->item[_ZARRAY_AT_INDEX(zarray, idx)])

#endif
