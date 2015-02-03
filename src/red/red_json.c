#include "red_json.h"

#include "red_hash.h"
#include "red_string.h"
#include "../under_construction/zarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define REF(hObj) ((hObj)->refcnt++, (hObj))

static char * _StrDup(const char *s)
{
    char * out = malloc(strlen(s) + 1);
    if (out)
        strcpy(out, s);
    return out;
}

typedef struct RedJsonValue_t
{
    int refcnt;
    RedJsonValueTypeEnum type;
    union
    {
        char *sz;
        double dbl;
        RedJsonObject hObj;
        RedJsonArray hArray;
        bool boolean;
    } val;
} RedJsonValue_t;

typedef struct RedJsonObject_t
{
    int refcnt;
    RedHash hash;
} RedJsonObject_t;

typedef struct RedJsonArray_t
{
    int refcnt;
    ZARRAY(RedJsonValue) items;
} RedJsonArray_t;

RedJsonValue RedJsonValue_FromString(const char * sz)
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_STRING;
    hNew->val.sz = _StrDup(sz);
    hNew->refcnt = 0;
    return hNew;
}

RedJsonValue RedJsonValue_FromNumber(double val)
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_NUMBER;
    hNew->val.dbl = val;
    hNew->refcnt = 0;
    return hNew;
}

RedJsonValue RedJsonValue_FromObject(RedJsonObject hObj)
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_OBJECT;
    hNew->val.hObj = REF(hObj);
    hNew->refcnt = 0;
    return hNew;
}

RedJsonValue RedJsonValue_FromArray(RedJsonArray hArray)
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_ARRAY;
    hNew->val.hArray = REF(hArray);
    hNew->refcnt = 0;
    return hNew;
}

RedJsonValue RedJsonValue_FromBoolean(bool val)
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_BOOLEAN;
    hNew->val.boolean = val;
    hNew->refcnt = 0;
    return hNew;
}

RedJsonValue RedJsonValue_Null()
{
    RedJsonValue hNew;
    hNew = malloc(sizeof(RedJsonValue_t));
    hNew->type = RED_JSON_VALUE_TYPE_NULL;
    hNew->refcnt = 0;
    return hNew;
}

void _RedJsonValue_Free(RedJsonValue hVal)
{
    free(hVal->val.sz);
    free(hVal);
}

char * RedJsonValue_GetString(RedJsonValue hVal)
{
    assert(hVal->type == RED_JSON_VALUE_TYPE_STRING);
    return _StrDup(hVal->val.sz);
}
double RedJsonValue_GetNumber(RedJsonValue hVal)
{
    assert(hVal->type == RED_JSON_VALUE_TYPE_NUMBER);
    return hVal->val.dbl;
}
RedJsonObject RedJsonValue_GetObject(RedJsonValue hVal)
{
    assert(hVal->type == RED_JSON_VALUE_TYPE_OBJECT);
    return REF(hVal->val.hObj);
}
RedJsonArray RedJsonValue_GetArray(RedJsonValue hVal)
{
    assert(hVal->type == RED_JSON_VALUE_TYPE_ARRAY);
    return REF(hVal->val.hArray);
}
bool RedJsonValue_GetBoolean(RedJsonValue hVal)
{
    assert(hVal->type == RED_JSON_VALUE_TYPE_BOOLEAN);
    return hVal->val.boolean;
}

bool RedJsonValue_IsString(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_STRING;
}
bool RedJsonValue_IsNumber(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_NUMBER;
}
bool RedJsonValue_IsObject(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_OBJECT;
}
bool RedJsonValue_IsArray(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_ARRAY;
}
bool RedJsonValue_IsBoolean(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_BOOLEAN;
}
bool RedJsonValue_IsNull(RedJsonValue hVal)
{
    return hVal->type == RED_JSON_VALUE_TYPE_NULL;
}

RedJsonObject RedJsonObject_New()
{
    RedJsonObject jsonObj;
    jsonObj = malloc(sizeof(RedJsonObject_t));
    jsonObj->hash = RedHash_New(0);
    jsonObj->refcnt = 1;
    return jsonObj;
}

