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
	string	m_strCurrPath;										//��ǰ����·��
	string	GetCurrentPath();									//��ȡ��ǰ����·��
	bool	RemoveDir(string strDirPath);						//ɾ��Ŀ¼��Ŀ¼�������ļ�
		
	void	GetIniFileInfo(string strFileName);					//��ȡINI�����ļ���Ϣ


	//�����������
	string	ChangeSendData();
	//��������Http����
	bool	SendHttpRequest(string strUrl, string strData, string strRawData);
	//���ͻ�ȡ��������
	bool	SendGetDataHttpRequest(string strUrl, string strData, string strRawData);
	//����Http������Ӧ����
	static size_t HttpDataWriter(void* data, size_t size, size_t nmemb, void* content);

	//��������json����,������
	bool	AnaResultInfo(string strJson, string &strRawData, string &strMac, string &strDataHash);	

	

	curl_slist *m_plist;
	string		m_strUrl;			//�����ַ
	string		m_strSendData;		//����

	string		m_strGetDataUrl;	//��ȡ���ݵ�ַ
	string		m_strGetDataParam;	//��ȡ���ݲ���

	//���ʹ���
	int			m_nSendTimes;		//���ʹ���
	CRITICAL_SECTION		m_sendDataCS;
	//���մ���
	int			m_nSuccCount;		//�ɹ�����
	int			m_nReqDataErrCount;	//����ɹ��������ݴ���
	int			m_nReqErrCount;		//����ʧ�ܴ���
	CRITICAL_SECTION		m_recvDataCS;

	map<string, int>		m_nodeRecvdCountMap;


	CRITICAL_SECTION		m_logCS;
	//��ӡ��־
	bool					m_bIsPrintLog;
	static void OutputLog(const std::string& szOutputFileName, const std::string&  szMessage);

	std::string UTF8_To_string(const std::string & str);

private:
	static GlobalInfo	*m_pInstance;
};

extern GlobalInfo *g_pGlobalInfo;

#endif _GLOBAL_H_