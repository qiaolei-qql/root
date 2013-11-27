#include "SDRedisServer.h"
#include <boost/assert.hpp>

IMPL_LOGGER(SDRedisServer, logger);

SDRedisServer::SDRedisServer(std::string id,
						std::string host,
						uint16_t port,
						uint16_t db_index,
						redisContext *redis_context)
:m_id(id),m_host(host),m_port(port),m_db_index(db_index),m_redis_context(redis_context),m_in_use(false)
{
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
}

SDRedisServer::~SDRedisServer()
{
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
	if(is_connected() && m_in_use != true)
	{
		close();
	}
}

bool SDRedisServer::operator<(const SDRedisServer& other) const
{
	return m_id < other.m_id;
}

bool SDRedisServer::connect(uint32_t toms)
{
	struct timeval timeout = {toms, 0};

	if(m_redis_context != NULL)
	{
		LOG4CPLUS_DEBUG(logger,"redis server:" << dump() << " has been connected, close it at first and reconnect");
		close();
	}
	
	m_redis_context = redisConnectWithTimeout((char*)m_host.c_str(), m_port, timeout);
	if(m_redis_context->err)
	{
		LOG4CPLUS_ERROR(logger,"connect to " << dump() << " failed.");
		close();
		return false;
	}

	LOG4CPLUS_DEBUG(logger,"connect to " << dump() << " ok.");

	return select_db(m_db_index);	// switch db at once
}


// ping redis server
bool SDRedisServer::ping()
{
	if(!is_connected())
	{
		LOG4CPLUS_WARN(logger,"ping " << dump() << " but is not connected!");
		return false;
	}
		
	// ping Redis server
	bool succ = false;
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "PING");
	if(NULL == reply)
	{
		LOG4CPLUS_WARN(logger,"ping " << dump() << " reply null, close connection");
		close();
		return false;
	}

	LOG4CPLUS_DEBUG(logger,"ping " << dump() << " reply:" << std::string(reply->str));
	if(reply->type == REDIS_REPLY_STATUS  && std::string(reply->str) == "PONG")
	{
		succ = true;
	}

	freeReplyObject(reply);
		
	return succ;
}
	

bool SDRedisServer::select_db(uint16_t db_index)
{
	if(!is_connected())
	{
		LOG4CPLUS_WARN(logger,"select " << dump() << " but is not connected!");
		return false;
	}
		
	// SELECT db_index
	bool succ = true;
	stringstream command;
	command << "SELECT " << db_index;
	
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "SELECT %d", db_index);
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	
	if(reply->type != REDIS_REPLY_STATUS || std::string(reply->str,reply->len) != "OK")
	{
		succ = false;		
	}

	freeReplyObject(reply);
	return succ;

}


// close connection
bool SDRedisServer::close()
{
	if(m_redis_context)
	{
		redisFree(m_redis_context);
	}

	m_redis_context = NULL;
		
	return true;
}

// hash get and if exist then update key timeout,
int SDRedisServer::hget2(const std::string &key, std::vector<Value> &values, uint32_t duration_time = -1,bool is_set_expire = false)
{
	// 最好用pipeline方式实现 
	int ret = hget(key, values);
    
	if(1 == ret&&is_set_expire)
	{
		expire(key, duration_time);
	}

	return ret;
}

//hgetall key
int SDRedisServer::hgetall(const std::string &key,std::vector<Value>&values,uint32_t duration_time,bool is_set_expire)
{
    // 最好用pipeline方式实现 
    int ret = hgetall(key, values);
    
    if(1 == ret&&is_set_expire)
    {
        expire(key, duration_time);
    }

    return ret;
}


//  hash set and then set key timeout，
int SDRedisServer::hset2(const std::string &key, std::vector<Value> &values, uint32_t duration_time = -1)
{
	// 最好用 用pipeline 方式实现
	int ret = hset(key, values);
    
	if(1 == ret)
	{
		expire(key, duration_time);
	}

	return ret;	
}

// HDEL key field [field ...]
int SDRedisServer::hrem(const std::string &key, std::vector<Value> &values)
{
	uint32_t size = values.size();
	const char * argv[size + 2];
	size_t argvlen[size + 2];
	
	// command
	std::string command = "HDEL";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();

	// fields
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		argv[2 + idx] = values[idx].m_field.data();
		argvlen[2 + idx] = values[idx].m_field.length();
	}
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	/*
	Integer reply: the number of members that were removed from the set, not including non existing members.
	*/
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "rem key:" << key << " 's filed in hash return " << reply->integer << "/" << size);
	}
	
   	freeReplyObject(reply);

	return 1;

}


