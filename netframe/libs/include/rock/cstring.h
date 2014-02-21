/**
 * @file        cstring.h
 * @author      luogang (luogang@xunlei.com)
 * @date        2014-01-26
 * @version     v1.0.0
 *
 * @brief       C semantic string library
 *
 * @todo        Use slab allocator for test unit
 */


#ifndef _CSTRING_H_
#define _CSTRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "darray.h"

/**
 * @brief Convert contents of str to upper case.
 *
 * @param str user string.
 *
 * @return Uppered string pointer. When user string is NULL, NULL will returned.
 */
char * toupperstr(char *str);

/**
 * @brief Convert contents of str to lower case.
 *
 * @param str user string.
 *
 * @return Lowered string pointer.  When user string is NULL, NULL will returned.
 */
char * tolowerstr(char *str);


/**
 * @brief Delete whitespace contiguous from the left end of the string.
 *
 * @param str user string.
 *
 * @return Trimed string
 */
char * ltrimws(char * str);

/**
 * @brief Delete whitespace contiguous from the right end of the string.
 *
 * @param str user string.
 *
 * @return Trimed string
 */
char * rtrimws (char * str);

/**
 * @brief Delete whitespace contiguous from both ends of the string.
 *
 * @param str user string.
 *
 * @return Trimed string
 */
char * trimws (char * str);

/**
 * @brief reverse the contents of str in place
 *
 * @param user string
 *
 * @return reversed string
 */
char * reverse(char * str);

 /**
 * @brief compare haystack string startswith needle
 *
 * @param haystack the checked string
 * @param needle startswith string
 *
 * @return return true if haystack string starts with needle, otherwise return false
 */
bool startswith(const char * __restrict haystack, const char * __restrict needle);

/**
 * @brief compare haystack endswith needle
 *
 * @param haystack the checked string
 * @param needle endswith string
 *
 * @return return true if haystack string ends with needle, otherwise return false
 */
bool endswith(const char * __restrict haystack, const char * __restrict needle);

/**
 * @brief find the index of the first occurrence of ch character in haystack string
 *
 * @param haystack :the searched string
 * @param ch :the character
 *
 * @return return the index of the first occurrence
 */
int indexof(const char * haystack, char ch);

/**
 * @brief find the index of thr first occurrence of needle string in haystack
 *
 * @param haystack :the searched string
 * @param needle :the string
 *
 * @return return the index of thr first occurrence
 */
int indexofstr(const char * __restrict haystack, const char * __restrict needle);

/**
 * @brief find the index of the last occurrence of ch character in haystack
 *
 * @param haystack :the searched string
 * @param ch :the character
 *
 * @return return the index of the last occurrence
 */
int rindexof(const char * haystack, char ch);

/**
 * @brief 用 replacement 字符替换 haystack 中首个target字符
 *
 * @param haystack :待处理字符串
 * @param target :将被替换的字符
 * @param replacement :用于替换的字符
 *
 * @return 替换后的字符串
 */
char *replace_ch_first(char * haystack, char target, char replacement);

/**
 * @brief 用 replacement 字符替换 haystack 中所有的 target 字符
 *
 * @param haystack :待处理字符串
 * @param target :将被替换的字符
 * @param replacement :用于替换的字符
 *
 * @return 替换后的字符串
 */
char *replace_ch_all(char * haystack, char target, char replacement);

/**
 * @brief 用 replacement 字符串替换 haystack 中首个子串 target
 *
 * @param haystack :待处理字符串
 * @param target :将被替换的子串
 * @param replacement :用于替换的字符串
 *
 * @return 替换后的新字符串
 * @note 若when len(target) < len(replacement),返回申请的新字符串，否则在原字符串空间替换
 */
char *replace_str_first(char *haystack, const char *__restrict target, \
                        const char *__restrict replacement);
/**
 * @brief 用 replacement 字符串替换 haystack 中所有子串 target
 *
 * @param haystack :待处理字符串
 * @param target :将被替换的子串
 * @param replacement :用于替换的字符串
 *
 * @return 替换后的新字符串
 * @note 若when len(target) < len(replacement),返回申请的新字符串，否则在原字符串空间替换
 */
char *replace_str_all(char *haystack, const char *__restrict target, \
                      const char *__restrict replacement);

struct substr_item_t
{
    char * substr;
};

/**
 * @brief 以字符 needle 分割字符串 haystack
 *
 * @param haystack :待处理字符串
 * @param needle :分割字符
 *
 * @return 子字符串数组
 */
darray split(const char *haystack, const char needle);

/**
 * @brief 以字符串 split_str 中任意字符分割字符串 haystack
 *
 * @param haystack :待处理字符串
 * @param needle :分割字符
 *
 * @return 子字符串数组
 */
darray splits(const char *__restrict haystack, const char *__restrict split_str);

/**
 * @brief 以字符串 split_str 分割 haystack
 *
 * @param haystack :待处理字符串
 * @param needle :分割字符
 *
 * @return 子字符串数组
 */
darray splitstr(const char *__restrict haystack, const char *__restrict split_str);


#ifdef __cplusplus
}
#endif

#endif /* _CSTRING_H_ */
