#include "LogicManager.h"
#include <unistd.h>
#include "Jsoncpp/json.h"
#include "curl/curl.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

LogicManager *g_pLogicManager = NULL;
LogicManager *LogicManager::m_pInstance = NULL;

LogicManager::LogicManager()
{
}

LogicManager::~LogicManager()
{

}

//获取单实例
LogicManager* LogicManager::GetInstance()
{
	NULL == m_pInstance ? m_pInstance = new LogicManager : 0;

	g_pLogicManager = m_pInstance;

	return m_pInstance;
}

//插入请求对象
void LogicManager::AddNewHttpReq(HTTPREQOBJ reqObj)
{
	if(reqObj.m_nIndex < 0)
        {
                return;
        }       
        //
        m_httpReqMap.insert(make_pair(reqObj.m_nIndex, reqObj));

	printf("=LogicManger.cpp======Insert Http Request obj========================\r\n");
}

void LogicManager:: RemoveHttpReq(int nIndex)
{
	if(nIndex <= 0)
	{
		return;
	}

	m_httpReqMap.erase(nIndex);
}

//获取当前请求数
int LogicManager::GetHttpReqCount()
{
	return m_httpReqMap.size();
}

//获取请求类型
HTTPREQTYPE LogicManager::GetReqType(string strInterface)
{
	if(strInterface.compare("/api/writedata") == 0)
	{
		return eWriteData;
	}
	if(strInterface.compare("/api/author") == 0)
	{
		return eAuthor;
	}
	if(strInterface.compare("/api/readdata") == 0)
        {
                return eReadData;
        }

	return eNull;
}

//业务处理函数
void LogicManager::DealLogicFun()
{
	printf("=LogicManager.cpp======In the logic deal fun==========================================\r\n");
	map<int, HTTPREQOBJ>::iterator iIter;
        for(iIter = m_httpReqMap.begin();
            iIter != m_httpReqMap.end();
            iIter++)
        {       
		int nIndex = iIter->first;
                switch(iIter->second.m_eReqType)
                {       
                case eWriteData:
                {
			WriteDataToSys(iIter->second);
			break;
                }       
                case eAuthor:
                {
			Authorize(iIter->second);
                        break;
                }
		case eReadData:
		{
			ReadData(iIter->second);
			break;
		}
                default:
                {
                        break;
                }
                }

		RemoveHttpReq(nIndex);
        }	
}

//
int LogicManager::CreateLogicThread()
{
	int nRes = pthread_create(&m_threadID, NULL, &DealLogicThread, this);
	if(nRes == 0)
	{
		printf("=LogicManager.cpp=DealLogicThread====startSuccess==============================\r\n");
	}
	else
	{
		printf("=LogicManager.cpp=DealLogicThread====failure===================================\r\n");
	}
       	return nRes;
}

//逻辑处理线程
void* LogicManager::DealLogicThread(void *arg)
{
	LogicManager *pLogic = (LogicManager*)arg;

	while(true)
	{
		while(pLogic->m_httpReqMap.size() > 0)
		{
			pLogic->DealLogicFun();
		}
		sleep(1);
	}

	return 0;
}

