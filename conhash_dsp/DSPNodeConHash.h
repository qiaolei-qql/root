#ifndef __DSP_NODE_CONHASH_H__
#define __DSP_NODE_CONHASH_H__

#include "libconhash/conhash.h"
#include <common/DSPLogger.h>
#include <common/DSPUtility.h>
#include <boost/thread.hpp>
#include <string>
#include <set>

// con hash ring wrapper
class DSPNodeConHash
{
public:
    	DSPNodeConHash(std::string ring_id);
	
    	~DSPNodeConHash();

    	bool init();

	// add a node info hash ring
	bool add_node(std::string id, uint32_t replicas);

	// del a node from hash ring
	bool del_node(std::string id);

	// clear all node from hash ring
	bool clear_nodes();

	// batch insert some nodes
	bool insert_nodes(std::vector<std::string> &ids, std::vector<uint32_t> &replicases);
		
	// repace all current nodes with ids
	bool replace_nodes(std::vector<std::string> &ids, std::vector<uint32_t> &replicases);

		
	// hash key --> node id
	std::string hash(const std::string &key);

	// return all nodes
	bool nodes(std::set<std::string> &ids);
	
	// get current nodesnum in hash ring
	uint32_t get_node_num();

	// each node hit times
	std::string get_statistic(int reset);
	

private:
	bool add_node_in(std::string id, uint32_t replicas);

	bool del_node_in(std::string id);

	bool clear_nodes_in();

	bool insert_nodes_in(std::vector<std::string> &ids, std::vector<uint32_t> &replicases);
	
	void reset_status(void);
		
private:
	std::string m_ring_id;			// id for this hash ring( infomation about lts type, function...)	
	uint32_t m_node_num;		// total different real node num (not vitual nodes)
	
	std::map<std::string, struct node_s *> m_nodes_map;		// id --> node
	
	struct conhash_s *m_conhash;
	boost::shared_mutex m_rw_lock;

	// some statistic
	std::map<std::string, uint32_t> m_nodes_stat;
	
	DECL_LOGGER(logger);

};

typedef boost::shared_ptr<DSPNodeConHash> DSPNodeConHash_ptr;


#endif	// __DSP_NODE_CONHASH_H__


