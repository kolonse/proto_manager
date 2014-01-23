
#include "ProtoManager.h"
#include "../../../CommonLib/Utility/HttpFile.h"
boost::shared_ptr<CWorkPool> ProtoManager::g_pPool;
boost::shared_ptr<ProtoCache> ProtoManager::g_pProtoCache ; 

bool ProtoManager::Init( const string& sCachePath, boost::uint64_t u64MaxSpace, int nThreadCount)
{
	g_pProtoCache = boost::shared_ptr<ProtoCache>( ProtoCache::Create(sCachePath,u64MaxSpace));
	if( NULL == g_pProtoCache )
	{
		return false ;
	}

	g_pPool = boost::shared_ptr<CWorkPool>( new CWorkPool() ) ;
	if( NULL == g_pPool )
	{
		return false ;
	}

	if( false == g_pPool ->Init( nThreadCount ))
	{
		return false ;
	}

	return true ;
}

void ProtoManager::Read( const string& key, proto_callback cb )
{
	g_pPool ->Add( boost::bind( &ProtoManager::callback, key, cb ) );
}

bool ProtoManager::Read( const string& key, string& content, bool download )
{
	int nTimeOut = 15000 ;
	int nErrorCode  = 0 ;
	bool bFileExist = true ;
	string sContent = "" ;

	bool bRet = false ;
	do
	{
		if( true == g_pProtoCache ->Read( key, content ) )
		{
			bRet = true ;
			break ;
		}

		if( true == download )
		{
			if( false == CHttpFile::DownloadFile( key, nTimeOut, sContent, nErrorCode, bFileExist) )
			{
				break ;
			}
			g_pProtoCache ->Write( key, sContent ) ;
			bRet = true ;
		}

	}while( 0 );

	return bRet ;
}

void ProtoManager::Delete( const string& key )
{
	g_pProtoCache ->Delete( key ) ;
}

void ProtoManager::UnInit( )
{
	if( NULL != g_pPool )
	{
		g_pPool ->UnInit();
	}
}

void ProtoManager::callback( string url, proto_callback cb )
{
	int nTimeOut = 15000 ;
	int nErrorCode  = 0 ;
	bool bFileExist = true ;
	string sContent = "" ;
	bool bSuccess = true ;
	do
	{
		if( true == g_pProtoCache ->Read( url, sContent ) )
		{
			break ;
		}

		if( false == CHttpFile::DownloadFile( url, nTimeOut, sContent, nErrorCode, bFileExist) )
		{
			bSuccess = false ;
			break ;
		}

		g_pProtoCache ->Write( url, sContent ) ;
	}while(0);

	cb( sContent, bSuccess );
}
