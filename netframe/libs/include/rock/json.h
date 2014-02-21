/**
 * @file        json.h
 * @author      maxingsong
 * @date        2013-12-08
 * @version     v1.0.0
 *
 * @brief       json库
 *
 */

#ifndef _JSON_H_
#define _JSON_H_

#include <stdbool.h>
#include "cjson.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cJSON json_struct;
typedef struct cJSON_Hooks json_hooks;

/**
 * @brief Supply malloc, realloc and free functions to cJSON
 *
 * @param hooks :malloc / free hooks
 */
extern void json_init_hooks(json_hooks * hooks);

/**
 * @brief Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. 
 *
 * @param value :to be parsed data
 *
 * @return json_struct
 */
extern json_struct *json_parse(const char *value);

/**
 * @brief Render a cJSON entity to text for transfer/storage. Free the char* when finished.
 *
 * @param item :json_struct item
 *
 * @return item string
 */
extern char *json_print(json_struct * item);

/**
 * @brief Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished.
 *
 * @param item :json_struct item
 *
 * @return :item string
 */
extern char *json_print_unformatted(json_struct * item);

/**
 * @brief Delete a cJSON entity and all subentities.
 *
 * @param c :json_struct item
 *
 * @return true means delete done, otherwise false
 */
extern int json_delete(json_struct * c);

/**
 * @brief Returns the number of items in an array (or object).
 *
 * @param array :json_struct
 *
 * @return the number of items in the array
 */
extern int json_get_array_size(json_struct * array);

/**
 * @brief Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
 *
 * @param array :json_struct
 * @param item :index number
 *
 * @return json_struct item data
 */
extern json_struct *json_get_array_item(json_struct * array, int item);

/**
 * @brief Get item "string" from object. Case insensitive.
 *
 * @param object :json_struct
 * @param string :the item string
 *
 * @return json_struct item data
 */
extern json_struct *json_get_object_item(json_struct * object,
                                             const char *string);

/**
 * @brief Get null value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_null_from_object(json_struct * object,
                                          const char *string);

/**
 * @brief Get true value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_true_from_object(json_struct * object,
                                          const char *string);

/**
 * @brief Get false value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_false_from_object(json_struct * object,
                                           const char *string);

/**
 * @brief Get int value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 * @param retVal :the return int value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_int_from_object(json_struct * object,
                                         const char *string, int *retVal);
/**
 * @brief Get double value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 * @param retVal :the return double value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_double_from_object(json_struct * object,
                                            const char *string, double *retVal);

/**
 * @brief Get string value from object(parent item) according to string
 *
 * @param object :json_struct object
 * @param string :the item string
 * @param retVal :the return string value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_string_from_object(json_struct * object,
                                            const char *string, char **retVal);

/**
 * @brief get name data form item
 *
 * @param item :json_struct item
 * @param retVal :the return string value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_name_from_item(json_struct * item, char **retVal);

/**
 * @brief get null data form item
 *
 * @param item :json_struct item
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_null_from_item(json_struct * item);

/**
 * @brief get true data form item
 *
 * @param item :json_struct item
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_true_from_item(json_struct * item);

/**
 * @brief get false data form item
 *
 * @param item :json_struct item
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_false_from_item(json_struct * item);

/**
 * @brief get int data form item
 *
 * @param item :json_struct item
 * @param retVal :the return int value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_int_from_item(json_struct * item, int *retVal);

/**
 * @brief get double data form item
 *
 * @param item :json_struct item
 * @param retVal :the return double value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_double_from_item(json_struct * item, double *retVal);

/**
 * @brief get string data form item
 *
 * @param item :json_struct item
 * @param retVal :the return string value
 *
 * @return  true means have gotten the value, otherwise false
 */
extern int json_get_string_from_item(json_struct * item, char **retVal);

/**
 * @brief For analysing failed parses.
 *
 * @return This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds.
 */
extern const char *json_get_error_ptr(void);

/**
 * @brief create a cJSON item of null type
 *
 * @return json_struct item
 */
extern json_struct *json_create_null(void);

/**
 * @brief create a cJSON item of true type
 *
 * @return json_struct item
 */
extern json_struct *json_create_true(void);

/**
 * @brief create a cJSON item of false type
 *
 * @return json_struct item
 */
extern json_struct *json_create_false(void);

/**
 * @brief create a cJSON item of bool type
 *
 * @return json_struct item
 */
extern json_struct *json_create_bool(int b);

/**
 * @brief create a cJSON item of int type
 *
 * @return json_struct item
 */
extern json_struct *json_create_int(int b);

/**
 * @brief create a cJSON item of double type
 *
 * @return json_struct item
 */
extern json_struct *json_create_double(double b);

