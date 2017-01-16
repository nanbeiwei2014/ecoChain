#include "stdafx.h"

#include "GlobalInfo.h"

#include <windows.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <Wtsapi32.h>
#include <UserEnv.h>
#include <Iphlpapi.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <io.h>
#include <ctime>
#include <direct.h>
#include "json.h"
//#include "ConstDefine.h"
#include "curl/curl.h"


#define BUF_LEN 2048

// static variables
GlobalInfo* GlobalInfo::m_pInstance = NULL;
GlobalInfo 	*g_pGlobalInfo;

GlobalInfo::GlobalInfo()
{
	fvalid=true;
	falsecount=0;
	test_data="abcdefghigklmnopq";
	m_pInstance = this;

	string strModulePath = GetCurrentPath();
	m_strCurrPath = strModulePath.substr(0, strModulePath.find_last_of("/\\")+1);

	m_bIsPrintLog = false;
	InitializeCriticalSection(&m_logCS);

	m_plist = curl_slist_append(NULL, "Content-Type:application/json;charset=unicode");//UTF-8");
	m_strUrl = "";		//请求地址

	m_nSendTimes = 0;
	InitializeCriticalSection(&m_sendDataCS);

	m_nSuccCount = 0;
	m_nReqDataErrCount = 0;
	m_nReqErrCount = 0;
	InitializeCriticalSection(&m_recvDataCS);
}

GlobalInfo::~GlobalInfo()
{
	DeleteCriticalSection(&m_logCS);
	DeleteCriticalSection(&m_sendDataCS);
	DeleteCriticalSection(&m_recvDataCS);
}

GlobalInfo* GlobalInfo::GetInstance()
{
	nullptr == m_pInstance ? m_pInstance = new GlobalInfo : 0;

	g_pGlobalInfo = m_pInstance;

	return m_pInstance;
}

//获取当前程序路径
string GlobalInfo::GetCurrentPath()
{
	char szPath[MAX_PATH];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	string curPath = szPath;
	curPath = curPath.substr(0, curPath.find_last_of("/\\") + 1);

	//SetCurrentDirectory(curPath.c_str());

	return curPath;
}

//删除目录及目录下所有文件
bool GlobalInfo::RemoveDir(string strDirPath)
{
	//WIN32_FIND_DATA wfd;
	//HANDLE hFind;
	//string sFullPath;
	//string sFindFilter;
	//DWORD dwAttributes = 0;

	//sFindFilter = strDirPath;
	//sFindFilter += "\\*.*";
	//if ((hFind = FindFirstFileA(sFindFilter.c_str(), &wfd)) == INVALID_HANDLE_VALUE)
	//{
	//	return false;
	//}

	//do
	//{
	//	if (strcmp(wfd.cFileName, ".") == 0 || strcmp(wfd.cFileName, "..") == 0)
	//	{
	//		continue;
	//	}

	//	sFullPath = strDirPath;
	//	sFullPath += '\\';
	//	sFullPath += wfd.cFileName;

	//	//去掉只读属性  
	//	dwAttributes = GetFileAttributes(sFullPath.c_str());
	//	if (dwAttributes & FILE_ATTRIBUTE_READONLY)
	//	{
	//		dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	//		SetFileAttributes(sFullPath.c_str(), dwAttributes);
	//	}

	//	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	//	{
	//		//printf("进入目录%s\n", sFullPath.c_str());
	//		RemoveDir(sFullPath);
	//		RemoveDirectory(sFullPath.c_str());
	//		//printf("删除目录%s成功\n", sFullPath.c_str());
	//	}
	//	else
	//	{
	//		DeleteFile(sFullPath.c_str());
	//		//printf("文件%s删除成功\n", sFullPath.c_str());
	//	}
	//} while (FindNextFile(hFind, &wfd));

	//FindClose(hFind);

	//int nRes = RemoveDirectory(strDirPath.c_str());
	//if (nRes == 0)
	//{
	//	DWORD dwRes = GetLastError();
	//	std::ostringstream ostr;
	//	ostr << "RemoveDirectory error : " << dwRes << "   dir : " << strDirPath.c_str();
	//	string strProInfo = ostr.str();
	//	OutputLog("KMWATCH.log", strProInfo);
	//}

	return true;
}

//获取INI配置文件信息
void GlobalInfo::GetIniFileInfo(string strFileName)
{
	string strPath = GetCurrentPath();
	string strRCMini = strPath + "\\a.ini";
	string strKMWATCHini = strPath + strFileName;

	//string szIP = "";
	//GetPrivateProfileString("KMRCMSTATE", "IP", "", (LPSTR)szIP.c_str(), 256, strKMWATCHini.c_str());
	//m_strIp = szIP.c_str();
	//string szCurrVer = "";
	//GetPrivateProfileString("KMRCMSTATE", "CV", "", (LPSTR)szCurrVer.c_str(), 256, strKMWATCHini.c_str());
	//m_strCurVersion = szCurrVer.c_str();
	//string szOldVer = "";
	//GetPrivateProfileString("KMRCMSTATE", "OV", "", (LPSTR)szOldVer.c_str(), 256, strKMWATCHini.c_str());
	//m_strOldVer = szOldVer.c_str();
	//string szIsPrintLog = "";
	//GetPrivateProfileString("KMRCMSTATE", "PL", "", (LPSTR)szIsPrintLog.c_str(), 256, strKMWATCHini.c_str());
	//string strPrintlog = szIsPrintLog.c_str();
}