void RedJsonObject_Set(RedJsonObject hObj, const char * szKey, RedJsonValue hVal)
{
    hVal->refcnt++;
    RedHash_InsertS(hObj->hash, szKey, hVal);
}

void RedJsonObject_SetNull(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue newVal = RedJsonValue_Null();
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

void RedJsonObject_SetString(RedJsonObject hObj, const char * szKey, const char *szVal)
{
    RedJsonValue newVal = RedJsonValue_FromString(szVal);
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

void RedJsonObject_SetNumber(RedJsonObject hObj, const char * szKey, double val)
{
    RedJsonValue newVal = RedJsonValue_FromNumber(val);
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

void RedJsonObject_SetObject(RedJsonObject hObj, const char * szKey, RedJsonObject hObjVal)
{
    RedJsonValue newVal = RedJsonValue_FromObject(hObjVal);
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

void RedJsonObject_SetArray(RedJsonObject hObj, const char * szKey, RedJsonArray hArray)
{
    RedJsonValue newVal = RedJsonValue_FromArray(hArray);
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

void RedJsonObject_SetBoolean(RedJsonObject hObj, const char * szKey, bool val)
{
    RedJsonValue newVal = RedJsonValue_FromBoolean(hObj);
    RedHash_InsertS(hObj->hash, szKey, newVal);
}

RedJsonValue RedJsonObject_Get(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetWithDefaultS(hObj->hash, szKey, NULL);
    return jsonVal;
}

RedJsonValueTypeEnum RedJsonObject_GetType(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetWithDefaultS(hObj->hash, szKey, NULL);
    return jsonVal ? jsonVal->type : RED_JSON_VALUE_TYPE_INVALID;
}

char * RedJsonObject_GetString(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetS(hObj->hash, szKey);
    return jsonVal->val.sz;
}
double RedJsonObject_GetNumber(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetS(hObj->hash, szKey);
    return jsonVal->val.dbl;
}
RedJsonObject RedJsonObject_GetObject(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetS(hObj->hash, szKey);
    return jsonVal->val.hObj;
}
RedJsonArray RedJsonObject_GetArray(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetS(hObj->hash, szKey);
    return jsonVal->val.hArray;
}
bool RedJsonObject_GetBoolean(RedJsonObject hObj, const char * szKey)
{
    RedJsonValue jsonVal;
    jsonVal = RedHash_GetS(hObj->hash, szKey);
    return jsonVal->val.boolean;
}

bool RedJsonObject_IsValueString(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_STRING;
}
bool RedJsonObject_IsValueNumber(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_NUMBER;
}
bool RedJsonObject_IsValueObject(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_OBJECT;
}
bool RedJsonObject_IsValueArray(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_ARRAY;
}
bool RedJsonObject_IsValueBoolean(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_BOOLEAN;
}
bool RedJsonObject_IsValueNull(RedJsonObject hObj, const char * szKey)
{
    return RedJsonObject_GetType(hObj, szKey) == RED_JSON_VALUE_TYPE_NULL;
}

void RedJsonObject_Unset(RedJsonObject hObj, const char * szKey)
{
    // TODO
}
bool RedJsonObject_HasKey(RedJsonObject hObj, const char * szKey)
{
    return RedHash_HasKeyS(hObj->hash, szKey);
}

unsigned RedJsonObject_NumItems(RedJsonObject jsonObj)
{
    return RedHash_NumItems(jsonObj->hash);
}

char ** RedJsonObject_NewKeysArray(RedJsonObject jsonObj)
{
    char **out;
    RedHashIterator_t iter;
    const char *key;
    size_t keySize;
    const void *value;
    unsigned numKeys;
    unsigned i=0;
    
    numKeys = RedJsonObject_NumItems(jsonObj);

    out = malloc(numKeys*sizeof(char *));

    RED_HASH_FOREACH(iter, jsonObj->hash, (const void **)&key, &keySize, &value)
    {
        out[i] = malloc(keySize);
        strncpy(out[i], key, keySize);
        i++;
    }
    return out;
}

void RedJsonObject_FreeKeysArray(char **keysArray)
{
    /* TODO: Free each key */
    free(keysArray);
}
RedJsonArray RedJsonArray_New()
{
    RedJsonArray hNew;
    hNew = malloc(sizeof(RedJsonArray_t));
    hNew->items = ZARRAY_NEW(RedJsonValue, 0);
    hNew->refcnt = 1;
    return hNew;
}

unsigned RedJsonArray_NumItems(RedJsonArray hArray)
{
    return ZARRAY_NUM_ITEMS(hArray->items);
}

void RedJsonArray_Append(RedJsonArray hArray, RedJsonValue hVal)
{
    ZARRAY_APPEND(hArray->items, hVal);
}
void RedJsonArray_AppendString(RedJsonArray hArray, char * szVal)
{
    RedJsonValue hVal;
    hVal = RedJsonValue_FromString(szVal);
    ZARRAY_APPEND(hArray->items, hVal);
}
void RedJsonArray_AppendNumber(RedJsonArray hArray, double val)
{
    RedJsonValue hVal;
    hVal = RedJsonValue_FromNumber(val);
    ZARRAY_APPEND(hArray->items, hVal);
}
void RedJsonArray_AppendObject(RedJsonArray hArray, RedJsonObject hObj)
{
    RedJsonValue hVal;
    hVal = RedJsonValue_FromObject(hObj);
    ZARRAY_APPEND(hArray->items, hVal);
}
void RedJsonArray_AppendArray(RedJsonArray jsonArray, RedJsonArray val)
{
    RedJsonValue jsonVal;
    jsonVal = RedJsonValue_FromArray(val);
    ZARRAY_APPEND(jsonArray->items, jsonVal);
}
void RedJsonArray_AppendBoolean(RedJsonArray hArray, bool val)
{
    RedJsonValue hVal;
    hVal = RedJsonValue_FromBoolean(val);
    ZARRAY_APPEND(hArray->items, hVal);
}
void RedJsonArray_AppendNull(RedJsonArray hArray)
{
    RedJsonValue hVal;
    hVal = RedJsonValue_Null();
    ZARRAY_APPEND(hArray->items, hVal);
}
RedJsonValue RedJsonArray_GetEntry(RedJsonArray jsonArray, unsigned idx)
{
    return ZARRAY_AT(jsonArray->items, idx);
}
char * RedJsonArray_GetEntryString(RedJsonArray jsonArray, unsigned idx)
{
    RedJsonValue val;
    val = ZARRAY_AT(jsonArray->items, idx);
    return val->val.sz;
}
double RedJsonArray_GetEntryNumber(RedJsonArray jsonArray, unsigned idx)
{
    RedJsonValue val;
    val = ZARRAY_AT(jsonArray->items, idx);
    return val->val.dbl;
}
RedJsonObject RedJsonArray_GetEntryObject(RedJsonArray jsonArray, unsigned idx)
{
    RedJsonValue val;
    val = ZARRAY_AT(jsonArray->items, idx);
    return val->val.hObj;
}
RedJsonArray RedJsonArray_GetEntryArray(RedJsonArray jsonArray, unsigned idx)
{
    RedJsonValue val;
    val = ZARRAY_AT(jsonArray->items, idx);
    return val->val.hArray;
}
bool RedJsonArray_GetEntryBoolean(RedJsonArray jsonArray, unsigned idx)
{
    RedJsonValue val;
    val = ZARRAY_AT(jsonArray->items, idx);
    return val->val.boolean;
}
bool RedJsonArray_IsEntryString(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_STRING);
}
bool RedJsonArray_IsEntryNumber(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_NUMBER);
}
bool RedJsonArray_IsEntryObject(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_OBJECT);
}
bool RedJsonArray_IsEntryArray(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_ARRAY);
}
bool RedJsonArray_IsEntryBoolean(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_BOOLEAN);
}
bool RedJsonArray_IsEntryNull(RedJsonArray hArray, unsigned idx)
{
    return (ZARRAY_AT(hArray->items, idx)->type == RED_JSON_VALUE_TYPE_NULL);
}