//写入数据
void LogicManager::WriteDataToSys(HTTPREQOBJ reqObj)
{
        printf("=LogicManger.cpp========================write data to sys =========================\r\n");
        
	//处理请求信息
	string strJsonData = reqObj.m_strReqData;
	
	Json::Value reqData;
	Json::Reader reader;
	bool bRes = reader.parse(strJsonData, reqData);
	if (!bRes)
	{
		return ;
	}

	string strAccount;
	Json::Value addrVal = reqData.get("account", "");
	strAccount = addrVal.asCString();

	string strDataType;
	Json::Value dataType = reqData.get("dataType", "");
	strDataType = dataType.asCString();		
	
	string strPubKey;
        Json::Value pubKey = reqData.get("pubKey", "");
        strPubKey = pubKey.asCString();

	string strData;
        Json::Value data = reqData.get("data", "");
        strData = data.asCString();

	string strSign;
        Json::Value sign = reqData.get("sign", "");
        strSign = sign.asCString();

	//请求账户信息
	Account	curAcc;
	curAcc.m_strAccount = strAccount;
	curAcc.m_bIsValid = true;
	curAcc.m_accRightMap.insert(make_pair(strDataType, 0x0077));
	curAcc.m_accRightMap.insert(make_pair("2", 0x0066));
	
	printf("=LogicManger.cpp===%s===%s===%s==%s=\r\n", strAccount.c_str(), strDataType.c_str(), strPubKey.c_str(), strSign.c_str());
	bool bWriteRig = true;
	string strResult;
	//查询用户信息
	map<string, Account>::iterator iIter;
	iIter = m_accountMap.find(strAccount);
	if(iIter == m_accountMap.end())
	{
		printf("=LogicManger.cpp===writedata Add new Account======\r\n");
		m_accountMap.insert(make_pair(strAccount, curAcc));

		//=============test==============		
		map<string, int>::iterator iRigIter;
		for(iRigIter = curAcc.m_accRightMap.begin(); iRigIter != curAcc.m_accRightMap.end(); iRigIter++)
		{
			printf("=LogicManger.cpp===Right===%s===%d===\r\n", iRigIter->first.c_str(), iRigIter->second);
		}
	}
	else
	{
		map<string, int >::iterator iRigIter;
		iRigIter = iIter->second.m_accRightMap.find(strDataType);
		if(iRigIter == iIter->second.m_accRightMap.end())
		{
			printf("=LogicManger.cpp===writedata Account exist but has no right======\r\n");
			iIter->second.m_accRightMap.insert(make_pair(strDataType, 0x0077));
                        printf("=LogicManger.cpp===New Type===%s===%d===\r\n", strDataType.c_str(), 0x0077);
		}
		else
		{
			bool bCanWrite = CheckRight(iRigIter->second, eWrite);
			if(!bCanWrite)
			{
				bWriteRig = false;
				strResult = "no right";
				printf("=LogicManger.cpp===write data no right======\r\n");
			}
		}
	}
	printf("=LogicManger.cpp===accountmap size : %d====\r\n", (int)m_accountMap.size());

	if(bWriteRig)
	{
		//向区块连转发请求
		string strUrl = "http://192.168.101.152:8332/send_data_to_sys";

		Json::Value root;  
        	root["publicKey"] = strPubKey;
        	root["data"] = strData; 
		root["sign"] = strSign;
		string strReqData = root.toStyledString();
		printf("=LogicManger.cpp===%s====\r\n", strReqData.c_str());

		bRes = SendHttpPostToBlockChain(strUrl, strReqData, strResult);
		if(!bRes)
		{
			strResult = "request false !";
		}
	}

	//返回结果
	struct evbuffer *buf;
        buf = evbuffer_new();
        evbuffer_add_printf(buf, strResult.c_str());
        evhttp_send_reply(reqObj.m_reqObj, HTTP_OK, "OK", buf);
        evbuffer_free(buf);

	//=======================test write acc data====================
	//写入账户信息
	map<string, Account> accMap = GetAccountMap();
	g_pLogicManager->WriteAccInfoToFile(accMap);
}

