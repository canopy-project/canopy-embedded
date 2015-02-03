// Copyright 2015 SimpleThings, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



/*
 * RedString - Easy-to-use interface for working with strings.
 * 
 * A string is created from C-style null-terminated char * using the RedString_New
 * routine.  The characters of a string can be accessed using the
 * RedString_GetChars routine.
 */

#ifndef STRINGS_INCLUDED
#define STRINGS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/*
 * A RedString represents a string of characters.
 */
typedef struct RedString_t * RedString;
typedef const struct RedString_t * ConstRedString;

/*
 * A RedStringList represents an array of strings.
 */
typedef struct RedStringList_t * RedStringList;

#if 0
/*
 * A RegExp represents a regular expression pattern.  Regular expression
 * patterns are a seperate object type from strings for improved performance,
 * since they need to be compiled under-the-covers.
 */
typedef void * RegExp;

/*
 * RegExpNew -- Creates and compiles a regular expression pattern.  Pattern
 *      should follow the POSIX extended regular expression format (see "man 7
 *      regex").
 */
RegExp RegExpNew(const char *pattern);

/*
 * RegExpFree -- Frees resources associated with the compiled regex pattern.
 */
void RegExpFree(RegExp *phRegExp);
#endif

/*
 * RedString_New -- Create a RedString object from a null-terminated array of chars.
 *      This routine makes a copy of <src> and stores the copy in the RedString
 *      object.  If <src> is NULL or "" an empty string of length 0 is created.
 */
RedString RedString_New(const char *src);

/*
 * RedString_NewLength -- Create a RedString object from a null-terminated array of
 * chars, limited to at most <length> characters..
 *
 *      This routine makes a copy of <src> and stores the copy in the RedString
 *      object.  If <src> is NULL or "" an empty string of length 0 is created.
 */
RedString RedString_NewLength(const char *src, unsigned length);

/*
 * RedString_NewPrintf -- Perform an snprintf and store the result in a newly
 *      allocated string.
 */
RedString RedString_NewPrintf(const char *fmt, unsigned size, ...);

/*
 * RedString_PrintfToNewChars -- Performs an sprintf and returns the result as
 * a newly allocated null-terminated char array.
 */
char *RedString_PrintfToNewChars(const char *fmt, ...);

/*
 * RedString_Free -- Frees a string's memory.
 */
void RedString_Free(RedString s);

/*
 * RedString_Set -- Sets the contents of <hOut> to <in>.  If <in> is NULL,
 * clears the contents of <hOut> by setting it to "\0".
 */
void RedString_Set(RedString hOut, const char *in);

/*
 * RedString_Clear -- Clears the contents of <hOut> by setting it to "\0".
 */
void RedString_Clear(RedString hOut);

/*
 * RedString_Length -- Returns the number of characters in a string.
 */
unsigned RedString_Length(const RedString hRedString);

/*
 * RedString_Bytes -- Returns the length in bytes of a string.
 */
unsigned RedString_Bytes(RedString hRedString);

/*
 * RedString_GetChars -- Returns a C-style null-terminated array of chars for use
 *      in functions like "printf.  The returned pointer is only valid as long
 *      as no other RedString* calls are made using <hRedString>.
 */
const char * RedString_GetChars(const RedString hRedString);

/*
 * RedString_Copy -- Sets <hResult> to contain a copy of <hSrc>.  The previous
 *      contents of <hResult> are discarded.  Does nothing if hSrc and hResult
 *      are the same.
 */
void RedString_Copy(RedString hResult, const RedString hSrc);

enum
{
    /* Ignore case when doing comparison. */
    STRING_COMPARE_IGNORE_CASE_FLAG = 0x1,

    /* Ignore whitespace differences when doing comparison. */
    STRING_COMPARE_IGNORE_WHITESPACE_FLAG = 0x2
};

/* 
 * RedString_Compare -- Compares two strings.  Returns 0 if the strings match.
 *      Returns -1 if <hRedStringA> is less than <hRedStringB>.  Returns 1 if
 *      <hRedStringA> is greater than <hRedStringA>.  <stringComparisonFlags> must be
 *      the bitwise OR of zero or more STRING_COMPARE_* values, and they affect
 *      the comparison as described above.
 */
