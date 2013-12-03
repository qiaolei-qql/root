#include "ZKProxy.h"
#include <boost/assert.hpp>

using namespace std;

IMPL_LOGGER(ZKProxy, logger);


ZKProxy::ZKProxy(std::string host, int recv_to_ms)
:m_zk_host_port(host),m_recv_timeout(recv_to_ms)
{		
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
}


ZKProxy::~ZKProxy()
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
	
	zk_uninit();
}

	
int ZKProxy::zk_init()
{
	#ifdef _DEBUG
	LOG4CPLUS_DEBUG(logger, "set zookeeper log level:" << "DEBUG");
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	#endif
	
	// try to establish zk session 
	m_zh = zookeeper_init(m_zk_host_port.c_str(), watcher, m_recv_timeout, 0, this, 0);		// this as context parameter
	LOG4CPLUS_DEBUG(logger, "init zookeeper, host:" << m_zk_host_port << " context:" << this);
	
    	if (!m_zh)
	{
		LOG4CPLUS_ERROR(logger, "zookeeper_init failed");
		return -1;
    	}

	// block to wait for finishing establishing zk session 
	if(!m_sync.wait(2*m_recv_timeout))		// timeout
	{
		LOG4CPLUS_ERROR(logger, "zookeeper handshake finish, TIMEOUT!");
		return -1;
	}
	else if(!m_zh)						// unnormal events
	{
		LOG4CPLUS_ERROR(logger, "zookeeper handshake finish, session establish failed!");
		return -1;
	}
	
	LOG4CPLUS_DEBUG(logger, "zookeeper handshake finish, session establish succ!");
	return 0;
}


void ZKProxy::zk_uninit()
{	
	LOG4CPLUS_DEBUG(logger, "zk_uninit, there MUST are exception events happend, close zookeeper session, handle:" << m_zh);
	
	zookeeper_close(m_zh);	// zookeeper_close  session
	m_zh = NULL;

    //已经解决了时间同步的问题，现屏蔽掉强制退出。
    //abort();
}


// flag 0|ZOO_EPHEMERAL|ZOO_SEQUENCE
int ZKProxy::create_znode(std::string &path, const std::string &value, int flag)
{
	char path_buff[MAX_PATH_LEN];
	int rc = zoo_create(m_zh, path.c_str(), value.c_str(), value.length(), &ZOO_OPEN_ACL_UNSAFE, flag, path_buff, MAX_PATH_LEN);
	LOG4CPLUS_DEBUG(logger, "create znode path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");

	if(ZOK == rc || ZNODEEXISTS == rc)
	{
		if(ZOK == rc && flag&ZOO_SEQUENCE)
		{
			path = string(path_buff);
		}
		
		LOG4CPLUS_DEBUG(logger, "create znode path:" << path << " ok or it has exist, rc:" << rc);
		return 0;
	}
	
	LOG4CPLUS_WARN(logger, "create znode path:" << path << " failed!, rc:" << rc);		
	return -1;
}


int ZKProxy::delete_znode(const std::string &path, int32_t version)
{
	int rc = zoo_delete(m_zh, path.c_str(), version);
	LOG4CPLUS_DEBUG(logger, "delete znode path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");	
	
	if(ZOK == rc)
	{
		LOG4CPLUS_DEBUG(logger, "delete znode path:" << path << " succ");	
		return 0;
	}

	LOG4CPLUS_WARN(logger, "delete znode path:" << path << " failed!, rc:" << rc);		
	return -1;
}


int ZKProxy::znode_exist(const std::string &path, bool &exist, bool watch)
{
	struct Stat stat;
	int rc = zoo_exists(m_zh, path.c_str(), watch == true ? 1 : 0, &stat);
	LOG4CPLUS_DEBUG(logger, "check exist znode path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");	
	
	if(ZOK == rc)
	{
		LOG4CPLUS_DEBUG(logger, "check exist znode path:" << path << " exist, node stat:" << dump_stat(&stat));	
		exist = true;
		return 0;	
	}
	else if(ZNONODE == rc)
	{
		LOG4CPLUS_DEBUG(logger, "check exist znode path:" << path << " not exist");
		exist = false;
		return 0;
	}

	LOG4CPLUS_WARN(logger, "check exist znode path:" << path << " failed!, rc:" << rc);
	return -1;
	
}


