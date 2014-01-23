#ifndef PROTOCACHE_H
#define PROTOCACHE_H
#include <string>
#include <map>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
using namespace std ;
//#define MAX_SPACE_USE 10 * 1024 * 1024 * 1024 // 10G

static const boost::uint64_t MAX_SPACE_USE = 10737418240 ;

class ProtoCache
{
public:
    static ProtoCache* Create( const string& sCachePath, boost::uint64_t u64MaxSpace = MAX_SPACE_USE );

	bool Write( const string& sKey, const string& sContent ) ;
	bool Read( const string& sKey, string& sContent ) ;
	void Delete( const string& sKey );
    virtual ~ProtoCache();
protected:
private:
	class FILEINFO;
    ProtoCache( const string& sCachePath, boost::uint64_t u64MaxSpace = MAX_SPACE_USE );
    void ClearCache() ;
	bool Find( const string& sKey, string& sResult ) ;
	void Insert( const string& sKey, FILEINFO& info ) ;
	bool WriteToFile( const string& sContent, FILEINFO& info );
	bool ReadFromFile( const string& sPath, string& sContent );
	bool ReleaseSpace( unsigned int needsize ) ;
	inline unsigned int takeOneIndex()
	{
		boost::mutex::scoped_lock lock( m_mux_index ) ;
		return m_uIndex ++ ;
	}

	inline void UsableReduce( const boost::uint64_t& value )
	{
		boost::mutex::scoped_lock lock( m_mux_use ) ;
		m_u64CurUse += value ;
	}

	inline void UsableAdd( const boost::uint64_t& value )
	{
		boost::mutex::scoped_lock lock( m_mux_use ) ;
		m_u64CurUse = m_u64CurUse >= value ? m_u64CurUse - value:0 ;
	}

	inline boost::uint64_t ResidueSpace()
	{
		boost::mutex::scoped_lock lock( m_mux_use ) ;
		return m_u64MaxSpace > m_u64CurUse?m_u64MaxSpace - m_u64CurUse : 0 ;
	}
private:
	class FILEINFO
	{
	public:
		FILEINFO()
		{
			key = "" ;
			path = "" ;
			size = 0 ;
			createtime = 0 ;
		}
		bool operator < ( FILEINFO& info )
		{
			if( createtime < info.createtime )
			{
				return true ;
			}
			return false ;
		}
		string key ;
		string path ;
		unsigned int size ;
		boost::uint64_t createtime ;
	};

    boost::uint64_t m_u64MaxSpace  ;
	boost::mutex m_mux_use ;
    boost::uint64_t m_u64CurUse ;
    string m_sCachePath ;
    typedef string KEY ;
    typedef FILEINFO PATH ;

	boost::mutex m_mux_map ;
    map<KEY,PATH> m_key_path ;

	boost::mutex m_mux_index ;
    unsigned int m_uIndex ;
};

#endif // PROTOCACHE_H