// set add some members into the set for a key
int SDRedisServer::sadd2(const std::string &key, std::vector<std::string> &members, uint32_t duration_time)
{
	int ret = sadd(key, members);
	if(1 == ret)
	{
		expire(key, duration_time);
	}

	return ret;

}


// get all members in the set for a key
int SDRedisServer::sget2(const std::string &key, std::vector<std::string> &members, uint32_t duration_time)
{
	int ret = sget(key, members);
	if(1 == ret)
	{
		expire(key, duration_time);
	}

	return ret;

}

	

// HMGET key field [field ...]
int SDRedisServer::hget(const std::string &key, std::vector<Value> &values)
{
	// 准备参数
	uint32_t size = values.size();
	const char * argv[size + 2];
	size_t argvlen[size + 2];
	
	// command
	std::string command = "HMGET";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();

	// fields
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		argv[2 + idx] = values[idx].m_field.data();
		argvlen[2 + idx] = values[idx].m_field.length();
	}
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	// HMGET应该返回一个ARRAY,且filed 数目是否和请求中的一致
	if(reply->type != REDIS_REPLY_ARRAY || reply->elements != size)
	{
		LOG4CPLUS_ERROR(logger,"HMGET key:" << key << " reply type:" << reply->type << "fileds:" << size << "/" << reply->elements);
		freeReplyObject(reply);
		return 0;
	}

	// 当redis数据为空 时也会返回一个array，只有当所有的filed都是空时我们认为没有命中
	uint has_data = 0;
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		redisReply *value_replay = reply->element[idx];
		// 类型判断	
		values[idx].m_value = std::string(value_replay->str,value_replay->len);
		if(!values[idx].m_value.empty())
		{
			++has_data;
		}
		
		//freeReplyObject(value_replay);	// double free make program core ,only free the whole reply structure
	}

	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ ,reply fileds size:" << reply->elements << ", has_date:" << has_data);
	freeReplyObject(reply);
	return has_data > 0 ? 1 : 0;
	
}

/*
 *  hgetall key 
 *  return all filed in hash table
 *
*/
int SDRedisServer::hgetall(const std::string &key, std::vector<Value> &values)
{
	// 准备参数
	uint32_t size = 0;
	const char * argv[size + 2];
	size_t argvlen[size + 2];
	
	// command
	std::string command = "HGETALL";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	// HGETALL应该返回一个ARRAY
	if(reply->type != REDIS_REPLY_ARRAY)
	{
		LOG4CPLUS_ERROR(logger,"HMGET key:" << key << " reply type:" << reply->type << "fileds:" << size << "/" << reply->elements);
		freeReplyObject(reply);
		return 0;
	}

	// 当redis数据为空 时也会返回一个array，只有当所有的filed都是空时我们认为没有命中
	uint32_t has_data = 0,idx=0;
    uint32_t num = reply->elements;

    std::string filed,value;
    
    for(idx=0;idx<num;idx+=2)
	{
		redisReply *value_replay_filed = reply->element[idx],*value_replay_value= reply->element[idx+1];
        
		// 类型判断	
		filed = std::string(value_replay_filed->str,value_replay_filed->len);
		value = std::string(value_replay_value->str,value_replay_value->len);

        Value tmp(filed,value,0);

        values.push_back(tmp);
        
		++has_data;
		
		//freeReplyObject(value_replay);	// double free make program core ,only free the whole reply structure
	}

	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ ,reply fileds size:" << reply->elements << ", has_date:" << has_data);
	freeReplyObject(reply);
	return has_data > 0 ? 1 : 0;
	
}


int SDRedisServer::hset(const std::string &key, std::vector<Value> &values)
{
	// 准备参数
	uint32_t size = values.size();
	const char * argv[size*2 + 2];
	size_t argvlen[size*2 + 2];
	
	// command
	std::string command = "HMSET";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();

	// fields & values
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		argv[2 + 2*idx] = values[idx].m_field.data();
		argvlen[2 + 2*idx] = values[idx].m_field.length();

		argv[2 + 2*idx + 1] = values[idx].m_value.data();
		argvlen[2 + 2*idx + 1] = values[idx].m_value.length();
	}
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size*2 + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	// HMGET应该返回一个ARRAY,且filed 数目是否和请求中的一致
	if(reply->type != REDIS_REPLY_STATUS)
	{
		LOG4CPLUS_ERROR(logger,"HMSET key:" << key << " reply type:" << reply->type);
		freeReplyObject(reply);
		return 0;
	}

	// 返回状态不是OK
	if(std::string(reply->str,reply->len) != "OK")
	{
		freeReplyObject(reply);
		return 0;
	}
	
	freeReplyObject(reply);
	return 1;

}