//授权
void LogicManager::Authorize(HTTPREQOBJ reqObj)
{
	printf("=LogicManger.cpp========================Authorize =========================\r\n");

	//处理请求信息
        string strJsonData = reqObj.m_strReqData;

        Json::Value reqData;
        Json::Reader reader;
        bool bRes = reader.parse(strJsonData, reqData);
        if (!bRes)
        {
		printf("=LogicManger.cpp=======invalid json str=========================\r\n");
                return ;
        }

        string strMainAccount;
        Json::Value mainAccVal = reqData.get("mainAccount", "");
        strMainAccount = mainAccVal.asCString();

        string strOtherAcc;
        Json::Value othAccVal = reqData.get("otherAccount", "");
        strOtherAcc = othAccVal.asCString();

        string strOptType;
        Json::Value optTypeVal = reqData.get("optType", "");
        strOptType = optTypeVal.asCString();

	bool bRight = false;
	string 	strRes = "";
	//添加账户/修改账户权限
	if(strOptType.compare("0") == 0)
	{
		printf("=LogicManger.cpp=======Add a new account =========================\r\n");
		map<string, int>     rightMap;
		Json::Value objRightArr = reqData["rights"];
		for (int i = 0; i< (int)objRightArr.size(); i++)
		{
  			string strDataType = objRightArr[i]["dataType"].asString();
  			string strRight = objRightArr[i]["right"].asString();

			rightMap.insert(make_pair(strDataType, atoi(strRight.c_str())));

			printf("=LogicManger.cpp=====author type==%s===%s====================\r\n", strDataType.c_str(), strRight.c_str());
		}

		//查找用户信息，如果不存在为添加，如果存在则是修改
		map<string, Account>::iterator iIter;
		iIter = m_accountMap.find(strOtherAcc);
		if(iIter == m_accountMap.end())
		{
			printf("=LogicManger.cpp=======add account !=========================\r\n");

			//判断主账户是否有权限创建新账户
			iIter = m_accountMap.find(strMainAccount);
			if(iIter != m_accountMap.end())
			{
				map<string, int>::iterator iDataTypeIter;
				for(iDataTypeIter = rightMap.begin(); iDataTypeIter != rightMap.end(); iDataTypeIter++)
				{
					printf("=LogicManger.cpp===main account=============================\r\n");
					map<string, int>::iterator iRigIter;
					iRigIter = iIter->second.m_accRightMap.find(iDataTypeIter->first);
					if(iRigIter != iIter->second.m_accRightMap.end())
					{
						bRight = CheckRight(iRigIter->second, eCraAcc);
					}
					else
					{
						strRes = "main account no right";
						break;
					}
				}
			}
			else
			{
				strRes = "no main account";
			}
			
			if(bRight)
			{
				//添加新用户
				Account curAcc;
				curAcc.m_strAccount = strOtherAcc;
				curAcc.m_bIsValid = true;
				curAcc.m_accRightMap = rightMap;
				m_accountMap.insert(make_pair(strOtherAcc, curAcc));
				
				strRes = "Add new account success";
			}

			printf("=LogicManger.cpp=======%s=========================\r\n", strRes.c_str());
		}
		else
		{
			printf("=LogicManger.cpp=======mod account !=========================\r\n");

			//判断主账户是否有修改用户账户权限
			iIter = m_accountMap.find(strMainAccount);
			if(iIter != m_accountMap.end())
			{
				map<string, int>::iterator iDataTypeIter;
                                for(iDataTypeIter = rightMap.begin(); iDataTypeIter != rightMap.end(); iDataTypeIter++)
                                {
					map<string, int>::iterator iRigIter;
					iRigIter = iIter->second.m_accRightMap.find(iDataTypeIter->first);
					if(iRigIter != iIter->second.m_accRightMap.end())
					{
						bRight = CheckRight(iRigIter->second, eModAcc);
					}
					else
                                	{
                                        	strRes = "main account no right";
						break;
                                	}
				}
			}
			else
			{
				strRes = "no account";
			}

			if(bRight)
			{
				//修改旧用户权限
				map<string, int>::iterator iRigIter;
				for(iRigIter = rightMap.begin(); iRigIter != rightMap.end(); iRigIter++)
				{
					iIter->second.m_accRightMap.insert(make_pair(iRigIter->first, iRigIter->second));
				}

				strRes = "Mod account right success";
			}

			printf("=LogicManger.cpp=======%s=========================\r\n", strRes.c_str());
		}
	}
	
	//删除账户
	if(strOptType.compare("1") == 0)
	{
		printf("=LogicManger.cpp=======del account !=========================\r\n");

		vector<string>     dataTypeVec;
                Json::Value objRightArr = reqData["rights"];
                for (int i = 0; i< (int)objRightArr.size(); i++)
                {
                        string strDataType = objRightArr[i]["dataType"].asString();
                        dataTypeVec.push_back(strDataType);
                        
			printf("=LogicManger.cpp=====remove author data type==%s=======================\r\n", strDataType.c_str());
                }
		
		//查找被操作用户信息
		bool bOtherExist = true;
		Account otherAcc;
		map<string, Account>::iterator iOtherIter;
                iOtherIter = m_accountMap.find(strOtherAcc);
                if(iOtherIter == m_accountMap.end())
                {
	                strRes = "no other account exist";
			bOtherExist = false;
                }

	if(bOtherExist == true)
	{
		int nDtrCount = 0;
		//判断主账户是否有删除用户账户权限
		map<string, Account>::iterator iIter;
		iIter = m_accountMap.find(strMainAccount);
		if(iIter != m_accountMap.end())
		{
			vector<string>::iterator iDtIter;
			for(iDtIter = dataTypeVec.begin(); iDtIter != dataTypeVec.end(); iDtIter++)
			{
				map<string, int>::iterator iRigIter;
				iRigIter = iIter->second.m_accRightMap.find(*iDtIter);
				if(iRigIter != iIter->second.m_accRightMap.end())
				{
					printf("=LogicManger.cpp=======right value %d=========================\r\n", iRigIter->second);
					bRight = CheckRight(iRigIter->second, eDelAcc);

					if(bRight)
					{
						nDtrCount++;
					
						map<string, int>::iterator iOtherRigIter;
						iOtherRigIter = iOtherIter->second.m_accRightMap.find(*iDtIter);
				                if(iOtherRigIter != iOtherIter->second.m_accRightMap.end())
						{
							iOtherIter->second.m_accRightMap.erase(iOtherRigIter++);
						}
						strRes = "delete dataType:" + *iDtIter + " right";
					}
					else
					{
						strRes = "has no right";
					}
				}
				else
				{
					strRes = "no this data type right";
				}
			}
		}
		else
		{
			strRes = "no main account";
		}
		
		if(nDtrCount == (int)dataTypeVec.size())
		{
			m_accountMap.erase(iOtherIter++);
			strRes = "Del account success";
		}
	}
		printf("=LogicManger.cpp=======%s=========================\r\n", strRes.c_str());
	}
	
	//返回结果
	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, strRes.c_str());
	evhttp_send_reply(reqObj.m_reqObj, HTTP_OK, "OK", buf);
	evbuffer_free(buf);	
}