void _Value_ToJson(RedStringList chain, RedJsonValue hVal)
{
    switch (hVal->type)
    {
        case RED_JSON_VALUE_TYPE_STRING:
        {
            // TODO: escape
            RedStringList_AppendPrintf(chain, "\"%s\"", hVal->val.sz);
            break;
        }
        case RED_JSON_VALUE_TYPE_NUMBER:
        {
            // TODO: formatting?
            RedStringList_AppendPrintf(chain, "%lf", hVal->val.dbl);
            break;
        }
        case RED_JSON_VALUE_TYPE_BOOLEAN:
        {
            RedStringList_AppendPrintf(chain, "%s", hVal->val.boolean ? "true" : "false");
            break;
        }
        case RED_JSON_VALUE_TYPE_OBJECT:
        {
            RedHashIterator_t iter;
            const char *key;
            size_t keySize;
            const void *value;
            int numItems;
            int i;
            RedStringList_AppendPrintf(chain, "{\n");
            numItems = RedHash_NumItems(hVal->val.hObj->hash);
            i = 0;
            RED_HASH_FOREACH(iter, hVal->val.hObj->hash, (const void **)&key, &keySize, &value)
            {
                RedJsonValue val = (RedJsonValue)value;
                RedStringList_AppendPrintf(chain, "\"%s\" : ", key);
                _Value_ToJson(chain, val);
                if (i < numItems - 1)
                {
                    RedStringList_AppendPrintf(chain, ",\n");
                }
                else
                {
                    RedStringList_AppendPrintf(chain, "\n");
                }
                i++;
            }
            RedStringList_AppendPrintf(chain, "}\n");
            break;
        }
        case RED_JSON_VALUE_TYPE_ARRAY:
        {
            int i, numItems;
            ZARRAY(RedJsonValue) items;
            RedStringList_AppendChars(chain, "[");
            items = (void *)hVal->val.hArray->items;
            numItems = ZARRAY_NUM_ITEMS(items);
            for (i = 0; i < numItems; i++)
            {
                RedJsonValue hItemVal = ZARRAY_AT(items, i);
                _Value_ToJson(chain, hItemVal);
                if (i < numItems - 1)
                    RedStringList_AppendChars(chain, ", ");
            }
            RedStringList_AppendChars(chain, "]");
            break;
        }
        case RED_JSON_VALUE_TYPE_NULL:
        {
            RedStringList_AppendChars(chain, "null");
            break;
        }
        case RED_JSON_VALUE_TYPE_INVALID:
        {
            assert("!Unexpected JSON type!");
            break;
        }
    }
}

