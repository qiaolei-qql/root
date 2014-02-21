/* Copyright(c). xunlei corporation, All Right Reserved
 */
/**
 * @file rock_hash.h
 * @author maxingsong, <maxingsong@xunlei.com>
 * @version 0.1
 * @date 2013-12-10
 * @Synopsis
 *      encapsulate ut_hash
 *      this file consists of two part: Converience Macros and Genernal Macros
 *
 *      Tips:
 *          1. Once a structure has been added to the hash, do not change the value of its key.
 *          2. it's your duty to check the uniqueness before adding the key to the hash.
 *          3. It is an error to add two items with the same key to the hash table. but uthash
 *             will not give you an error code or throw an exception. so obey the order 2.
 *          4. Deleting a structure just removes it from the hash table-- it doesn’t free it. T
 *             the choice of when to free your structure is entirely up to you; uthash will never
 *             free your structure.
 *          5. for more information, visit <http://troydhanson.github.io/uthash/userguide.html>
 */
#ifndef _ROCK_HASH_H_
#define _ROCK_HASH_H_

#include "uthash.h"

/*------------------------------------------------------------------------------------------*/
/*-------------------------------------Convenience Macros-----------------------------------*/
/*      Convenience macros:                                                                 */
/*      The convenience macros do the same thing as the generalized macros,                 */
/*      but require fewer arguments.                                                        */
/*      In order to use the convenience macros,                                             */
/*          1. the structure's UT_hash_handle field must be named hh                        */
/*          2. for add or find, the key field must be of type int or char[] or pointer      */
/*------------------------------------------------------------------------------------------*/

/**
 * scenario:
 *      typedef struct {
 *          int key;
 *          UT_hash_handle hh;
 *      };
 */
#define hash_add_int(head, keyfield_name, item_ptr)                         \
    HASH_ADD_INT(head, keyfield_name, item_ptr)

#define hash_replace_int(head, keyfield_name, item_ptr, replaced_item_ptr)  \
    HASH_REPLACE_INT(head, keyfield_name, item_ptr, replaced_item_ptr)

#define hash_find_int(head, key_ptr, item_ptr)                              \
    HASH_FIND_INT(head, key_ptr, item_ptr)

/**
 * scenario:
 *      typedef struct {
 *          char name[10];
 *          UT_hash_handle hh;
 *      };
 *
 */
#define hash_add_str(head, keyfield_name, item_ptr)                         \
    HASH_ADD_STR(head, keyfield_name, item_ptr)

#define hash_replace_str(head, keyfield_name, item_ptr, replaced_item_ptr)  \
    HASH_REPLACE_STR(head,keyfield_name, item_ptr, replaced_item_ptr)

#define hash_find_str(head, key_ptr, item_ptr)                              \
    HASH_FIND_STR(head, key_ptr, item_ptr)

/**
 * scenario:
 *      typedef struct {
 *          void *key;
 *          UT_hash_handle hh;
 *      };
 */
#define hash_add_ptr(head, keyfield_name, item_ptr)                         \
    HASH_ADD_PTR(head, keyfield_name, item_ptr)

#define hash_replace_ptr(head, keyfield_name, item_ptr, replaced_item_ptr)  \
    HASH_REPLACE_PTR(head, keyfield_name, item_ptr, replaced_item_ptr)

#define hash_find_ptr(head, key_ptr, item_ptr)                              \
    HASH_FIND_PTR(head, key_ptr, item_ptr)

#define hash_del(head, item_ptr)                                            \
    HASH_DEL(head, item_ptr)

#define hash_sort(head, cmp)                                                \
    HASH_SORT(head, cmp)

#define hash_count(head)                                                    \
    HASH_COUNT(head)


/*------------------------------------------------------------------------------------------*/
/*-------------------------------------General Macros---------------------------------------*/
/*      General macros:                                                                     */
/*          These macros add, find, delete and sort the items in a hash. You need to use    */
/*          the general macros if your UT_hash_handle is named something other than hh, or  */
/*          if your key’s data type isn’t int or char[].  or pointer                      */
/*------------------------------------------------------------------------------------------*/

#define hash_add(hh_name, head, keyfield_name, key_len, item_ptr)           \
    HASH_ADD(hh_name, head, keyfield_name, key_len, item_ptr)

/**
 * scenario:
 *      typedef struct{
 *          char *name;
 *          UT_hash_handle hh;
 *      }
 */
#define hash_add_keyptr(hh_name, head, key_ptr, key_len, item_ptr)          \
    HASH_ADD_KEYPTR(hh_name, head, key_ptr, key_len, item_ptr)

#define hash_replace(hh_name, head, keyfield_name, key_len, item_ptr, replaced_item_ptr)    \
    HASH_REPLACE(hh_name, head, keyfield_name, key_len, item_ptr, replaced_item_ptr)

#define hash_find(hh_name, head, key_ptr, key_len, item_ptr)                \
    HASH_FIND(hh_name, head, key_ptr, key_len, item_ptr)

#define hash_delete(hh_name, head, item_ptr)                                \
    HASH_DELETE(hh_name, head, item_ptr)

#define hash_srt(hh_name, head, cmp)                                        \
    HASH_SRT(hh_name, head, cmp)

#define hash_cnt(hh_name, head)                                             \
    HASH_CNT(hh_name, head)

#define hash_clear(hh_name, head)                                           \
    HASH_CLEAR(hh_name, head)

#define hash_select(dst_hh_name, dst_head, src_hh_name, src_head, condition) \
    HASH_SELECT(dst_hh_name, dst_head, src_hh_name, src_head, condition)

#define hash_iter(hh_name, head, item_ptr, tmp_item_ptr)                    \
    HASH_ITER(hh_name, head, item_ptr, tmp_item_ptr)

#define hash_overhead(hh_name, head)                                        \
    HASH_OVERHEAD(hh_name, head)

#endif /* ! _ROCK_HASH_H_ */
/* vim: set ts=4 sw=4: */

