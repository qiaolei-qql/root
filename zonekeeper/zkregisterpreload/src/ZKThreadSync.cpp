#include "ZKThreadSync.h"
#include <boost/assert.hpp>

using namespace boost;

Synchronized::Synchronized()
:m_continue(false)
{

}

	
Synchronized::~Synchronized()
{

}

	
bool Synchronized::wait(uint32_t to_ms)
{
	bool ret = false;
	boost::mutex::scoped_lock guard(m_lock);
		
	while(!m_continue)
	{
		//ret = m_ready.timed_wait(m_lock, boost::posix_time::milliseconds(to_ms));
		ret = m_ready.timed_wait(m_lock,  boost::get_system_time() + boost::posix_time::milliseconds(to_ms));
	}
		
	return ret;
}


void Synchronized::signal()
{
	boost::mutex::scoped_lock guard(m_lock);
	m_continue = true;
		
	m_ready.notify_one();
}

	
void Synchronized::broadcast()
{
	boost::mutex::scoped_lock guard(m_lock);
	m_continue = true;
		
	m_ready.notify_all();
}

