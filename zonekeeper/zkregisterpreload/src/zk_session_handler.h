/*
 * filename      : ZKSessionHandler.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2013-09-17 14:29
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _ZKSESSIONHANDLER_H_
#define _ZKSESSIONHANDLER_H_
#include <zookeeper/zookeeper.h>
#include <string>
#include <stdint.h>

#include "base_header.h"
#include "ZKThreadSync.h"
class ZKSessionHandler
{
    const static int32_t MAX_PATH_LEN = 4096;
    const static int32_t MAX_DATA_LEN = 1024*1024;
public:
    ZKSessionHandler(const std::string& host_list, int recv_timeout);
    virtual ~ZKSessionHandler();
    //virtual int init()=0;
    virtual int process_seesion_expired_recovering()=0;
    bool connected;
protected:
    int zk_init();
    void zk_uninit();
    bool reConnected();
    bool waitForConnected(); 
    static void watcher(zhandle_t* zh, int type, int state, const char *path,void*watcherCtx);
    int create_znode(std::string &path, const std::string &value, int flag=0);
    int znode_exist(const std::string &path, bool &exist, bool watch);
    virtual int handle_create(zhandle_t *zzh, int type, int state, const char *path,void* context);

    virtual int handle_delete(zhandle_t *zzh, int type, int state, const char *path,void* context);

    virtual int handle_change(zhandle_t *zzh, int type, int state, const char *path,void* context);

    virtual int handle_child(zhandle_t *zzh, int type, int state, const char *path,void* context);

    virtual int handle_session(zhandle_t *zzh, int type, int state, const char *path,void* context);

    virtual int handle_notwatching(zhandle_t *zzh, int type, int state, const char *path,void* context);


    static const char* type2string(int type);

    static const char* state2string(int state);

    static std::string dump_stat(const struct Stat *stat);


    std::string dump_stat(const struct Stat &stat);

    private:
    ZKSessionHandler(const ZKSessionHandler& zkrs);
    ZKSessionHandler* operator=(const ZKSessionHandler &zkrs);
    Synchronized m_sync;

    std::string m_hosts_list;
    int m_recv_timeout;
    zhandle_t* m_zh;
    clientid_t m_session_id;
    //Synchronized m_sync;                // sync init process, init is sync!
    LOG_CLS_DEC();

};
#endif
