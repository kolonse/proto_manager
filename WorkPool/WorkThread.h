#pragma once
#include "../../CommonLib/Thread/BaseThread.h"
#include <boost/function.hpp>
#include <queue>
using std::queue;

typedef boost::function<void (void)> do_work ;
class CWorkThread :
	public CBaseThread
{
public:
	CWorkThread( );
	~CWorkThread(void);
    virtual void ThreadMain();
    virtual void InitInstance();
    virtual void ExitInstance();
	void StopProcess();
	void AddWork( do_work one ) ;
	void Start();
private:
	queue<do_work>				m_queWork ;
	bool						m_running ;
	boost::mutex				m_cs;
	boost::condition_variable	m_threadCond;
};

