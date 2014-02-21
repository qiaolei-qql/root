/**
 * @file        aes.h
 * @author      王卫华
 * @date        2009-04-09
 * @version     v1.0.0
 *
 * @brief       aes加/解密库
 */


/**************************** 条件编译选项和头文件 ****************************/
#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include <stdbool.h>

typedef int32_t int32;
typedef uint32_t uint32;
typedef unsigned char byte;

/********************************** 常量和宏 **********************************/
#define AES_ENCRYPT     1
#define AES_DECRYPT     0
#define AES_BLOCK_SIZE 16
/********************************** 数据类型 **********************************/
typedef struct aes_context
{
	int32 nr;           /*!<  number of rounds  */
	uint32 *rk;          /*!<  AES round keys    */
	uint32 buf[68];      /*!<  unaligned data    */
} aes_context;

#ifdef __cplusplus
extern "C" {
#endif

/********************************** 函数声明 **********************************/

/**
 * @brief 设置加密的密钥和加密强度
 *
 * @param ctx :aes_context对象
 * @param key :密钥
 * @param key_bitsize :密钥的位数，有效数为128、192、256
 *
 * @return 密钥设置成功返回0，否则返回-1
 */
int32 aes_setkey_enc(aes_context* ctx, const byte* key, int32 key_bitsize);

/**
 * @brief 设置解密的密钥和解密强度
 *
 * @param ctx :aes_context对象
 * @param key :密钥
 * @param key_bitsize :密钥的位数，有效数为128、192、256
 *
 * @return 密钥设置成功返回0，否则返回-1
 */
int32 aes_setkey_dec(aes_context* ctx, const byte* key, int32 key_bitsize);

/**
 * @brief 加密/解密处理
 *
 * @param ctx :aes_context对象
 * @param mode :加密/解密模式,AES_ENCRYPT / AES_DECRYPT
 * @param input[16] :被加密/解密的数据
 * @param output[16] :加密/解密处理后的数据
 */
void aes_crypt_ecb(aes_context* ctx, int32 mode, const byte input[16], byte output[16]);

/**
 * @brief 加密处理（会调整加密数据长度，保证加密数据为16个字节的整数倍，因此要求输出缓存足够大以防止写越界）
 *
 * @param ctx :aes_context对象
 * @param input :被加密数据指针
 * @param inlen :被加密数据大小
 * @param from :从哪个位置开始加密
 * @param output :保存加密结果的缓存指针
 * @param outlen :保存加密结果的缓存大小，并保存加密后的数据长度(由于加密后数据会变长，为了避免内存越界写错误，建议outlen >= inlen + 16)
 *
 * @return 加密正确返回true，否则返回false
 */
bool aes_encrypt(aes_context* ctx, const byte* input, uint32 inlen, uint32 from, byte* output, uint32 *outlen);

/**
 * @brief 解密处理（由于加密过程中调整了加密数据长度，因此解密后数据长度变短）
 *
 * @param ctx :aes_context对象
 * @param input :被解密数据缓存指针
 * @param inlen :被解密缓存大小
 * @param from :从哪个位置开始解密
 * @param output :保存解密结果的缓存指针
 * @param outlen :保存解密结果的缓存大小，并保存解密后的数据长度(由于解密后数据会变短，为了避免内存越界写错误，建议*outlen >= inlen)
 *
 * @return 解密正确返回true，否则返回false
 */
bool aes_decrypt(aes_context* ctx, const byte* input, uint32 inlen, uint32 from, byte* output, uint32 *outlen);

/*********************************** 类定义 ***********************************/
#ifdef __cplusplus
}
#endif

#endif /* ! _AES_H_  */
