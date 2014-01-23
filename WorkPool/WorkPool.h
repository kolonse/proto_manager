#pragma once
#include <string>
#include <vector>
#include "WorkThread.h"
using namespace std ;
class CWorkPool
{
public:
	CWorkPool( );
	~CWorkPool(void);
	bool Init( int nCount = 4 ) ;
	void UnInit();
	void Add( do_work one ) ;
private:
	vector< CWorkThread* > m_vThreadPool ;
	int m_nCount ;
	int m_nIndex ;
	boost::mutex				m_cs;
	static const int MAX_POOL = 100 ;
};


