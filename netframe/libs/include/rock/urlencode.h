/**
 * @file        urlencode.h
 * @author      jingmi@gmail.com
 * @date        2014-01-09
 * @version     v1.0.0
 *
 * @brief       urlencode / urldecode 
 */

#ifndef _URLENCODE_H_
#define _URLENCODE_H_

/**
 * @brief url encode input_string
 *
 * @param input_string :input_string
 *
 * @return a new malloced string returned
 */
char *url_encode(const char *input_string);

/*
 * return a new malloced string.
 */
/**
 * @brief  decode url string
 *
 * @param input_string :input_string
 *
 * @return a new malloced string returned
 */
char *url_decode(const char *input_string);

#endif /* ! _URLENCODE_H_ */
/* vim: set expandtab tabstop=4 shiftwidth=4 foldmethod=marker: */
