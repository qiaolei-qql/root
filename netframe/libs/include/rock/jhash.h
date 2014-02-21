/**
 * @file        jhash.h
 * @author      jingmi@gmail.com
 * @date        2013-12-17
 * @version     v1.0.0
 *
 * @brief       jhash
 */


#ifndef _JHASH_H_
#define _JHASH_H_

#include <stdint.h>

/**
 * @brief calculate data jhash
 *
 * @param key :data to jhash
 * @param key_len :data len
 *
 * @return jhash result
 */
static inline uint32_t jhash(const void *key, size_t key_len)
{
    /* jenkins_one_at_a_time_hash */
    uint32_t hash = 0;
    size_t i;
    const unsigned char *k = key;

    for (i = 0; i < key_len; i++)
    {
        hash += k[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

#endif /* ! _JHASH_H_ */