/**
 * @brief create a cJSON item of string type
 *
 * @return json_struct item
 */
extern json_struct *json_create_string(const char *string);

/**
 * @brief create a cJSON item of array type
 *
 * @return json_struct item
 */
extern json_struct *json_create_array(void);

/**
 * @brief create a cJSON item of object type
 *
 * @return json_struct item
 */
extern json_struct *json_create_object(void);

/* These utilities create an Array of count items. */

/**
 * @brief create an array type with count int items
 *
 * @param numbers :each number in each item
 * @param count :items count
 *
 * @return json_struct list
 */
extern json_struct *json_create_int_array(const int *numbers, int count);

/**
 * @brief create an array type with count float items
 *
 * @param numbers :each number in each item
 * @param count :items count
 *
 * @return json_struct list
 */
extern json_struct *json_create_float_array(const float *numbers,
                                            int count);

/**
 * @brief create an array type with count double items
 *
 * @param numbers :each number in each item
 * @param count :items count
 *
 * @return json_struct list
 */
extern json_struct *json_create_double_array(const double *numbers,
                                             int count);
/**
 * @brief create an array type with count string items
 *
 * @param numbers :each string in each item
 * @param count :items count
 *
 * @return json_struct list
 */
extern json_struct *json_create_string_array(const char **strings,
                                             int count);

/* Append item to the specified array/object. */
/**
 * @brief append item to the specified array
 *
 * @param array :target array
 * @param item :the json_struct item
 *
 * @return true when appending successful, otherwise false
 */
extern int json_add_item_to_array(json_struct * array, json_struct * item);

/**
 * @brief append item to the specified object
 *
 * @param array :target json_struct object
 * @param string :item string value
 * @param item :the json_struct item
 *
 * @return true when appending successful, otherwise false
 */
extern int json_add_item_to_object(json_struct * object,
                                        const char *string, json_struct * item);

/**
 * @brief Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON.
 *
 * @param array :target json_struct array
 * @param item :reference item
 *
 * @return true when appending successful, otherwise false
 */
extern int json_add_item_reference_to_array(json_struct * array,
                                             json_struct * item);
extern int json_add_item_reference_to_object(json_struct * object,
                                              const char *string,
                                              json_struct * item);

/**
 * @brief Remove/Detatch items from Arrays/Objects.
 *
 * @param array :target json_struct array
 * @param which :item index
 *
 * @return remove/Detatch item
 */
extern json_struct *json_detach_item_from_array(json_struct * array,
                                                int which);
extern int json_delete_item_from_array(json_struct * array, int which);
extern json_struct *json_detach_item_from_object(json_struct * object,
                                                 const char *string);
extern int json_delete_item_from_object(json_struct * object,
                                         const char *string);

/**
 * @brief Update array items.
 *
 * @param array :target json_struct array
 * @param which :item index
 * @param newitem :new item
 *
 * @return true when update successful, otherwise false
 */
extern int json_replace_item_in_array(json_struct * array, int which,
                                       json_struct * newitem);
extern int json_replace_item_in_object(json_struct * object,
                                        const char *string,
                                        json_struct * newitem);

/* Duplicate a cJSON item */
/**
 * @brief Duplicate a cJSON item
 *
 * @param item :the item
 * @param recurse :recurse mode, 0 means non-recursive, >0 means retrieve
 *
 * @return a new Duplicate item
 * @note Duplicate will create a new, identical cJSON item to the one you pass,
 * in new memory that will need to be released. With recurse!=0, it will
 * duplicate any children connected to the item.The item->next and ->prev
 * pointers are always zero on return from Duplicate.
 */
extern json_struct *json_duplicate(json_struct * item, int recurse);

/**
 * @brief ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed.
 *
 * @param value
 * @param return_parse_end
 * @param require_null_terminated
 *
 * @return
 */
extern json_struct *json_parse_with_opts(const char *value,
                                         const char **return_parse_end,
                                         int require_null_terminated);
extern void json_minify(char *json);

/**
 * @brief adding data to object
 *
 * @param object :target object
 * @param name :item string value
 *
 * @return true when successful, otherwise false
 */
extern int json_add_null_to_object(json_struct * object, const char *name);
extern int json_add_true_to_object(json_struct * object, const char *name);
extern int json_add_false_to_object(json_struct * object,
                                     const char *name);
extern int json_add_bool_to_object(json_struct * object, const char *name,
                                    bool b);
extern int json_add_int_to_object(json_struct * object, const char *name,
                                   int n);
extern int json_add_double_to_object(json_struct * object,
                                      const char *name, double d);
extern int json_add_string_to_object(json_struct * object,
                                      const char *name, const char *s);

#ifdef __cplusplus
}
#endif
#endif /* ! _JSON_H_ */
