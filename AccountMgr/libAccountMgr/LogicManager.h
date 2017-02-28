#ifndef LOGICMANAGER_H
#define LOGICMANAGER_H

#include "DataDefine.h"
#include <map>
using namespace std;

class LogicManager
{
public: 
        LogicManager();
        ~LogicManager();

	static LogicManager *m_pInstance;
	static LogicManager* GetInstance();

public:
        void    AddNewHttpReq(HTTPREQOBJ reqObj);
	void 	RemoveHttpReq(int nIndex);
	int 	GetHttpReqCount();
	
	void 	DealLogicFun();
	int 	CreateLogicThread();
	static  void* DealLogicThread(void *arg);

	HTTPREQTYPE GetReqType(string strInterface);	//获取请求类型

	//处理业务函数
	void 	WriteDataToSys(HTTPREQOBJ reqObj);	//写入数据
	void 	Authorize(HTTPREQOBJ reqObj);		//授权
	void	ReadData(HTTPREQOBJ reqObj);		//读取数据

	//返回Http请求响应数据
	static  size_t HttpDataWriter(void* data, size_t size, size_t nmemb, void* content);
	bool	SendHttpPostToBlockChain(string strUrl, string strData, string &data);	//向区块链发送请求

	//序列化
	string	SerializeAcc(Account acc);
	Account	UnSerializeAcc(string strContent);

	//读写文件
	void	WriteAccInfoToFile(map<string, Account> accMap);
	void	ReadAccInfoFromFile();
	
	map<string, Account> GetAccountMap();
	void SetAccountMap(map<string, Account> accMap);

	bool	CheckRight(int nRight, ACCRIGHT eRigType);//检查是否有该权限

private:
	map<int, HTTPREQOBJ>	m_httpReqMap;		//Http请求Map	int:请求编号 HTTPREQOBJ:请求对象

	pthread_t m_threadID;

	map<string, Account>	m_accountMap;		//用户账户信息	
};

extern LogicManager *g_pLogicManager;

#endif
