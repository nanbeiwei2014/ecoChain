#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "curl/curl.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class GlobalInfo
{
public:
	GlobalInfo();
	~GlobalInfo();
	static GlobalInfo* GetInstance();

public:
	//
	bool  fvalid;
	string  test_data;
	int     falsecount;
	string	m_strCurrPath;										//当前程序路径
	string	GetCurrentPath();									//获取当前程序路径
	bool	RemoveDir(string strDirPath);						//删除目录及目录下所有文件
		
	void	GetIniFileInfo(string strFileName);					//获取INI配置文件信息


	//变更发送数据
	string	ChangeSendData();
	//发送数据Http请求
	bool	SendHttpRequest(string strUrl, string strData, string strRawData);
	//发送获取数据请求
	bool	SendGetDataHttpRequest(string strUrl, string strData, string strRawData);
	//返回Http请求响应数据
	static size_t HttpDataWriter(void* data, size_t size, size_t nmemb, void* content);

	//解析返回json数据,并保存
	bool	AnaResultInfo(string strJson, string &strRawData, string &strMac, string &strDataHash);	

	

	curl_slist *m_plist;
	string		m_strUrl;			//请求地址
	string		m_strSendData;		//数据

	string		m_strGetDataUrl;	//获取数据地址
	string		m_strGetDataParam;	//获取数据参数

	//发送次数
	int			m_nSendTimes;		//发送次数
	CRITICAL_SECTION		m_sendDataCS;
	//接收次数
	int			m_nSuccCount;		//成功次数
	int			m_nReqDataErrCount;	//请求成功返回数据错误
	int			m_nReqErrCount;		//请求失败次数
	CRITICAL_SECTION		m_recvDataCS;

	map<string, int>		m_nodeRecvdCountMap;


	CRITICAL_SECTION		m_logCS;
	//打印日志
	bool					m_bIsPrintLog;
	static void OutputLog(const std::string& szOutputFileName, const std::string&  szMessage);

	std::string UTF8_To_string(const std::string & str);

private:
	static GlobalInfo	*m_pInstance;
};

extern GlobalInfo *g_pGlobalInfo;

#endif _GLOBAL_H_