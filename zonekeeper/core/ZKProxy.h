#ifndef __ZK_PROXY_H__
#define __ZK_PROXY_H__

#include <zookeeper.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <common/ZKLogger.h>
#include <common/ZKUtility.h>
#include <common/ZKThreadSync.h>
#include <string>
#include <set>


// wrapper zookeeper raw sync api
class ZKProxy: private boost::noncopyable
{
	static const int MAX_PATH_LEN = 256;
	static const int MAX_DATA_LEN = 1048576;		// max 1M
	
public:
	ZKProxy(std::string host, int recv_to_ms);

	virtual ~ZKProxy();

	virtual bool init() = 0;	
		
protected:	
	int zk_init();

	void zk_uninit();

	int create_znode(std::string &path, const std::string &value, int flag = 0);

	int delete_znode(const std::string &path, int32_t version);

	int znode_exist(const std::string &path, bool &exist, bool watch);

	int get_znode_data(const std::string &path, std::string &value, bool watch, int32_t &version);

	int set_znode_data(const std::string &path, const std::string &value, int32_t version);

	int get_znode_children(const std::string &path, std::set<std::string> &children, bool watch);

	static void watcher(zhandle_t *zzh, int type, int state, const char *path,void* context);
		
	virtual int handle_create(zhandle_t *zzh, int type, int state, const char *path,void* context);

	virtual int handle_delete(zhandle_t *zzh, int type, int state, const char *path,void* context);

	virtual int handle_change(zhandle_t *zzh, int type, int state, const char *path,void* context);

	virtual int handle_child(zhandle_t *zzh, int type, int state, const char *path,void* context);

	virtual int handle_session(zhandle_t *zzh, int type, int state, const char *path,void* context);

	virtual int handle_notwatching(zhandle_t *zzh, int type, int state, const char *path,void* context);


	static const char* type2string(int type);

	static const char* state2string(int state);

	static std::string dump_stat(const struct Stat *stat);
	
private:
	std::string m_zk_host_port;	      // comma separated host:port pairs, like 127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002
	Synchronized m_sync;				// sync init process, init is sync!
	
	zhandle_t * m_zh;				// zk handle
	clientid_t m_myid;				// 

	int m_recv_timeout;				// timeout for the session

    DECL_LOGGER(logger);

};

typedef boost::shared_ptr<ZKProxy> ZKP_ptr;

#endif	// __ZK_PROXY_H__