int RedString_Compare(
        const RedString hRedStringA, 
        const RedString hRedStringB,
        unsigned stringComparisonFlags);

bool RedString_Equals(
    RedString hRedString,
    const char * otherString);

char *RedString_strdup(const char *src);

/*
 * RedStringCompareChars - Same as RedStringCompare but with (char *)s.
 */
int RedString_CompareChars(
        const char * strA, 
        const char * strB,
        unsigned stringComparisonFlags);

/* 
 * RedString_ContainsChars -- Returns True if <hHaystack> contains the characters
 *      of <needle> as a substring.  Returns False otherwise.
 */
bool RedString_ContainsChars(ConstRedString hHaystack, const char *needle);

/*
 * RedString_BeginsWith -- Returns True if <hRedString> begins with the characters of
 *      <needle>.
 */
bool RedString_BeginsWith(ConstRedString hRedString, const char *needle);

/* RedString_Search -- Returns position of first occurance of <c> in <hHaystack>,
 *      or -1 if it is not found.
 */
int RedString_Search(ConstRedString hHaystack, const char c);

/*
 * RedString_Trim -- Removes whitespace from the left and right sides of <hRedString>.
 */
void RedString_Trim(RedString hRedString);

/*
 * RedString_RemoveFloat -- Read a floating-point number from the beginning of
 *      <hRedString>, remove it from <hRedString>, and return it. */
float RedString_RemoveFloat(RedString hRedString);

/*
 * RedString_FirstNonWhitespaceChar -- Returns the first non-whitespace character
 *      of <hRedString>.
 */
char RedString_FirstNonWhitespaceChar(ConstRedString hRedString);

/*
 * RedString_SubString -- Sets <hResult> to contain a substring of <hSrc>
 *      (discarding <hResult>'s contents).  <start> and <end> specify the
 *      inclusive range of characters to copy.  
 *
 *      0 indicates the first character in the string.
 *      (RedString_Length(hSrc) - 1) is the last character in the string.
 *
 *      Negative values can be used to count backwards starting with the end of
 *      the string.
 *
 *      -1 indicates the last character in the string.
 *      -(RedString_Length(hSrc)) indicates the first character in the string. 
 * 
 *      If <start> or <end> are outside the string, only characters that fall
 *      within the string are copied.  For example:
 *      
 *          // This code sets hDest to "Hell".
 *          RedString hSrc = RedString_New("Hello");
 *          RedString hDest = RedString_New(NULL);
 *          RedStringSubString(hDest, hSrc, -7, -2);
 *
 *      If <end> comes before <start>, the result is the empty string.
 *
 *      It is allowed for hResult to equal hSrc.
 */
void RedString_SubString(
        RedString hResult, 
        const RedString hSrc, 
        int start, 
        int end);

unsigned RedString_ToU32(ConstRedString hRedString);

/*
 * RedString_Reverse -- Reverses the contents of <hRedString> in place.
 */
void RedString_Reverse(RedString hRedString);

typedef enum
{
    STRING_CASE_UPPER,
    STRING_CASE_LOWER
} RedStringCase; 

/* 
 * RedString_CaseConvert -- Changes the case of <hRedString> in place.
 */
void RedString_CaseConvert(RedString hRedString, RedStringCase newCase);

/*
 * RedString_Append -- Adds <hAppend> to the end of <hOriginal>.
 */
void RedString_Append(RedString hOriginal, const RedString hAppend);

/*
 * RedString_AppendChars -- Adds NULL-terminated string <pAppend> to the end of
 *      <hOriginal>.
 */
void RedString_AppendChars(RedString hOriginal, const char * pAppend);

/*
 * RedString_AppendPrintf -- Adds <hAppend> to the end of <hOriginal>.
 */
void RedString_AppendPrintf(RedString hOriginal, const char *fmt, unsigned size, ...);

/*
 * RedString_RemoveToChar -- Remove all characters in <hRedString> to the left of (and
 *      including) the first occurance of <c>.  If <c> does not appear in
 *      <hRedString>, this clears the string.
 */
