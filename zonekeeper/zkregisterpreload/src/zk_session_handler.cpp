#include "zk_session_handler.h"

	ZKSessionHandler::ZKSessionHandler(const std::string& host_list, int recv_timeout)
:m_zh(NULL),
	m_hosts_list(host_list),
	m_recv_timeout(recv_timeout),
	connected(false)
{
	zk_init();
}
ZKSessionHandler::~ZKSessionHandler()
{
	zk_uninit();
}
int ZKSessionHandler::zk_init()
{
#ifdef _DEBUG
	LOG_DEBUG("set zookeeper log level:" << "DEBUG");
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
#endif
	m_zh = zookeeper_init(m_hosts_list.c_str(), watcher, m_recv_timeout, 0, this, 0);
	// block to wait for finishing establishing zk session 
	if(!m_sync.wait(2*m_recv_timeout))		// timeout
	{
		LOG_ERROR("zookeeper handshake finish, TIMEOUT!");
		return -1;
	}
	else if (!m_zh){
		LOG_ERROR("zokeeper init failed "<< strerror(errno));
		return -1;
	}
	LOG_DEBUG("zokeeper init succ");
	return 0;

}
void ZKSessionHandler::zk_uninit()
{
	if (m_zh){
		LOG_DEBUG("zookeeper_close");
		zookeeper_close(m_zh);
		m_zh = NULL;
	}
}
void ZKSessionHandler::watcher(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	ZKSessionHandler* zk_session_handler = (ZKSessionHandler*)watcherCtx;
	if (state == ZOO_CONNECTED_STATE) {
		zk_session_handler->connected = true;
	} else {
		zk_session_handler->connected = false;
	}

	LOG_DEBUG("watcherCtx:" << watcherCtx << " Watcher event type:" << type2string(type) << " session state :" << state2string(state));
	if(ZOO_CREATED_EVENT == type)
	{
		zk_session_handler->handle_create(zh_t, type, state, path, watcherCtx);
	}
	else if(ZOO_DELETED_EVENT == type)
	{
		zk_session_handler->handle_delete(zh_t, type, state, path, watcherCtx);
	}
	else if(ZOO_CHANGED_EVENT == type)
	{
		zk_session_handler->handle_change(zh_t, type, state, path, watcherCtx);
	}
	else if(ZOO_CHILD_EVENT == type)
	{
		zk_session_handler->handle_child(zh_t, type, state, path, watcherCtx);
	}
	else if(ZOO_SESSION_EVENT == type)
	{
		zk_session_handler->handle_session(zh_t, type, state, path, watcherCtx);
	}
	else if(ZOO_NOTWATCHING_EVENT == type)
	{
		zk_session_handler->handle_notwatching(zh_t, type, state, path, watcherCtx);
	}
	else
	{
		//BOOST_ASSERT( false && "INVALID EVENT TYPE:");
	}
}
int ZKSessionHandler::create_znode(std::string &path, const std::string &value, int flag)
{
	char path_buff[MAX_PATH_LEN];
	int rc = zoo_create(m_zh, path.c_str(), value.c_str(), value.length(), &ZOO_OPEN_ACL_UNSAFE, flag, path_buff, MAX_PATH_LEN);
	LOG_DEBUG("create znode path:" << path << " rc:" << rc << "(" << zerror(rc) << ")");

	if(ZOK == rc || ZNODEEXISTS == rc)
	{
		if(ZOK == rc && flag&ZOO_SEQUENCE)
		{
			path = string(path_buff);
		}

		LOG_DEBUG("create znode path:" << path << " ok or it has exist, rc:" << rc);
		return 0;
	}

	LOG_WARN("create znode path:" << path << " failed!, rc:" << rc);      
	return -1;
}
int ZKSessionHandler::znode_exist(const std::string &path, bool &exist, bool watch)
{
	struct Stat stat;
	int rc = zoo_exists(m_zh, path.c_str(), watch == true ? 1 : 0, &stat);
	LOG_DEBUG("check exist znode path:" << path << " rc:" << rc << "(" << zerror(rc) << ")"); 

	if(ZOK == rc)
	{
		LOG_DEBUG("check exist znode path:" << path << " exist, node stat:" << dump_stat(stat)); 
		exist = true;
		return 0;   
	}
	else if(ZNONODE == rc)
	{
		LOG_DEBUG("check exist znode path:" << path << " not exist");
		exist = false;
		return 0;
	}

	LOG_WARN("check exist znode path:" << path << " failed!, rc:" << rc);
	return -1;
}
int ZKSessionHandler::handle_create(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle create event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKSessionHandler::handle_delete(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle delete event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKSessionHandler::handle_change(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle change event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKSessionHandler::handle_child(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle child event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}


int ZKSessionHandler::handle_session(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle session event. this:" << this << " state:" << state2string(state) << " path:" << path);

	// handshake ok session
	if(ZOO_CONNECTED_STATE == state)
	{
		const clientid_t *id = zoo_client_id(zh_t);

		if (m_session_id.client_id == 0 || m_session_id.client_id != id->client_id)
		{
			m_session_id = *id;
			LOG_DEBUG("Got a new session id:" << (long)(m_session_id.client_id));
		}
		else
		{
			LOG_DEBUG("Use the previous session id:" << (long)(m_session_id.client_id) << "/" << (long)(id->client_id));    
		}
	}
	else if(ZOO_AUTH_FAILED_STATE == state){
		LOG_WARN("Authentication failure. Shutting down...");
		zk_uninit();
	}
	else if(ZOO_EXPIRED_SESSION_STATE == state)
	{
		LOG_FATAL("Session expired. registing...");
		zk_uninit();
		reConnected();
		waitForConnected();
		LOG_DEBUG("wait for connected succ");
		process_seesion_expired_recovering();
	}

	LOG_DEBUG("signal handle session event finish");
	m_sync.signal();
	return 0;

}

int ZKSessionHandler::handle_notwatching(zhandle_t *zh_t, int type, int state, const char *path,void* watcherCtx)
{
	LOG_DEBUG("handle notwatching event. this:" << this << " state:" << state2string(state) << " path:" << path);

	return 0;
}
bool ZKSessionHandler::reConnected() {
	while(0 != zk_init() ) {
		sleep(1);
	}
	return true;
}
bool ZKSessionHandler::waitForConnected() {
	while(!connected) {
		LOG_DEBUG("wait for connected sleep 1");
		sleep(1);
	}
	return connected;
}
std::string ZKSessionHandler::dump_stat(const struct Stat &stat)
{
	std::ostringstream oss;

	oss //<< " ctime:"                << ZKUtility::format_time(stat.ctime/1000)
		<< " czxid:"                << (long long) stat.czxid
		//<< " mtime:"                << ZKUtility::format_time(stat.mtime/1000)
		<< " mzxid:"                << (long long) stat.mzxid
		<< " version:"              << (unsigned int)stat.version
		<< " aversion:"             << (unsigned int)stat.aversion
		<< " ephemeralOwner:"   << (long long) stat.ephemeralOwner;

	return oss.str();

}
const char* ZKSessionHandler::type2string(int type)
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
const char* ZKSessionHandler::state2string(int state)
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
