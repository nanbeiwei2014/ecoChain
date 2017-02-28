#include "HttpServer.h"
#include <event.h>
#include <evhttp.h>
#include <string.h> 
#include "LogicManager.h"

#define BUF_MAX 1024*512

HttpServer *g_pHttpServer = NULL;
HttpServer *HttpServer::m_pInstance = NULL;

int HttpServer::m_nIndex = 0;

HttpServer::HttpServer()
{
	m_nPort = 0;
	m_strIP = "";
	
}

HttpServer::~HttpServer()
{
	StopHttpServer();
}

//获取单实例
HttpServer* HttpServer::GetInstance()
{
        NULL == m_pInstance ? m_pInstance = new HttpServer : 0;

        g_pHttpServer = m_pInstance;

        return m_pInstance;
}

//初始化WinSock
int HttpServer::InitWinSock()
{
	return 0;
}

//设置IP和端口
void HttpServer::SetPortAndIP(int nPort, const char* szIP)
{
	m_nPort = nPort;
	m_strIP = szIP;
}

//初始化服务
bool HttpServer::InitHttpServer()
{
	m_base = event_base_new();
	//if (-1 == this->InitWinSock()) 
	//{
	//	event_base_free(m_base);
	//	return false;
	//}

	m_httpServer = evhttp_new(m_base);
	if (!m_httpServer)
	{
		printf("=HttpServer.cpp====m_httpServer create failed!=============\r\n");
		return false;
	}

	int ret = evhttp_bind_socket(m_httpServer, m_strIP.c_str(), m_nPort);
	if (ret != 0)
	{
		printf("=HttpServer.cpp====m_httpServer bind IP and PORT failed!=============\r\n");
		return false;
	}

	evhttp_set_gencb(m_httpServer, DealHttpRequest, NULL);

	printf("http server start OK! \n");

	//打印网络模式
	char *pNetMode = (char*)event_base_get_method(m_base);
	printf(pNetMode);
	
	return true;
}

//
void HttpServer::StartEventDispatch()
{
	event_base_dispatch(m_base);
}

//停止服务
void HttpServer::StopHttpServer()
{
	evhttp_free(m_httpServer);

	if (m_base)
	{
		event_base_free(m_base);
	}
}

void HttpServer::DealHttpRequest(struct evhttp_request *req, void *arg)
{
	m_nIndex++;
	
	printf("****Recv Http Request!**********************\r\n");

	if (req->type == EVHTTP_REQ_GET)
	{
		HTTPREQOBJ newReqObj;
                newReqObj.m_reqObj = req;
                newReqObj.m_nIndex = m_nIndex;
                newReqObj.m_eReqType = eWriteData;
                newReqObj.m_strUrl = req->uri;
                newReqObj.m_charSet = "utf-8";
                newReqObj.m_strReqData = "";
                LogicManager::GetInstance()->AddNewHttpReq(newReqObj);
	}
	if (req->type == EVHTTP_REQ_POST)
	{
		//请求接口
		string strUrl = req->uri;

		//请求类型
                string strInterface = evhttp_decode_uri(strUrl.c_str());
		HTTPREQTYPE eType = LogicManager::GetInstance()->GetReqType(strInterface);
		
		//请求字符集
		struct evkeyvalq *inputHeader = req->input_headers;
		string strHeader = evhttp_find_header(inputHeader, "Content-Type");

		int nPos = strHeader.find("charset");
		string strCharSet = strHeader.substr(nPos + 8, strHeader.length() - nPos + 1);

		//请求数据
		//获取POST方法的数据
		string strPostData;
		size_t postDataSize = EVBUFFER_LENGTH(req->input_buffer);
		if (postDataSize > 0)
		{
			char szDaBuf[BUF_MAX];
			size_t copyLen = postDataSize > BUF_MAX ? BUF_MAX : postDataSize;
			memcpy(szDaBuf, EVBUFFER_DATA(req->input_buffer), copyLen);
			strPostData.assign(szDaBuf, copyLen);
		}
		else
		{       
			return;                                                                               
                }
                
		string strData = strPostData + "\r\n";
		printf(strData.c_str());

		HTTPREQOBJ newReqObj;
                newReqObj.m_reqObj = req;
                newReqObj.m_nIndex = m_nIndex;
                newReqObj.m_eReqType = eType;
                newReqObj.m_strUrl = strUrl;
                newReqObj.m_charSet = strCharSet;
                newReqObj.m_strReqData = strPostData;
                LogicManager::GetInstance()->AddNewHttpReq(newReqObj);
	}	
}
