#include "ProtoCache.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std ;
ProtoCache::~ProtoCache()
{
    //dtor
}

ProtoCache* ProtoCache::Create( const string& sCachePath, boost::uint64_t u64MaxSpace )
{
    ProtoCache* pCache = NULL;
	try
	{
		pCache = new ProtoCache( sCachePath,  u64MaxSpace ) ;
	}
	catch( std::exception& ec )
	{
		cout << ec.what() << endl ;
		delete pCache ;
		pCache = NULL ;
	}

    return pCache ;
}

ProtoCache::ProtoCache( const string& sCachePath, boost::uint64_t u64MaxSpace )
{
    m_sCachePath = sCachePath ;
    m_u64MaxSpace = u64MaxSpace ;
    m_u64CurUse = 0 ;
    m_uIndex = 0 ;
    ClearCache() ;
}

void ProtoCache::ClearCache()
{
	boost::filesystem::path path( m_sCachePath ) ;
	boost::system::error_code ec ;
	if( true == boost::filesystem::is_directory( path, ec ) )
	{
		boost::filesystem::remove_all( path ) ;
	}
	else
	{
		if( true == boost::filesystem::is_regular_file( path, ec ))
		{
			stringstream ss ;
			ss << "非合法缓存路径,errorcode:" << ec.message() ;
			throw std::exception( ss.str().c_str() );
		}
	}
	/**
	*		@2013.11.15		by jszhou2
	*		目录已经不存在 需要创建目录
	*/
	if( false == boost::filesystem::create_directories( path, ec ) )
	{
		stringstream ss ;
		ss << "创建缓存目录失败,errorcode:" << ec.message() ;
		throw std::exception( ss.str().c_str() );
	}
	
	char c = m_sCachePath[ m_sCachePath.size() - 1 ];
	if( c != '\\' && c != '/' )
	{
		m_sCachePath += '/' ;
	}
}

bool ProtoCache::Write( const string& sKey, const string& sContent ) 
{
	FILEINFO info;
	bool bRet = true ;

	do
	{
		if( ResidueSpace() < sContent.size() )
		{
			//bRet = false ;
			//break ;
			if( false == ReleaseSpace( sContent.size() ) )
			{
				bRet = false ;
				break ;
			}
		}

		bRet = WriteToFile( sContent, info ) ;
		if( false == bRet )
		{
			break ;
		}
		Insert( sKey, info ) ;
	}while( 0 );

	return bRet ;
}

bool ProtoCache::ReleaseSpace( unsigned int needsize )
{
	if( m_u64MaxSpace < needsize )
	{
		return false ;
	}

	vector<FILEINFO> infos ;
	{
		boost::mutex::scoped_lock lock( m_mux_map );
		map<KEY,PATH>::iterator it = m_key_path.begin();
		for( ; it != m_key_path.end(); ++ it )
		{
			infos.push_back( it ->second );
		}
	}

	std::sort( infos.begin(), infos.end() );

	int i = 0 ;
	int size = infos.size() ;
	boost::uint64_t tmp = needsize ;
	while( ResidueSpace() < tmp && i < size )
	{
		Delete( infos[i ++ ].key );
	}

	if( ResidueSpace() < tmp )
	{
		return false ;
	}
	return true ;
}

bool ProtoCache::Read( const string& sKey, string& sContent ) 
{
	sContent.clear() ;
	bool bRet = true ;

	do
	{
		string path ;
		bRet = Find( sKey, path ) ;
		if( false == bRet )
		{
			break ;
		}
		bRet = ReadFromFile( path, sContent ) ;
	}while( 0 );

	return bRet ;
}

void ProtoCache::Delete( const string& sKey )
{
	string path ;
	boost::mutex::scoped_lock lock( m_mux_map );
	map<KEY,PATH>::iterator it = m_key_path.find( sKey ) ;
	if( m_key_path.end() == it )
	{
		return ;
	}

	UsableAdd( it ->second.size ) ;
	path = it ->second.path ;
	m_key_path.erase( it ) ;
	boost::system::error_code ec ;
	boost::filesystem::remove( path, ec );
}

bool ProtoCache::Find( const string& sKey, string& sResult ) 
{
	boost::mutex::scoped_lock lock( m_mux_map );
	map<KEY,PATH>::iterator it = m_key_path.find( sKey ) ;
	if( m_key_path.end() == it )
	{
		return false ;
	}

	sResult = it ->second.path ;
	return true ;
}

void ProtoCache::Insert( const string& sKey, FILEINFO& info ) 
{
	info.key = sKey ;
	info.createtime = time( NULL ) ;
	boost::mutex::scoped_lock lock( m_mux_map );
	map<KEY,PATH>::iterator it = m_key_path.find( sKey ) ;
	if( m_key_path.end() == it )
	{
		m_key_path[sKey] = info ;
		return ;
	}

	boost::system::error_code ec ;
	boost::filesystem::remove( it ->second.path, ec );
	it ->second = info ;
}

bool ProtoCache::WriteToFile( const string& sContent, FILEINFO& info )
{
	string filename = m_sCachePath + boost::lexical_cast<string,unsigned int>( takeOneIndex() ) ;
	bool bRet = false ;
	do
	{
		FILE* fp = fopen( filename.c_str(), "wb" );
		if( NULL == fp )
		{
			break ;
		}
		if( 1 == fwrite(sContent.c_str(),sContent.size(), 1, fp ) )
		{	
			bRet = true ;
		}
		fclose( fp ) ;

		if( false == bRet )
		{
			boost::system::error_code ec ;
			boost::filesystem::remove( filename, ec );
			break;
		}

		UsableReduce( sContent.size() );
		info.path = filename;
		info.size = sContent.size() ;
	}while( 0 ) ;

	return bRet ;
}

bool ProtoCache::ReadFromFile( const string& sPath, string& sContent )
{
	bool bRet = false ;
	do
	{
		FILE* fp = fopen( sPath.c_str(), "rb" );
		if( NULL == fp )
		{
			break ;
		}

		fseek( fp, 0, SEEK_END );
		int filesize = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		char* buff = new char[ filesize ] ;
		if( NULL == buff )
		{
			fclose( fp ) ;
			break ;
		}
		if( 1 == fread( buff, filesize, 1, fp ) )
		{
			bRet = true ;
			sContent.append( buff, filesize ) ;
			delete[] buff ;
		}
		fclose( fp ) ;
	}while(0);

	return bRet ;
}
