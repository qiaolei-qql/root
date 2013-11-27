#include "DSPNodeConHash.h"
#include <boost/typeof/typeof.hpp>
#include <boost/assert.hpp>
#include <errno.h>


using namespace std;

IMPL_LOGGER(DSPNodeConHash, logger);

DSPNodeConHash::DSPNodeConHash(std::string ring_id)
:m_ring_id(ring_id), m_node_num(0), m_conhash(NULL)
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
	
	reset_status();	
}


DSPNodeConHash::~DSPNodeConHash()
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
	
	conhash_fini(m_conhash);
}


bool DSPNodeConHash::init()
{
	LOG4CPLUS_DEBUG(logger, m_ring_id << "DSPNodeConHash start init");
	
	// init conhash instance
    	m_conhash = conhash_init(NULL);
    	if(!m_conhash)
    	{
    		LOG4CPLUS_ERROR(logger, m_ring_id << "DSPNodeConHash init conhash failed!");	
    	}
	
	LOG4CPLUS_DEBUG(logger, m_ring_id << "DSPNodeConHash init succ!");
	return true;	
}


bool DSPNodeConHash::add_node(std::string id, uint32_t replicas)
{
	boost::unique_lock<boost::shared_mutex> ul(m_rw_lock);

	return add_node_in(id, replicas);
}


bool DSPNodeConHash::del_node(std::string id)
{
	boost::unique_lock<boost::shared_mutex> ul(m_rw_lock);

	return del_node_in(id);
}
	

// clear all node from hash ring
bool DSPNodeConHash::clear_nodes()
{
	boost::unique_lock<boost::shared_mutex> ul(m_rw_lock);

	return clear_nodes_in();
}


// batch insert some nodes
bool DSPNodeConHash::insert_nodes(std::vector<std::string> &ids, std::vector<uint32_t> &replicases)
{
	boost::unique_lock<boost::shared_mutex> ul(m_rw_lock);

	return insert_nodes_in(ids, replicases);
}


// repace all current nodes with ids
bool DSPNodeConHash::replace_nodes(std::vector<std::string> &ids, std::vector<uint32_t> &replicases)
{
	boost::unique_lock<boost::shared_mutex> ul(m_rw_lock);

	// clear all exist node at first
	clear_nodes_in();

	// insert new nodes
	return insert_nodes_in(ids, replicases);
}


//hash(key) --> node id
std::string DSPNodeConHash::hash(const std::string &key)
{
	boost::shared_lock<boost::shared_mutex> sl(m_rw_lock);

	const struct node_s *node = conhash_lookup(m_conhash, key.c_str(), key.length());

	std::string id;
	if(node != NULL)
	{
		id = std::string(node->iden);
	}

	//std::map<std::string,struct node_s *>::iterator it = m_nodes_map.find(id);
	BOOST_AUTO(it, m_nodes_map.find(id));
	if(it == m_nodes_map.end() || it->second != node)
	{
		LOG4CPLUS_WARN(logger, "node:" << node << " found in " << m_ring_id << " conhash ring, but is not the same map[" << id << "]");
		return "";
	}

	m_nodes_stat[id]++;
		
	return id;
}


uint32_t DSPNodeConHash::get_node_num()
{
	boost::shared_lock<boost::shared_mutex> sl(m_rw_lock);

	return m_node_num;
}


bool DSPNodeConHash::add_node_in(std::string id, uint32_t replicas)
{
	LOG4CPLUS_DEBUG(logger, " add node:" << id << " into " << m_ring_id << " conhash ring");

	if(m_nodes_map.find(id) != m_nodes_map.end())
	{
		LOG4CPLUS_DEBUG(logger, "node:" << id << " has already been in " << m_ring_id<< " conhash ring");
		return true;
	}

	struct node_s * node = new node_s;
	if(!node)
	{
		LOG4CPLUS_ERROR(logger, "can't create a new node struct for " << id << " in " << m_ring_id << " conhash ring");
		return false;
	}

	LOG4CPLUS_DEBUG(logger, "set new node(" << node << ")'s id:" << id << " and replicas:" << replicas << " in " << m_ring_id << " conhash ring");
    	conhash_set_node(node, id.c_str(), replicas);

	LOG4CPLUS_DEBUG(logger, "add node(" << node << ")-id:" << node->iden << " in " << m_ring_id << " conhash ring");
	conhash_add_node(m_conhash, node);

	m_nodes_map.insert(make_pair(id, node));
	m_node_num++;

	uint32_t vnodes_num = conhash_get_vnodes_num(m_conhash);
	LOG4CPLUS_DEBUG(logger,"after add there are " << vnodes_num << " vnode in " << m_ring_id << " conhash ring, nodes_map size:" << m_nodes_map.size());
	
	return true;

}


bool DSPNodeConHash::del_node_in(std::string id)
{
	LOG4CPLUS_DEBUG(logger, " del node:" << id << " from " << m_ring_id << " conhash ring");
	
	if(m_nodes_map.find(id) == m_nodes_map.end())
	{
		LOG4CPLUS_DEBUG(logger, "node:" << id << " has not been in " << m_ring_id << " conhash ring");
		return true;
	}

	struct node_s * node = m_nodes_map.find(id)->second;
	if(node != NULL)
	{
		LOG4CPLUS_DEBUG(logger, "del node(" << node << ")-id:" << node->iden << " from " << m_ring_id << " conhash ring");
		conhash_del_node(m_conhash, node);
	}
	
	m_node_num--;
	m_nodes_map.erase(id);

	uint32_t vnodes_num = conhash_get_vnodes_num(m_conhash);
	LOG4CPLUS_DEBUG(logger,"after del there are " << vnodes_num << " vnode left in " << m_ring_id << " conhash ring, nodes_map size:" << m_nodes_map.size());
	
	return true;

}


bool DSPNodeConHash::clear_nodes_in()
{
	while(m_nodes_map.size() > 0)
	{
		std::string id = m_nodes_map.begin()->first;
		bool succ = del_node_in(id);
		BOOST_ASSERT(succ && "delete from hash ring failed, when clear nodes");
	}

	return true;
}


bool DSPNodeConHash::insert_nodes_in(std::vector<std::string> &ids, std::vector<uint32_t> &replicases)
{
	BOOST_ASSERT(ids.size() == replicases.size() && "invalid ids or replicases size");

	std::size_t total = ids.size();
	for(std::size_t idx = 0; idx < total; ++idx)
	{
		bool succ = add_node_in(ids[idx], replicases[idx]);
		BOOST_ASSERT(succ && "add into hash ring failed, when insert nodes");
	}	

	return true;
}


void DSPNodeConHash::reset_status(void)
{
	for(BOOST_AUTO(it, m_nodes_stat.begin()); it != m_nodes_stat.end() ; ++it)
	{
		it->second = 0;
	}
}


std::string DSPNodeConHash::get_statistic(int reset)
{
	stringstream output;

	output << "[DSPNodeConHash:" << m_ring_id << ",node_num:" << m_node_num;

	output << ",dispatch:[";
	
	for(BOOST_AUTO(it, m_nodes_stat.begin()); it != m_nodes_stat.end() ; ++it)
	{
		if(it != m_nodes_stat.begin())
		{
			output << ",";
		}
		
		output << it->first << ":" << it->second;
	}
	
	output << "]]";
		
	if(reset)
	{
		reset_status();
	}
	
	return output.str();	
}