//读取数据
void LogicManager::ReadData(HTTPREQOBJ reqObj)
{
	printf("=LogicManger.cpp========================Read Data =========================\r\n");
	//处理请求信息
        string strJsonData = reqObj.m_strReqData;

        Json::Value reqData;
        Json::Reader reader;
        bool bRes = reader.parse(strJsonData, reqData);
        if (!bRes)
        {
                return ;
        }

        string strAccount;
        Json::Value accVal = reqData.get("account", "");
        strAccount = accVal.asCString();

        string strDataType;
        Json::Value dataTypeVal = reqData.get("dataType", "");
        strDataType = dataTypeVal.asCString();

        string strDataHash;
        Json::Value dataHashVal = reqData.get("dataHash", "");
        strDataHash = dataHashVal.asCString();

	printf("=LogicManger.cpp===%s===%s===%s=\r\n", strAccount.c_str(), strDataType.c_str(), strDataHash.c_str());
	
	bool bRight = false;
	string strResult;
	map<string, Account>::iterator iIter;
	//判断主账户是否有删除用户账户权限
	iIter = m_accountMap.find(strAccount);
	if(iIter != m_accountMap.end())
	{
		map<string, int>::iterator iRigIter;
		iRigIter = iIter->second.m_accRightMap.find(strDataType);
		if(iRigIter != iIter->second.m_accRightMap.end())
		{
			bRight = CheckRight(iRigIter->second, eRead);
			if(bRight == false)
			{
				strResult = "no right";
			}
		}
		else
		{
			strResult = "no right";
		}
	}
	else
	{
		strResult = "no account";
	}

	if(bRight)
	{
		//向区块链发送读取数据请求
        	string strUrl = "http://192.168.101.152:8332/get_data_from_sys";
 
                string strReqData = strDataHash;
                printf("=LogicManger.cpp===%s====\r\n", strReqData.c_str());

		bRes = SendHttpPostToBlockChain(strUrl, strReqData, strResult);
        	if(!bRes)
        	{
        	        strResult = "request false !";
        	}
	}
	printf("=LogicManger.cpp======%s=============\r\n", strResult.c_str());

        //返回结果
        struct evbuffer *buf;
        buf = evbuffer_new();
        evbuffer_add_printf(buf, strResult.c_str());
        evhttp_send_reply(reqObj.m_reqObj, HTTP_OK, "OK", buf);
        evbuffer_free(buf);
}

size_t LogicManager::HttpDataWriter(void* data, size_t size, size_t nmemb, void* content)  
{  
    long totalSize = size*nmemb;  
    std::string* symbolBuffer = (std::string*)content;  
    if(symbolBuffer)  
    {  
        symbolBuffer->append((char *)data, ((char*)data)+totalSize);  
    }  
    return totalSize;  
}
//向区块链发送请求
bool LogicManager::SendHttpPostToBlockChain(string strUrl, string strData, string &data)
{
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
	curl_slist *m_plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
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
	if (res == CURLE_OK)
	{
		if (strRes.length() > 0)
		{
			data = strRes;
		}
		if(strRes.length() <= 0)
		{
			strRes = "";
		}
	}
	else
	{
		return false;
	}
	
	curl_easy_cleanup(curl); 
	curl_global_cleanup();

	return true;
}

//序列化
string LogicManager::SerializeAcc(Account acc)
{
	std::ostringstream os;  
    	boost::archive::binary_oarchive oa(os);  
    	oa << acc;
	
	string strCont = os.str();
	return strCont;
}
Account LogicManager::UnSerializeAcc(string strContent)
{
	Account acc;
	std::istringstream is(strContent);
    	boost::archive::binary_iarchive ia(is);
    	ia >> acc;
	
	return acc;
}

