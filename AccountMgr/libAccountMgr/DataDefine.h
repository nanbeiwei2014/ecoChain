#ifndef	DATADEFINE_H
#define DATADEFINE_H

#include <evhttp.h>
#include <string>
#include <map>

using namespace std;

enum	HTTPREQTYPE
{
	eNull,
	eWriteData,	//数据写入
	eAuthor,	//授权
	eReadData	//读取数据
};


struct	HTTPREQOBJ
{
public:
	int 		m_nIndex;
	HTTPREQTYPE 	m_eReqType;
	string		m_strUrl;
	string		m_charSet;
	evhttp_request *m_reqObj;	
	string		m_strReqData;	//请求数据
	string		m_strResData;	//响应数据

	void Init()
	{
		m_nIndex = -1;
		m_eReqType = eWriteData;
		m_strUrl = "";
		m_charSet = "";
		m_reqObj = NULL;
		m_strReqData = "";
		m_strResData = "";
	}
};

enum  	ACCRIGHT
{
	eRead,
	eWrite,
	eExecute,
	eCraAcc,
	eModAcc,
	eDelAcc
};

struct 	Account
{
	string			m_strAccount;			//账户字符串
	map<string, int>	m_accRightMap;			//数据类型权限
	bool			m_bIsValid;			//账户是否有效
};

namespace boost {  
    namespace serialization {  
  
        template<class Archive>  
        void serialize(Archive & ar, Account &acc, const unsigned int version)  
        {  
            ar & acc.m_strAccount;
            ar & acc.m_bIsValid;
	    ar & acc.m_accRightMap;
/*	    map<string, int>::iterator iIter;
	    for(iIter = acc.m_accRightMap.begin();
		iIter != acc.m_accRightMap.end();
		iIter++)
	    {
		string strKey = iIter->first;
		ar & strKey;
		int nVal = iIter->second;
		ar & nVal;
	    }
*/        }  
  
    } // namespace serialization  
} // namespace boost

#endif