//变更数据
string GlobalInfo::ChangeSendData()
{
	int nTmpTimes = 0;
	EnterCriticalSection(&m_sendDataCS);
	nTmpTimes = m_nSendTimes++;
	//LeaveCriticalSection(&m_sendDataCS);

	Json::Value sendObj;
	Json::Reader reader;
	bool bRes = reader.parse(m_strSendData, sendObj);
	if (!bRes)
	{
		return "";
	}

	string strAddr;
	Json::Value addrVal = sendObj.get("address", "");
	strAddr = addrVal.asCString();

	string strData;
	Json::Value dataVal = sendObj.get("data", "");
	strData = dataVal.asCString();

	//改变数据
	string strTmp = "1234567890";
	string strExtra;
//	if(m_strSendData.length() < 1024*510)
//	{
//		for(int i=0; i<nTmpTimes * 100; i++)
//		{
//			strExtra += strTmp;
//		}
//	}
//	else
//	{
//		for(int i=0; i<nTmpTimes; i++)
//		{
//			strExtra += strTmp;
//		}
//}
	//if (fvalid)
	//{
	//		test_data += test_data;
	//}
	//else{
	//	string  strTmp(test_data,0,test_data.size()/2);
	//	falsecount++;
	//	string  strHalf(strTmp,0,strTmp.size()/2);
	//	test_data=strTmp+strHalf;
	//}
	//strData=test_data;
	std::stringstream ss;  
	ss << strData << "***==" << nTmpTimes << "==***" << strExtra;  
	string strNewData = ss.str();
	sendObj["data"] = strNewData;

	string strSign;
	Json::Value signVal = sendObj.get("sign", "");
	strSign = signVal.asCString();

	//替换字符
	string strNewJson = sendObj.toStyledString();
	//while(true)   
	//{     
 //       string::size_type   pos(0);     
 //       if( (pos = strNewJson.find("\n")) != string::npos )
	//	{
 //           strNewJson.replace(pos, 1, "");

	//		int nSize = strNewJson.size();
	//		if(pos == nSize)
	//		{
	//			break;
	//		}
	//	}
	//}
	LeaveCriticalSection(&m_sendDataCS);

	return strNewJson;
}

size_t GlobalInfo::HttpDataWriter(void* data, size_t size, size_t nmemb, void* content)  
{  
    long totalSize = size*nmemb;  
    std::string* symbolBuffer = (std::string*)content;  
    if(symbolBuffer)  
    {  
        symbolBuffer->append((char *)data, ((char*)data)+totalSize);  
    }  
    return totalSize;  
} 

//发送数据Http请求
bool GlobalInfo::SendHttpRequest(string strUrl, string strData, string strRawData)
{
	EnterCriticalSection(&g_pGlobalInfo->m_logCS);

	//
	CURL	*curl = curl_easy_init();
	if (NULL == curl) 
	{
		return false;
	}
	
	CURLcode code;

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

	//设置超时
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3000);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3000);
	
	//设置请求模式
	code = curl_easy_setopt(curl, CURLOPT_POST, true);
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_POST " << std::endl;  
        return false;  
    }  

	// 设置http发送的内容类型为JSON
	code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, m_plist);
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_HEADERS " << std::endl;  
        return false;  
    } 
	
	// 设置要POST的JSON数据
	code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strData.c_str());
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_HEADERS " << std::endl;  
        return false;  
    } 
	
	//返回数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpDataWriter);
	string strRes;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strRes);
	
	//发送
	CURLcode res = curl_easy_perform(curl);

	//
	string	strRetRawData, strMac, strDataHash;

	bool bRes = false;
	if (res == CURLE_OK)
	{
		if (strRes.length() > 0)
		{
			//解析返回数据
			bool bAnaRes = AnaResultInfo(strRes, strRetRawData, strMac, strDataHash);
			if(bAnaRes)	//成功
			{
				m_nSuccCount++;
			}
			else		//失败
			{
				m_nReqDataErrCount++;
			}

			bRes = true;
		}

		if(strRes.length() <= 0)
		{
			strRes = "recv null====================";
		}
	}
	else
	{
		//strRes = "request failed!====================";
		res;
		m_nReqErrCount++;
		bRes = false;
	}

	map<string, int>::iterator iIter = m_nodeRecvdCountMap.find(strMac);
	if(iIter == m_nodeRecvdCountMap.end())
	{
		m_nodeRecvdCountMap.insert(make_pair(strMac, 1));
	}
	else
	{
		iIter->second++;
	}
	
	GlobalInfo::OutputLog("sendReqData.log", strRawData);
	GlobalInfo::OutputLog("RecvRes.log", strRes);

	GlobalInfo::OutputLog("dataHash.log", strDataHash);

	curl_easy_cleanup(curl); 
	curl_global_cleanup();

	LeaveCriticalSection(&g_pGlobalInfo->m_logCS);

	return bRes;
}

