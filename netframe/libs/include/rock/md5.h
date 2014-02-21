/**
 * @file        md5.h
 * @date        2014-02-08
 * @version     v1.0.0
 *
 * @brief       md5库
 *
 */


#ifndef _MD5_H_
#define _MD5_H_

#include <stdint.h>

/**
 * @brief          MD5 context structure
 */
typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[4];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}md5_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          MD5 context setup
 *
 * @param ctx      context to be initialized
 */
void md5_starts(md5_context *ctx);

/**
 * @brief          MD5 process buffer
 *
 * @param ctx      MD5 context
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 */
void md5_update(md5_context *ctx, const unsigned char *input, uint32_t ilen);

/**
 * @brief          MD5 final digest
 *
 * @param ctx      MD5 context
 * @param output   MD5 checksum result
 */
void md5_finish(md5_context *ctx, unsigned char output[16]);

/**
 * @brief          Output = MD5(input buffer)
 *
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 * @param output   MD5 checksum result
 */
void md5_data(const unsigned char *input, uint32_t ilen, unsigned char output[16]);

/**
 * @brief          Output = MD5(file contents)
 *
 * @param path     input file name
 * @param output   MD5 checksum result
 *
 * @return         0 if successful, 1 if fopen failed,
 *                 or 2 if fread failed
 */
uint32_t md5_file(const char *path, unsigned char output[16]);

/**
 * @brief          MD5 HMAC context setup
 *
 * @param ctx      HMAC context to be initialized
 * @param key      HMAC secret key
 * @param keylen   length of the HMAC key
 */
void md5_hmac_starts(md5_context *ctx, const unsigned char *key, uint32_t keylen);

/**
 * @brief          MD5 HMAC process buffer
 *
 * @param ctx      HMAC context
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 */
void md5_hmac_update(md5_context *ctx, const unsigned char *input, uint32_t ilen);

/**
 * @brief          MD5 HMAC final digest
 *
 * @param ctx      HMAC context
 * @param output   MD5 HMAC checksum result
 */
void md5_hmac_finish(md5_context *ctx, unsigned char output[16]);

/**
 * @brief          Output = HMAC-MD5(hmac key, input buffer)
 *
 * @param key      HMAC secret key
 * @param keylen   length of the HMAC key
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 * @param output   HMAC-MD5 result
 */
void md5_hmac_data(const unsigned char *key, uint32_t keylen,
                    const unsigned char *input, uint32_t ilen,
                    unsigned char output[16]);

#ifdef __cplusplus
}
#endif

#endif /*  ! _MD5_H_  */