//写文件
void LogicManager::WriteAccInfoToFile(map<string, Account> accMap)
{
	//当前目录
	char szWorkDir[260] = {0};
    	if(!getcwd(szWorkDir, 260))  
    	{  
        	return;  
    	}  
  
    	string strFilePath = szWorkDir;  
    	strFilePath.append("/") ;  
    	strFilePath.append("AccData.dat") ; 
	
	//保存路径
	ofstream fout(strFilePath.c_str(), ios::out | ios::app);
	if (fout.fail())
	{
		return;
	}
	
	//
	string strAccesInfo;
	map<string, Account>::iterator iIter;
	for(iIter = accMap.begin(); iIter != accMap.end(); iIter++)
	{
		string strTemp = SerializeAcc(iIter->second);
		printf("=serobj size : %d===%s====\n",  (int)strTemp.size(), strTemp.c_str());
		strAccesInfo += strTemp + "\n";
	}
	fout.write(strAccesInfo.c_str(), strAccesInfo.size());
	fout.close();
}
//读文件
void LogicManager::ReadAccInfoFromFile()
{
        //当前目录
        char szWorkDir[260] = {0};
	if(!getcwd(szWorkDir, 260))
        {
                return;
        }

        string strFilePath = szWorkDir;
        strFilePath.append("/") ;        
        strFilePath.append("AccData.dat");
	printf("=LogicManger.cpp======%s=============\r\n", strFilePath.c_str());

	//
	ifstream inFile;
	inFile.open(strFilePath.c_str(), ios::in);
	if(!inFile)
	{
		printf("=LogicManger.cpp======Account data File don't exist !=============\r\n");
		inFile.close();	
		return;
	}
	
	//读取数据
        vector<string>  strAccInfoVec;
	string strTemp;
	while(getline(inFile, strTemp))   // 若未到文件结束一直循环
	{
		strAccInfoVec.push_back(strTemp);
	}

	int nIndex = 1;
	string strObj;
	vector<string>::iterator iIter;
	for(iIter = strAccInfoVec.begin(); iIter != strAccInfoVec.end(); iIter++)
	{
		strObj += *iIter + "\n";

		if(nIndex % 2 != 0)
		{
			continue;
		}
		
		Account accTemp;
                accTemp = UnSerializeAcc(strObj);
                m_accountMap.insert(make_pair(accTemp.m_strAccount, accTemp));
		
		//=======================tests=============================
		printf("=LogicManger.cpp======AccName:%s===%d=======\r\n", accTemp.m_strAccount.c_str(), accTemp.m_bIsValid);
		map<string, int>::iterator iRigIter;
		for(iRigIter = accTemp.m_accRightMap.begin(); iRigIter != accTemp.m_accRightMap.end(); iRigIter++)
		{
			string strType = iRigIter->first;
		        printf("=DataType:%s========\n", strType.c_str());
		        printf("=Right:%d===========\n", iRigIter->second);
		}
	}
	
 	inFile.close(); 
}

map<string, Account> LogicManager::GetAccountMap()
{
	return m_accountMap;
}
void LogicManager::SetAccountMap(map<string, Account> accMap)
{
	m_accountMap = accMap;
}

//检查是否有该权限
bool LogicManager::CheckRight(int nRight, ACCRIGHT eRigType)
{
	switch(eRigType)
	{
	case eRead:
		{
		int nTemp = nRight & 0x0001;
		if (nTemp == 1)
			return true;
		else
			return false;
		}
		break;
	case eWrite:
		{
		int nTemp = nRight & 0x0002;
                if (nTemp == 2)
                        return true;
                else
                        return false;
		}
                break;
	case eExecute:
		{
		int nTemp = nRight & 0x0004;
                if (nTemp == 4)
                        return true;
                else
                        return false;
		}
                break;
	case eCraAcc:
		{
		int nTemp = nRight & 0x0010;
                if (nTemp == 16)
                        return true;
                else
                        return false;
		}
                break;
	case eModAcc:
		{
		int nTemp = nRight & 0x0020;
                if (nTemp == 32)
                        return true;
                else
                        return false;
		}
                break;
	case eDelAcc:
		{
		int nTemp = nRight & 0x0040;
                if (nTemp == 64)
                        return true;
                else
                        return false;
		}
                break;
	default:
		break;
	}
	return false;
}