int ZKProxy::get_znode_data(const std::string &path, std::string &value, bool watch, int32_t &version)
{
	struct Stat stat;
	int buffer_len = MAX_DATA_LEN;
	char *buffer=new char[MAX_DATA_LEN];

	int rc = zoo_get(m_zh, path.c_str(), watch == true ? 1 : 0, buffer, &buffer_len, &stat);
	LOG4CPLUS_DEBUG(logger, "get znode data path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");
	
	if(ZOK == rc)
	{
		value = std::string(buffer,buffer_len);
		version = stat.version;
        if(buffer!=NULL)
        {
            delete []buffer;
            buffer=NULL;
        }
		LOG4CPLUS_DEBUG(logger, "get znode data path:" << path << " data:" << value << " version:" << version << " stat:" << dump_stat(&stat));
		return 0;
	}

	LOG4CPLUS_WARN(logger, "get znode data path:" << path << " failed!, rc:" << rc);
    if(buffer!=NULL)
    {
        delete []buffer;
        buffer=NULL;
    }
	return -1;
}


int ZKProxy::set_znode_data(const std::string &path, const std::string &value, int32_t version)
{
	struct Stat stat;
	
	int rc = zoo_set2(m_zh, path.c_str(), value.c_str(), value.length(), version, &stat);
	LOG4CPLUS_DEBUG(logger, "set znode data path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");
	
	if(ZOK == rc)
	{
		LOG4CPLUS_DEBUG(logger, "set znode data path:" << path << " data:" << value << " version:" << version << " stat:" << dump_stat(&stat));
		return 0;
	}
	
	if(ZBADVERSION == rc)
	{
		LOG4CPLUS_DEBUG(logger, "set znode data path:" << path << " data:" << value << " bad version:" << version);
		return 1;
	}
	
	LOG4CPLUS_WARN(logger, "set znode data path:" << path << " data failed!, rc:" << rc);	
	return -1;
}


int ZKProxy::get_znode_children(const std::string &path, std::set<std::string> &children, bool watch)
{
	children.clear();
	
	struct Stat stat;
	struct String_vector strings;
		
	int rc = zoo_get_children2(m_zh, path.c_str(), watch == true ? 1 : 0, &strings, &stat);
	LOG4CPLUS_DEBUG(logger, "get znode children path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");

	if(ZOK == rc)
	{
		LOG4CPLUS_DEBUG(logger, "get " << strings.count << " children for znode:" << path << " stat:" << dump_stat(&stat));
		for(int idx = 0 ; idx < strings.count ; ++idx)
		{
			LOG4CPLUS_DEBUG(logger, "znode:" << path << "'s child[" << idx << "]:" << strings.data[idx]);
			children.insert(std::string(strings.data[idx]));
		}
		
		return 0;
	}

	LOG4CPLUS_WARN(logger, "get znode children path:" << path << " failed!, rc:" << rc);
	return -1;

}


void ZKProxy::watcher(zhandle_t *zzh, int type, int state, const char *path, void* context)
{
	LOG4CPLUS_DEBUG(logger, "context:" << context << " Watcher event type:" << type2string(type) << " session state :" << state2string(state));
	ZKProxy * proxy = (ZKProxy *)context;
	
	if(ZOO_CREATED_EVENT == type)
	{
		proxy->handle_create(zzh, type, state, path, context);
	}
	else if(ZOO_DELETED_EVENT == type)
	{
		proxy->handle_delete(zzh, type, state, path, context);
	}
	else if(ZOO_CHANGED_EVENT == type)
	{
		proxy->handle_change(zzh, type, state, path, context);
	}
	else if(ZOO_CHILD_EVENT == type)
	{
		proxy->handle_child(zzh, type, state, path, context);
	}
	else if(ZOO_SESSION_EVENT == type)
	{
		proxy->handle_session(zzh, type, state, path, context);
	}
	else if(ZOO_NOTWATCHING_EVENT == type)
	{
		proxy->handle_notwatching(zzh, type, state, path, context);
	}
	else
	{
		BOOST_ASSERT( false && "INVALID EVENT TYPE:");
	}
	
}