char * RedJsonValue_ToJsonString(RedJsonValue hVal)
{
    RedStringList chain = RedStringList_New();
    char * out;
    _Value_ToJson(chain, hVal);
    out = RedStringList_ToNewChars(chain);
    RedStringList_Free(chain);
    return out;
}

char * RedJsonObject_ToJsonString(RedJsonObject jsonObj)
{
    char *out;
    RedJsonValue val = RedJsonValue_FromObject(jsonObj);
    out = RedJsonValue_ToJsonString(val);
    return out;
}

typedef enum _JsonTokenEnum
{
    _JSON_TOKEN_OPEN_CURLY_BRACE,
    _JSON_TOKEN_CLOSE_CURLY_BRACE,
    _JSON_TOKEN_OPEN_SQUARE_BRACE,
    _JSON_TOKEN_CLOSE_SQUARE_BRACE,
    _JSON_TOKEN_COLON,
    _JSON_TOKEN_COMMA,
    _JSON_TOKEN_STRING,
    _JSON_TOKEN_NUMBER,
    _JSON_TOKEN_BOOL,
    _JSON_TOKEN_NULL,
} _JsonTokenEnum;

typedef struct _JsonToken
{
    struct _JsonToken *next;
    _JsonTokenEnum type;
    union
    {
        bool valBool;
        double valDbl;
        char *valString;
    } val;
} _JsonToken;

static _JsonToken * _EmitSimpleToken(_JsonToken *parent, _JsonTokenEnum type)
{
    switch (type)
    {
        case _JSON_TOKEN_OPEN_CURLY_BRACE:
        case _JSON_TOKEN_CLOSE_CURLY_BRACE:
        case _JSON_TOKEN_OPEN_SQUARE_BRACE:
        case _JSON_TOKEN_CLOSE_SQUARE_BRACE:
        case _JSON_TOKEN_COLON:
        case _JSON_TOKEN_COMMA:
        case _JSON_TOKEN_NULL:
        {
            _JsonToken *token = calloc(1, sizeof(_JsonToken));
            if (token)
            {
                token->type = type;
                if (parent)
                    parent->next = token;
            }
            return token;
        }
        default:
        {
            assert(!"Bad token type in _EmitSimpleToken");
            return NULL;
        }
    }
}

