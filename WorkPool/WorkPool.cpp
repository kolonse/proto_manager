#include "WorkPool.h"


CWorkPool::CWorkPool()
{
}


CWorkPool::~CWorkPool(void)
{
}

bool CWorkPool::Init( int nCount )
{
	if( nCount > MAX_POOL )
	{
		nCount = MAX_POOL ;
	}

	m_nCount = nCount ;
	m_nIndex = 0 ;
	m_vThreadPool.clear() ;
	for( int i = 0 ; i < nCount; ++ i )
	{
		CWorkThread* pThread = new CWorkThread() ;
		pThread ->Start() ;
		m_vThreadPool.push_back( pThread ) ;
	}
	return true ;
}

void CWorkPool::UnInit()
{
	m_nCount = 0 ;
	vector< CWorkThread* >::iterator it = m_vThreadPool.begin() ;
	while( it != m_vThreadPool.end() )
	{
		(*it) ->StopProcess() ;
		delete *it ;
		++ it ;
	}
	m_vThreadPool.clear() ;
}

void CWorkPool::Add( do_work one )
{
	int nIndex = 0 ;
	{
		boost::mutex::scoped_lock csLock(m_cs);
		nIndex = m_nIndex ++ % m_nCount ;
	}
	m_vThreadPool[ nIndex ] ->AddWork( one ) ;
}