void RedString_RemoveToChar(RedString hRedString, char c);

char * RedString_ToNewChars(RedString s);

/*
 * RedString_Hash -- Compute a hash string from <hSrc> and store it in <hResult>.
 *      It is valid for <hResult> and <hSrc> to be the same string.
 *
 * TODO: Let caller choose hash type (SHA1, etc)
 */
void RedString_Hash(RedString hResult, ConstRedString hSrc);

/*
 * RedString_Rot13 -- Rot13 encryption/decryption.  Rot13 is extremely weak
 *      encryption and should not be used in situations where security is a
 *      real concern.  <hResult> and <hOriginal> can be the same string.
 *
 *      Note: g? in VIM will apply Rot13 encryption/decryption.
 */
void RedString_Rot13(RedString hResult, RedString hOriginal);

/*
 * RedString_Split -- Creates a new RedStringList object by splitting <hRedString> into
 *      several smaller strings.  The <delimeter> character determines where
 *      the boundaries are, and all <delimeter> characters are removed.
 *
 *      For example: 
 *          splitting "This is a:  test" 
 *          with delimeter ' ' will produce:
 *
 *          "This"
 *          "is"
 *          "a:"
 *          ""
 *          "test"
 *
 *      Use '\n' for <delimeter> to split a multi-line string into individual
 *      lines.
 *
 *      The returned RedStringList object must be freed with RedString_ListFree() when
 *      you are finished using it.  The individual strings of the may be
 *      modified (ie, with RedStringAppend, RedStringReverse, etc), but MUST NOT be
 *      freed with RedStringFree.
 */
RedStringList RedString_Split(RedString hRedString, char delimiter);

/*
 * RedString_SplitChars -- Same as RedString_Split but takes a (char *) instead
 * of RedString.
 */
RedStringList RedString_SplitChars(const char *string, char delimiter);

/*
 * RedStringList_NumRedStrings -- Returns the number of strings in <hList>.
 */
unsigned RedStringList_NumStrings(RedStringList hList);

/*
 * RedStringList_GetString -- Returns the string in position <idx> of <hList>.
 *      Will ASSERT if <idx> is greater than or equal to the number of strings
 *      in <hList>.
 *
 *      The RedStrings returned by this routine must never be individually
 *      freed.  Instead, use RedStringList_Free to free the whole list when you
 *      are finished with it.
 */
RedString RedStringList_GetString(RedStringList hList, unsigned idx);

const char * RedStringList_GetStringChars(RedStringList hList, unsigned idx);

void RedStringList_Join(RedString hString, RedStringList hList, const char *joiner);

void RedStringList_AppendPrintf(RedStringList list, const char *fmt, ...);

void RedStringList_AppendChars(RedStringList list, const char *chars);

RedStringList RedStringList_New();

char * RedStringList_ToNewChars(RedStringList list);

char * RedStringList_JoinToNewChars(RedStringList list, const char *joiner);

/*
 * RedStringList_Free -- Frees all strings contained in <*phList> and all other
 *      resources associated with the list.  After calling RedStringList_Free you
 *      may no longer use strings obtained from <*phList> or BAD THINGS will
 *      happen.
 */
void RedStringList_Free(RedStringList list);

typedef enum
{
    STRING_REPLACE_IGNORE_CASE_FLAG,
    STRING_REPLACE_ALL
} RedStringReplaceFlags;

/*
 * RedStringReplace -- not yet supported
 */
/*void RedStringReplace(
        RedString hResult,
        const RedString hSrc,
        const RedString hFind,
        const RedString hReplacement,
        unsigned replaceFlags);*/

/*
 * RedStringReplaceRegExp -- Not yet supported
 */
/*void RedStringReplaceRegExp(
        RedString hResult,
        const RedString hSrc,
        const RegExp hRegExp,
        const RedString hReplacement);*/


/* RedStringSplit -- Returns an array of words of <hRedString>.  <hSeperator> If
 * <hSeperator> is NULL, then the words are separated by arbitrary strings of
 * whitespace characters.
 */
/*Array RedStringSplitChar(
        RedString
        const RedString hSeparator,
        )*/

#ifdef __cplusplus
}
#endif

#endif
