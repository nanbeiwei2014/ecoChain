#include "stdafx.h"
#include "DataManager.h"

#include <windows.h>
#include <tchar.h>
#include <fstream>
#include "stdio.h"
#include "conio.h"
#include "curl/curl.h"

DataManager 	*g_pGlobal_Data = NULL;
DataManager		*DataManager::m_pInstance = NULL;

DataManager::DataManager()
{
	m_bIsPrintLog = false;
}
	
DataManager::~DataManager()
{
	
}

//��ȡ��ʵ��
DataManager* DataManager::instance()
{
	NULL == m_pInstance ? m_pInstance = new DataManager : 0;

	g_pGlobal_Data = m_pInstance;

	return m_pInstance;
}
//�Ƿ��Ѿ�ʵ����
bool DataManager::isInstance()
{
	return m_pInstance == NULL ? false : true;
}
//�ͷŵ�ʵ��
void DataManager::release()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
		g_pGlobal_Data = NULL;
	}
}

//bool DataManager::getUrl(char *filename)
//{
	//CURL *curl;
	//CURLcode res;
	//FILE *fp;
	//if ((fp = fopen(filename, "w")) == NULL)  // ���ؽ�����ļ��洢
	//	return false;
	//struct curl_slist *headers = NULL;
	//headers = curl_slist_append(headers, "Accept: Agent-007");
	//curl = curl_easy_init();    // ��ʼ��
	//if (curl)
	//{
	//	//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// ����
	//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// ��Э��ͷ
	//	curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
	//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //�����ص�httpͷ�����fpָ����ļ�
	//	curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp); //�����ص�html�������������fpָ����ļ�
	//	res = curl_easy_perform(curl);   // ִ��
	//	if (res != 0) {

	//		curl_slist_free_all(headers);
	//		curl_easy_cleanup(curl);
	//	}
	//	fclose(fp);
	//	return true;
	//}
//	return false;
//}

//bool DataManager::postUrl(char *filename)
//{
	//CURL *curl;
	//CURLcode res;

	//curl = curl_easy_init();
	//if (curl)
	//{
	//	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // ָ��cookie�ļ�
	//	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "&logintype=uid&u=xieyan&psw=xxx86");    // ָ��post����
	//	//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");
	//	curl_easy_setopt(curl, CURLOPT_URL, " http://mail.sina.com.cn/cgi-bin/login.cgi ");   // ָ��url
	//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	//	res = curl_easy_perform(curl);
	//	curl_easy_cleanup(curl);
	//}
	//fclose(fp);
//	return true;
//}