static _JsonToken * _EmitBoolToken(_JsonToken *parent, bool value)
{
    _JsonToken *token = calloc(1, sizeof(_JsonToken));
    if (token)
    {
        token->type = _JSON_TOKEN_BOOL;
        token->val.valBool = value;
        if (parent)
            parent->next = token;
    }
    return token;
}

static _JsonToken * _EmitNumberToken(_JsonToken *parent, double value)
{
    _JsonToken *token = calloc(1, sizeof(_JsonToken));
    if (token)
    {
        token->type = _JSON_TOKEN_NUMBER;
        token->val.valDbl = value;
        if (parent)
            parent->next = token;
    }
    return token;
}

static _JsonToken * _EmitStringToken(_JsonToken *parent, const char *stringStart, size_t stringLength)
{
    _JsonToken *token = calloc(1, sizeof(_JsonToken));
    if (token)
    {
        token->type = _JSON_TOKEN_STRING;
        token->val.valString = calloc(1, stringLength+1);
        strncpy(token->val.valString, stringStart, stringLength);
        if (parent)
            parent->next = token;
    }
    return token;
}

static RedJsonObject _ParseObject(_JsonToken **head);
static RedJsonArray _ParseArray(_JsonToken **head);

static RedJsonValue _ParseValue(_JsonToken **head)
{
    RedJsonValue val;
    switch ((*head)->type)
    {
        case _JSON_TOKEN_OPEN_CURLY_BRACE:
        {
            RedJsonObject obj;
            obj = _ParseObject(head);
            /* TODO: error handling */
            val = RedJsonValue_FromObject(obj);
            return val;
        }
        case _JSON_TOKEN_OPEN_SQUARE_BRACE:
        {
            RedJsonArray array;
            array = _ParseArray(head);
            val = RedJsonValue_FromArray(array);
            return val;
        }
        case _JSON_TOKEN_STRING:
        {
            char *sz;
            sz = (*head)->val.valString;
            val = RedJsonValue_FromString(sz);
            *head = (*head)->next;
            return val;
        }
        case _JSON_TOKEN_NUMBER:
        {
            double dbl;
            dbl = (*head)->val.valDbl;
            val = RedJsonValue_FromNumber(dbl);
            *head = (*head)->next;
            return val;
        }
        case _JSON_TOKEN_BOOL:
        {
            bool b;
            b = (*head)->val.valBool;
            val = RedJsonValue_FromBoolean(b);
            *head = (*head)->next;
            return val;
        }
        case _JSON_TOKEN_NULL:
        {
            *head = (*head)->next;
            return RedJsonValue_Null();
        }
        default:
        {
            fprintf(stderr, "Value expected, got %d\n", (*head)->type);
            return NULL;
        }
    }
}

static RedJsonArray _ParseArray(_JsonToken **head)
{
    RedJsonArray array;

    array = RedJsonArray_New();

    if ((*head)->type != _JSON_TOKEN_OPEN_SQUARE_BRACE)
    {
        fprintf(stderr, "'[' expected at start of array\n");
        return NULL;
    }
    (*head) = (*head)->next;

    while ((*head)->type != _JSON_TOKEN_CLOSE_SQUARE_BRACE)
    {
        RedJsonValue val;
        /* consume value */
        val = _ParseValue(head);
        if (!val)
        {
            return NULL;
        }
        RedJsonArray_Append(array, val);

        if ((*head)->type != _JSON_TOKEN_CLOSE_SQUARE_BRACE)
        {
            if ((*head)->type != _JSON_TOKEN_COMMA)
            {
                fprintf(stderr, "',' expected between array values %d %d\n", (*head)->type, (*head)->next->type);
                return NULL;
            }
            (*head) = (*head)->next;
        }
    }
    (*head) = (*head)->next;

    return array;
}