//解析返回json数据
bool GlobalInfo::AnaResultInfo(string strJson, string &strRawData, string &strMac, string &strDataHash)
{
	Json::Value recvObj;
	Json::Reader reader;
	bool bRes = reader.parse(strJson, recvObj);
	if (!bRes)
	{
		return false;
	}

	string strResult;
	Json::Value resVal = recvObj.get("result", "");
	bool bResult = resVal.isNull();
	if(bResult)
	{
		return false;
	}
	else
	{
		//解析返回数据
		if(resVal.isObject())
		{
			Json::Value dataHashVal = resVal.get("dataHash", "");
			if(dataHashVal.isString())
			{
				strDataHash = dataHashVal.toStyledString();
				strDataHash = strDataHash.substr(0, strDataHash.length() - 1);
			}

			Json::Value dataVal = resVal.get("data", "");
			if(dataVal.isString())
			{
				string strData = dataVal.toStyledString();

				int nPosStart = strData.find("***==");
				int nPosEnd = strData.find("==***");

				strRawData = strData.substr(1, nPosEnd - 1 + 5);
				strMac = strData.substr(nPosEnd + 5, 17);
			}
		}
	}

	string strErr;
	Json::Value errVal = recvObj.get("error", "");
	if(!errVal.isNull())
	{
		//解析返回错误


		return false;
	}

	return true;
}

//发送获取数据请求
bool GlobalInfo::SendGetDataHttpRequest(string strUrl, string strData, string strRawData)
{
	//
	CURL	*curl = curl_easy_init();
	if (NULL == curl) 
	{
		return false;
	}
	
	CURLcode code;

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

	//设置超时
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3000);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3000);
	
	//设置请求模式
	code = curl_easy_setopt(curl, CURLOPT_POST, true);
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_POST " << std::endl;  
        return false;  
    }  

	//// 设置http发送的内容类型为JSON
	code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, m_plist);
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_HEADERS " << std::endl;  
        return false;  
    } 
	
	int nlen = strData.length();
	char *pchar = new char[strData.length()];
	memset(pchar, 0, strData.length());
	memcpy(pchar, strData.c_str(), strData.length());

	// 设置要POST的JSON数据
	//code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strData.c_str());
	code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pchar);
	if (code != CURLE_OK)  
    {  
        std::cout << stderr << " Failed to set CURLOPT_HEADERS " << std::endl;  
        return false;  
    } 
	
	//返回数据
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpDataWriter);
	string strRes;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strRes);
	
	//发送
	CURLcode res = curl_easy_perform(curl);

	strRes = UTF8_To_string(strRes);

	//
	string	strRetRawData, strMac, strDataHash;

	bool bRes = false;
	if (res == CURLE_OK)
	{
		if (strRes.length() > 0)
		{
			bRes = true;
		}

		if(strRes.length() <= 0)
		{
			strRes = "recv null====================";
		}
	}
	else
	{
		strRes = "request failed!====================";

		m_nReqErrCount++;
		bRes = false;
	}

	
	GlobalInfo::OutputLog("getData.log", strRawData);
	GlobalInfo::OutputLog("getData.log", strRes);

	curl_easy_cleanup(curl); 
	curl_global_cleanup();

	return true;
}

std::string GlobalInfo::UTF8_To_string(const std::string & str) 
{ 
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2); 

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	memset(pBuf, 0, nLen + 1); 

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr = pBuf; 

	delete []pBuf; 
	delete []pwBuf; 

	pBuf = NULL; 
	pwBuf = NULL; 

	return retStr; 
} 

//打印日志
void GlobalInfo::OutputLog(const std::string& szOutputFileName, const std::string&  szMessage)
{
	////判断是否打印日志
	//if (!m_bIsPrintLog)
	//{
	//	return;
	//}

	if (szOutputFileName.length() <= 0) {
		return;
	}

	//当前目录
	char bufDir[1024];
	memset(bufDir, 0, 1024);
	//getcwd(bufDir, 1024);
	GetCurrentDirectoryA(MAX_PATH, bufDir);

	std::string strAppPath = bufDir;
	strAppPath += ("/log/");

	//判断当前目录是否存在，如果不存在则创建
	//if (opendir(strAppPath.c_str()) == NULL)
	if (fopen(strAppPath.c_str(), "r") == NULL)
	{
		//mkdir(strAppPath.c_str(), 774);
		if (CreateDirectoryA(strAppPath.c_str(), NULL))
		{
			CreateDirectoryA(strAppPath.c_str(), NULL);
		}
	}

	//保存路径
	string strFilePath = strAppPath + szOutputFileName;
	ofstream fout(strFilePath.c_str(), ios::out | ios::app);
	if (fout.fail())
	{
		return;
	}
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	//
	char buf[128] = { 0 };
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S", timeinfo);
	string strTime(buf);
	string szTmp = strTime + " :: " + szMessage + "\n";

	fout.write(szTmp.c_str(), szTmp.size());
	fout.close();
}