#ifndef _ZK_THREAD_SYNC_H_
#define _ZK_THREAD_SYNC_H_

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


// Synchronized
class Synchronized: boost::noncopyable 
{
public:
	Synchronized();
	
	~Synchronized();
	
	bool wait(uint32_t to_ms);
	
	void signal();
	
	void broadcast();
	
private:
	boost::mutex m_lock;
	boost::condition_variable_any m_ready;
	bool m_continue;
	
};

		
#endif // _ZK_THREAD_SYNC_H_

