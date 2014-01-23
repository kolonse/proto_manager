#ifndef PROTO_MANAGER_INCLUDE
#define PROTO_MANAGER_INCLUDE

#include "ProtoCache.h"
#include "../WorkPool/WorkPool.h"

typedef boost::function<void (const string&,bool)> proto_callback ;
class ProtoManager
{
public:
	/**
	*		#2013.11.18		by jszhou2
	*		func:图片管理初始化接口
	*		@param:
	*		sCachePath:要缓存的路径
	*		u64MaxSpace:图片占用的最大空间,超过最大空间的图片将被丢弃
	*		nThreadCount:下载线程数目
	*/
	static bool Init( const string& sCachePath, boost::uint64_t u64MaxSpace = MAX_SPACE_USE, int nThreadCount = 4 );
	/**
	*		#2013.11.18		by jszhou2
	*		func:异步读取文件接口,如果文件不存在,文件将被下载,并缓存
	*		@param:
	*		key:要下载的文件关键字,这里表示url
	*		cb： 异步回调接口
	*/
	static void Read( const string& key, proto_callback cb );
	/**
	*		#2013.11.18		by jszhou2
	*		func:同步读取文件接口,如果文件不存在,文件是否下载有参数download决定,若下载会被缓存
	*		@param:
	*		key:要下载的文件关键字,这里表示url
	*		content:被读取文件的内存保存空间
	*		download:若文件不存在决定是否被下载
	*		@return:读取成功返回ture,否则返回false
	*/
	static bool Read( const string& key, string& content, bool download );
	/**
	*		#2013.11.19		by jszhou2
	*		func:删除文件
	*		@param:
	*		key:要删除文件的关键字
	*/
	static void Delete( const string& key ) ;
	/**
	*		#2013.11.18		by jszhou2
	*		func:逆初始化接口
	*/
	static void UnInit( );
private:
	static void callback( string url, proto_callback cb );
	ProtoManager(){}
	static boost::shared_ptr<CWorkPool> g_pPool ;
	static boost::shared_ptr<ProtoCache> g_pProtoCache ; 
};
#endif
