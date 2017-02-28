#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <event.h>
#include <event2/http.h>
#include <evhttp.h>
#include <event2/buffer.h>
#include <string>

using namespace std;


class HttpServer
{
public:
        HttpServer();
        ~HttpServer();
	
	static HttpServer *m_pInstance;
        static HttpServer* GetInstance();

	int	InitWinSock();					//初始化WinSock
	void	SetPortAndIP(int nPort, const char* szIP);	//设置端口和IP

	bool	InitHttpServer();	//开启http服务
	void	StartEventDispatch();
	void	StopHttpServer();	//停止服务

	static 	int 	m_nIndex;
	static	void DealHttpRequest(evhttp_request *req, void *arg);		//处理http请求并返回响应

private:
        int     m_nPort;                        //端口  
        string  m_strIP;                        //IP

        event_base      *m_base;
        evhttp          *m_httpServer;	
};

extern HttpServer *g_pHttpServer;

#endif
