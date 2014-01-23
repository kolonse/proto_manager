#include "WorkThread.h"
CWorkThread::CWorkThread()
{
	m_running = false ;
}


CWorkThread::~CWorkThread(void)
{
}

void CWorkThread::ThreadMain()
{
	while(m_running)
	{
		do_work one ;
		{
			boost::mutex::scoped_lock csLock(m_cs);
			if ( true == m_queWork.empty() )
			{
				m_threadCond.wait(csLock);
				continue;
			}
			one = m_queWork.front() ;
			m_queWork.pop() ;
		}
		one() ;
	}
}

void CWorkThread::InitInstance()
{
}

void CWorkThread::ExitInstance()
{
}

void CWorkThread::StopProcess()
{
    m_running = false;
	{
		boost::mutex::scoped_lock csLock(m_cs);
		m_threadCond.notify_one();
	}
	join();
	while( false == m_queWork.empty() )
	{
		m_queWork.pop() ;
	}
}

void CWorkThread::AddWork( do_work one )
{
	boost::mutex::scoped_lock csLock(m_cs);
	m_queWork.push( one ) ;
	m_threadCond.notify_one();
}

void CWorkThread::Start()
{
    if( false == m_running )
    {
        m_running = true;
        start();
    }
}