/*
 Copyright (c) 2009 Dave Gamble

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef cJSONZJ__h
#define cJSONZJ__h
#include "stdint.h"

#ifndef CJSONOBJ_INT
#define CJSONOBJ_INT
#define int32 int32_t
#define uint32 uint32_t
#define int64 int64_t
#define uint64 uint64_t
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* cJSONZJTypes: */
#define cJSONZJ_False 0
#define cJSONZJ_True 1
#define cJSONZJ_NULL 2
#define cJSONZJ_Int 3
#define cJSONZJ_Double 4
#define cJSONZJ_String 5
#define cJSONZJ_Array 6
#define cJSONZJ_Object 7

#define cJSONZJ_IsReference 256

/* The cJSONZJstructure: */
typedef struct cJSONZJ
{
    struct cJSONZJ *next, *prev; /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSONZJ *child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    int type; /* The type of the item, as above. */

    char *valuestring; /* The item's string, if type==cJSONZJ_String */
    uint64 valueint; /* The item's number, if type==cJSONZJ_Number */
    double valuedouble; /* The item's number, if type==cJSONZJ_Number */
    int sign;   /* sign of valueint, 1(unsigned), -1(signed) */

    char *string; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSONZJ;

typedef struct cJSONZJ_Hooks
{
    void *(*malloc_fn)(size_t sz);
    void (*free_fn)(void *ptr);
} cJSONZJ_Hooks;

/* Supply malloc, realloc and free functions to cJSONZJ*/
extern void cJSONZJ_InitHooks(cJSONZJ_Hooks* hooks);

/* Supply a block of JSON, and this returns a cJSONZJobject you can interrogate. Call cJSONZJ_Delete when finished. */
extern cJSONZJ *cJSONZJ_Parse(const char *value);
/* Render a cJSONZJentity to text for transfer/storage. Free the char* when finished. */
extern char *cJSONZJ_Print(cJSONZJ *item);
/* Render a cJSONZJentity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char *cJSONZJ_PrintUnformatted(cJSONZJ *item);
/* Delete a cJSONZJentity and all subentities. */
extern void cJSONZJ_Delete(cJSONZJ *c);

/* Returns the number of items in an array (or object). */
extern int cJSONZJ_GetArraySize(cJSONZJ *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern cJSONZJ*cJSONZJ_GetArrayItem(cJSONZJ *array, int item);
/* Get item "string" from object. Case insensitive. */
extern cJSONZJ*cJSONZJ_GetObjectItem(cJSONZJ *object, const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSONZJ_Parse() returns 0. 0 when cJSONZJ_Parse() succeeds. */
extern const char *cJSONZJ_GetErrorPtr();

/* These calls create a cJSONZJitem of the appropriate type. */
extern cJSONZJ *cJSONZJ_CreateNull();
extern cJSONZJ *cJSONZJ_CreateTrue();
extern cJSONZJ *cJSONZJ_CreateFalse();
extern cJSONZJ *cJSONZJ_CreateBool(int b);
extern cJSONZJ *cJSONZJ_CreateDouble(double num, int sign);
extern cJSONZJ *cJSONZJ_CreateInt(uint64 num, int sign);
extern cJSONZJ *cJSONZJ_CreateString(const char *string);
extern cJSONZJ *cJSONZJ_CreateArray();
extern cJSONZJ *cJSONZJ_CreateObject();

/* These utilities create an Array of count items. */
extern cJSONZJ *cJSONZJ_CreateIntArray(int *numbers, int sign, int count);
extern cJSONZJ *cJSONZJ_CreateFloatArray(float *numbers, int count);
extern cJSONZJ *cJSONZJ_CreateDoubleArray(double *numbers, int count);
extern cJSONZJ *cJSONZJ_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
extern void cJSONZJ_AddItemToArray(cJSONZJ *array, cJSONZJ *item);
extern void cJSONZJ_AddItemToArrayHead(cJSONZJ *array, cJSONZJ *item);    /* add by Bwar on 2015-01-28 */
extern void cJSONZJ_AddItemToObject(cJSONZJ *object, const char *string,
                cJSONZJ *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSONZJto a new cJSON, but don't want to corrupt your existing cJSON. */
extern void cJSONZJ_AddItemReferenceToArray(cJSONZJ *array, cJSONZJ *item);
extern void cJSONZJ_AddItemReferenceToObject(cJSONZJ *object, const char *string,
                cJSONZJ *item);

/* Remove/Detatch items from Arrays/Objects. */
extern cJSONZJ *cJSONZJ_DetachItemFromArray(cJSONZJ *array, int which);
extern void cJSONZJ_DeleteItemFromArray(cJSONZJ *array, int which);
extern cJSONZJ *cJSONZJ_DetachItemFromObject(cJSONZJ *object, const char *string);
extern void cJSONZJ_DeleteItemFromObject(cJSONZJ *object, const char *string);

/* Update array items. */
extern void cJSONZJ_ReplaceItemInArray(cJSONZJ *array, int which, cJSONZJ *newitem);
extern void cJSONZJ_ReplaceItemInObject(cJSONZJ *object, const char *string,
                cJSONZJ *newitem);

#define cJSONZJ_AddNullToObject(object,name)	cJSONZJ_AddItemToObject(object, name, cJSONZJ_CreateNull())
#define cJSONZJ_AddTrueToObject(object,name)	cJSONZJ_AddItemToObject(object, name, cJSONZJ_CreateTrue())
#define cJSONZJ_AddFalseToObject(object,name)		cJSONZJ_AddItemToObject(object, name, cJSONZJ_CreateFalse())
#define cJSONZJ_AddNumberToObject(object,name,n)	cJSONZJ_AddItemToObject(object, name, cJSONZJ_CreateNumber(n))
#define cJSONZJ_AddStringToObject(object,name,s)	cJSONZJ_AddItemToObject(object, name, cJSONZJ_CreateString(s))


#ifdef __cplusplus
}
#endif

#endif