// string get
int SDRedisServer::strget2(const std::string &key, std::string &value, uint32_t duration_time)
{
	int ret = strget(key, value);
	if(1 == ret && duration_time>0)
	{
		expire(key, duration_time);
	}

	return ret;	
}



// string set
int SDRedisServer::strset2(const std::string &key, const std::string &value, uint32_t duration_time)
{
	stringstream command;
	command << "SETEX " << key;
		
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "SETEX %b %d %b", (char *)key.data(), key.length(),duration_time, value.data(), value.length());
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	BOOST_ASSERT(reply->type == REDIS_REPLY_STATUS);

	int ret = 0;
	if(reply->type == REDIS_REPLY_STATUS && std::string(reply->str,reply->len) == "OK")
	{
		ret = 1;
	}

	LOG4CPLUS_DEBUG(logger,"SETEX key:" << key << " reply type:" << reply->type << " value len:" << value.length());
	freeReplyObject(reply);
	return ret;
	
}


	
int SDRedisServer::exist(const std::string &key, bool &exist)
{
	stringstream command;
	command << "EXISTS " << key;
		
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "EXISTS %b", (char *)key.data(), key.length());
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	exist = false;
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		if(reply->integer == 1)
		{
			LOG4CPLUS_DEBUG(logger, "key:" << key << " exist in cache");
			exist = true;
		}
		else
		{
			exist = false;
			LOG4CPLUS_WARN(logger, "key:" << key << " not exist in cache");
		}
	}
	
    freeReplyObject(reply);
	return 1;

}

 
int SDRedisServer::remove(const std::string &key)
{
	stringstream command;
	command << "DEL " << key;
	
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "DEL %b", (char *)key.data(), key.length());
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	/*
	Integer reply: The number of keys that were removed.
	*/
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "remove key:" << key << " in cache return " << reply->integer);
	}
	
    freeReplyObject(reply);

	return 1;

}


int SDRedisServer::expire(const std::string &key, uint32_t duration_time)
{
	stringstream command;
	command << "EXPIRE " << key << " " << (int)duration_time;
	
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "EXPIRE %b %d", (char *)key.data(), key.length(), duration_time);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	/*
	Integer reply, specifically:
		1 if the timeout was set.
		0 if key does not exist or the timeout could not be set.
	*/
	int ret = 0;
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "expire key:" << key << " in cache return " << reply->integer);
		if(reply->integer == 1)
		{
			ret = 1;
		}
	}
	
    freeReplyObject(reply);

	return ret;
}

int SDRedisServer::hincrby(const std::string &key,const std::string &filed,int32_t &increment)
{
    stringstream command;
    
	command << "HINCRBY " << key << " "<<filed<<" "<<increment;
	
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "HINCRBY %b %b %d", (char *)key.data(),key.length(),(char *)filed.data(),filed.length(),increment);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	/*
	Integer reply, specifically:
		1 if the timeout was set.
		0 if key does not exist or the timeout could not be set.
	*/
	int ret = 0;
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "HINCRBY key:" << key << " in cache return " << reply->integer);
        ret =1;
	}
    else
    {
        LOG4CPLUS_ERROR(logger, "HINCRBY key:" << key << " in cache return " << reply->str);
        ret = 0;
    }
	
    freeReplyObject(reply);

	return ret;
}

int SDRedisServer::migrate(std::string &dst_redis_ip, uint16_t dst_redis_port, const std::string &key, uint32_t db, uint32_t to_ms)
{
	stringstream command;
	command << "MIGRATE " << key << " to " << dst_redis_ip << ":" << dst_redis_port;
	
      BOOST_ASSERT(db == m_db_index && "dst db should be the same to src db!");
	  
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "MIGRATE %s %d %b %d %d", (char *)dst_redis_ip.c_str(), dst_redis_port, (char *)key.data(), key.length(), db, to_ms);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	int ret = 0;
	if(reply->type == REDIS_REPLY_STATUS)
	{
		if(std::string(reply->str,reply->len) == "OK")
		{
			LOG4CPLUS_DEBUG(logger, "MIGRATE key:" << key << " return " << std::string(reply->str,reply->len));
			ret = 1;
		}
		else
		{
			LOG4CPLUS_WARN(logger, "MIGRATE key:" << key << " return " << std::string(reply->str,reply->len));
			ret = 0;
		}
	}
	
    freeReplyObject(reply);

	return ret;

}



