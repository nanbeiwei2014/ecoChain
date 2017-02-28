#include <stdio.h>
#include "AccountMgr.h"
#include "LogicManager.h"
#include "HttpServer.h"

void StartAccMgrSvr(const char* szIp, int nPort)
{
    //启动工作线程
    if(g_pLogicManager == NULL)
    {
	LogicManager::GetInstance();
    }
    g_pLogicManager->CreateLogicThread();

    //读取账户信息
    g_pLogicManager->ReadAccInfoFromFile();    

    //启动HttpServer服务    
    if(g_pHttpServer == NULL)
    {
        HttpServer::GetInstance();
    }

    g_pHttpServer->SetPortAndIP(nPort, szIp);		//18150, "192.168.88.147");
    g_pHttpServer->InitHttpServer();
    g_pHttpServer->StartEventDispatch();
}

//停止服务
void StopSvr()
{
    //停止服务
    g_pHttpServer->StopHttpServer();

    //停止工作线程 
    //g_pLogicManager
    
    //写入账户信息
    map<string, Account> accMap = g_pLogicManager->GetAccountMap();
    g_pLogicManager->WriteAccInfoToFile(accMap);
}
