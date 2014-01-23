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
	*		func:ͼƬ�����ʼ���ӿ�
	*		@param:
	*		sCachePath:Ҫ�����·��
	*		u64MaxSpace:ͼƬռ�õ����ռ�,�������ռ��ͼƬ��������
	*		nThreadCount:�����߳���Ŀ
	*/
	static bool Init( const string& sCachePath, boost::uint64_t u64MaxSpace = MAX_SPACE_USE, int nThreadCount = 4 );
	/**
	*		#2013.11.18		by jszhou2
	*		func:�첽��ȡ�ļ��ӿ�,����ļ�������,�ļ���������,������
	*		@param:
	*		key:Ҫ���ص��ļ��ؼ���,�����ʾurl
	*		cb�� �첽�ص��ӿ�
	*/
	static void Read( const string& key, proto_callback cb );
	/**
	*		#2013.11.18		by jszhou2
	*		func:ͬ����ȡ�ļ��ӿ�,����ļ�������,�ļ��Ƿ������в���download����,�����ػᱻ����
	*		@param:
	*		key:Ҫ���ص��ļ��ؼ���,�����ʾurl
	*		content:����ȡ�ļ����ڴ汣��ռ�
	*		download:���ļ������ھ����Ƿ�����
	*		@return:��ȡ�ɹ�����ture,���򷵻�false
	*/
	static bool Read( const string& key, string& content, bool download );
	/**
	*		#2013.11.19		by jszhou2
	*		func:ɾ���ļ�
	*		@param:
	*		key:Ҫɾ���ļ��Ĺؼ���
	*/
	static void Delete( const string& key ) ;
	/**
	*		#2013.11.18		by jszhou2
	*		func:���ʼ���ӿ�
	*/
	static void UnInit( );
private:
	static void callback( string url, proto_callback cb );
	ProtoManager(){}
	static boost::shared_ptr<CWorkPool> g_pPool ;
	static boost::shared_ptr<ProtoCache> g_pProtoCache ; 
};
#endif