// set add some members into the set for a key
/*
Add the specified members to the set stored at key. 
Specified members that are already a member of this set are ignored. If key does not exist, a new set is created before adding the specified members.
An error is returned when the value stored at key is not a set
*/
int SDRedisServer::sadd(const std::string &key, std::vector<std::string> &members)
{
	// SADD key member [member ...]
	uint32_t size = members.size();
	const char * argv[size + 2];
	size_t argvlen[size + 2];
	
	// command
	std::string command = "SADD";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();

	// members
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		argv[2 + idx] = members[idx].data();
		argvlen[2 + idx] = members[idx].length();
	}
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	/*
	Integer reply: the number of elements that were added to the set, not including all the elements already present into the set.
	*/
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "add key:" << key << " 's member in set return " << reply->integer << "/" << size);
	}
	
   	freeReplyObject(reply);

	return 1;

}

// get all members in the set for a key
int SDRedisServer::sget(const std::string &key, std::vector<std::string> &members)
{
	// SMEMBERS key
	/*
	Returns all the members of the set value stored at key.
	This has the same effect as running SINTER with one argument key.
	*/
	stringstream command;
	command << "SMEMBERS " << key;
		
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "SMEMBERS %b", (char *)key.data(), key.length());
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	
	if(reply->type != REDIS_REPLY_ARRAY)
	{	
		LOG4CPLUS_ERROR(logger,"SMEMBERS key:" << key << " reply invalid type:" << reply->type);
		freeReplyObject(reply);
		return 0;
	}

	for(uint32_t idx = 0 ; idx < reply->elements ; ++idx)
	{
		redisReply *value_replay = reply->element[idx];
		members.push_back(std::string(value_replay->str,value_replay->len));
		
		//freeReplyObject(value_replay);	// double free make program core ,only free the whole reply structure
	}

	LOG4CPLUS_DEBUG(logger,"SMEMBERS key:" << key << " reply type:" << reply->type << " member size:" << reply->elements);
	freeReplyObject(reply);
	return 1;

}

// remove set members
int SDRedisServer::srem(const std::string &key, std::vector<std::string> &members)
{
	// SREM key member [member ...]
	uint32_t size = members.size();
	const char * argv[size + 2];
	size_t argvlen[size + 2];
	
	// command
	std::string command = "SREM";
	argv[0] = command.c_str();
	argvlen[0] = command.length();

	// key
	argv[1] = key.data();
	argvlen[1] = key.length();

	// members
	for(uint32_t idx = 0 ; idx < size ; ++idx)
	{
		argv[2 + idx] = members[idx].data();
		argvlen[2 + idx] = members[idx].length();
	}
	
	redisReply *reply = (redisReply *)redisCommandArgv(m_redis_context, size + 2, argv, argvlen);
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command << " succ, reply type:" << reply->type);

	/*
	Integer reply: the number of members that were removed from the set, not including non existing members.
	*/
	if(reply->type == REDIS_REPLY_INTEGER)
	{
		LOG4CPLUS_DEBUG(logger, "rem key:" << key << " 's member in set return " << reply->integer << "/" << size);
	}
	
   	freeReplyObject(reply);

	return 1;

}

/*
 * return value:
 * -1 conn error
 *  0 succ and key not exit
 *  1 succ key exist
 */
int SDRedisServer::strget(const std::string &key, std::string &value)
{
	stringstream command;
	command << "GET " << key;
		
	redisReply *reply = (redisReply *)redisCommand(m_redis_context, "GET %b", (char *)key.data(), key.length());
	
	if(!reply)
	{
		//Once an error is returned the context cannot be reused and you should set up a new connection
		LOG4CPLUS_ERROR(logger, "redisCommand(" << m_redis_context << ","  << command.str() << ") failed! err:" << m_redis_context->err << "(" << string(m_redis_context->errstr) << ")");
		close();
		return -1;
	}
	LOG4CPLUS_DEBUG(logger, "redisCommand " << command.str() << " succ, reply type:" << reply->type);

	BOOST_ASSERT(reply->type == REDIS_REPLY_STRING || reply->type == REDIS_REPLY_NIL);

	int ret = 1;
	if(reply->type == REDIS_REPLY_STRING)
	{
		value = std::string(reply->str, reply->len);
	}
	if(reply->type == REDIS_REPLY_NIL)
    {
        ret = 0;
    }

	LOG4CPLUS_DEBUG(logger,"GET key:" << key << " reply type:" << reply->type << "value:" << value << " value len:" << value.length());
	freeReplyObject(reply);
	return ret;
	
}