static RedJsonObject _ParseObject(_JsonToken **head)
{
    RedJsonObject obj;
    char *key;
    obj = RedJsonObject_New();

    /* consume { */
    if ((*head)->type != _JSON_TOKEN_OPEN_CURLY_BRACE)
    {
        fprintf(stderr, "'{' expected at start of object\n");
        return NULL;
    }
    (*head) = (*head)->next;

    while ((*head)->type != _JSON_TOKEN_CLOSE_CURLY_BRACE)
    {
        RedJsonValue val;
        /* consume KEY */
        if ((*head)->type != _JSON_TOKEN_STRING)
        {
            fprintf(stderr, "'\"' expected at start of string key\n");
            return NULL;
        }
        key = (*head)->val.valString;
        (*head) = (*head)->next;

        /* consume : */
        if ((*head)->type != _JSON_TOKEN_COLON)
        {
            fprintf(stderr, "':' expected between key and value\n");
            return NULL;
        }
        (*head) = (*head)->next;

        val = _ParseValue(head);
        if (!val)
        {
            return NULL;
        }

        if ((*head)->type != _JSON_TOKEN_CLOSE_CURLY_BRACE)
        {
            if ((*head)->type != _JSON_TOKEN_COMMA)
            {
                fprintf(stderr, "',' expected between object properties %d %d\n", (*head)->type, (*head)->next->type);
                fprintf(stderr, "',' expected between object properties\n");
                return NULL;
            }
            (*head) = (*head)->next;
        }
        RedJsonObject_Set(obj, key, val);
    }
    (*head) = (*head)->next;
    return obj;
}

RedJsonObject RedJson_Parse(const char *text)
{
    _JsonToken *tokens = NULL, *tail = NULL;
    RedJsonObject out;
    /* Tokenize */
    while (*text)
    {
        switch (text[0])
        {
            case '{':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_OPEN_CURLY_BRACE);
                text++;
                break;
            case '}':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_CLOSE_CURLY_BRACE);
                text++;
                break;
            case '[':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_OPEN_SQUARE_BRACE);
                text++;
                break;
            case ']':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_CLOSE_SQUARE_BRACE);
                text++;
                break;
            case ',':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_COMMA);
                text++;
                break;
            case ':':
                tail = _EmitSimpleToken(tail, _JSON_TOKEN_COLON);
                text++;
                break;
            case 't':
                if (!strncmp(text, "true", 4))
                {
                    tail = _EmitBoolToken(tail, true);
                    text = &text[4];
                }
                else
                {
                    goto fail;
                }
                break;
            case 'f':
                if (!strncmp(text, "false", 5))
                {
                    tail = _EmitBoolToken(tail, false);
                    text = &text[5];
                }
                else
                {
                    goto fail;
                }
                break;
            case 'n':
                if (!strncmp(text, "null", 4))
                {
                    tail = _EmitSimpleToken(tail, _JSON_TOKEN_NULL);
                    text = &text[4];
                }
                else
                {
                    goto fail;
                }
                break;
            case '"':
            {
                // consume string
                const char *stringStart = text + 1;
                text = stringStart;
                while (*text && *text != '"')
                {
                    text++;
                }
                tail = _EmitStringToken(tail, stringStart, text - stringStart);

                if (!*text) {
                    /* unterminated parentheses */
                    goto fail;
                }

                /* TODO: handle end-of-input */
                text++;
                break;
            }
            case '/':
            {
                if (text[1] == '*')
                {
                    text++;
                    // consume comment
                    while (text[0] && text[1] && (text[0] != '*' || text[1] != '/'))
                    {
                        text++;
                    }

                    text += 2;
                }
                else
                {
                    goto fail;
                }
                break;
            }

            default:
            {
                if ((text[0] >= '0' && text[0] <= '9') || text[0] == '-')
                {
                    double val;
                    val = strtod(&text[0], (char **)&text);
                    tail = _EmitNumberToken(tail, val);
                    // TODO: handle exponentials

                }
                else if (isspace(text[0]))
                {
                    text++;
                }
                else
                {
                    goto fail;
                }
                break;
            }
        }
        if (tail && !tokens)
        {
            /* keep track of first token */
            tokens = tail;
        }
    }

    /* Parse into object tree */
    out = _ParseObject(&tokens);
    return out;
fail:
    fprintf(stderr, "Failure parsing JSON!");
    return NULL;
}