int ZKProxy::handle_create(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle create event. this:" << this << " state:" << state2string(state) << " path:" << path);
	
	return 0;
}


int ZKProxy::handle_delete(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle delete event. this:" << this << " state:" << state2string(state) << " path:" << path);
	
	return 0;
}


int ZKProxy::handle_change(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle change event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKProxy::handle_child(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle child event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKProxy::handle_session(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle session event. this:" << this << " state:" << state2string(state) << " path:" << path);
	
	// handshake ok session
 	if(ZOO_CONNECTED_STATE == state)
 	{
		const clientid_t *id = zoo_client_id(zzh);

		if (m_myid.client_id == 0 || m_myid.client_id != id->client_id)
		{
			m_myid = *id;
			LOG4CPLUS_DEBUG(logger, "Got a new session id:" << (long)(m_myid.client_id));
		}
		else
		{
			LOG4CPLUS_DEBUG(logger, "Use the previous session id:" << (long)(m_myid.client_id) << "/" << (long)(id->client_id));	
		}
 	}
	else if(ZOO_AUTH_FAILED_STATE == state)
	{
		LOG4CPLUS_WARN(logger, "Authentication failure. Shutting down...");
		zk_uninit();
	}
	else if(ZOO_EXPIRED_SESSION_STATE == state)
	{
		LOG4CPLUS_FATAL(logger, "Session expired. Shutting down...");
		zk_uninit();

		abort();		//
	}

	LOG4CPLUS_DEBUG(logger, "signal handle session event finish");
	m_sync.signal();
	
	return 0;
	
}


int ZKProxy::handle_notwatching(zhandle_t *zzh, int type, int state, const char *path,void* context)
{
	LOG4CPLUS_DEBUG(logger, "handle notwatching event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


// event types
const char* ZKProxy::type2string(int type)
{
	if (type == ZOO_CREATED_EVENT)
		return "ZOO_CREATED_EVENT";
	
  	if (type == ZOO_DELETED_EVENT)
		return "ZOO_DELETED_EVENT";
	
	if (type == ZOO_CHANGED_EVENT)
		return "ZOO_CHANGED_EVENT";

	if (type == ZOO_CHILD_EVENT)
		return "ZOO_CHILD_EVENT";

	if (type == ZOO_SESSION_EVENT)
		return "ZOO_SESSION_EVENT";

	if (type == ZOO_NOTWATCHING_EVENT)
		return "ZOO_NOTWATCHING_EVENT";

	return "INVALID EVENT";
	
}


// session state
const char* ZKProxy::state2string(int state)
{
	if (state == 0)
		return "CLOSED_STATE";

	if (state == ZOO_CONNECTING_STATE)
		return "CONNECTING_STATE";

	if (state == ZOO_ASSOCIATING_STATE)
		return "ASSOCIATING_STATE";

	if (state == ZOO_CONNECTED_STATE)
		return "CONNECTED_STATE";

	if (state == ZOO_EXPIRED_SESSION_STATE)
		return "EXPIRED_SESSION_STATE";

	if (state == ZOO_AUTH_FAILED_STATE)
		return "AUTH_FAILED_STATE";

	return "INVALID_STATE";
}	


std::string ZKProxy::dump_stat(const struct Stat *stat)
{
	std::ostringstream oss;
		
	oss << " ctime:"             	<< ZKUtility::format_time(stat->ctime/1000)
		<< " czxid:"             	<< (long long) stat->czxid
		<< " mtime:"             	<< ZKUtility::format_time(stat->mtime/1000)
		<< " mzxid:"             	<< (long long) stat->mzxid
		<< " version:"	         	<< (unsigned int)stat->version
		<< " aversion:"          	<< (unsigned int)stat->aversion
		<< " ephemeralOwner:" 	<< (long long) stat->ephemeralOwner;
			
		return oss.str();

}


