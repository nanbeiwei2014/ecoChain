/*File : http.c
 *Auth : sjin
 *Date : 20141206
 *Mail : 413977243@qq.com
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "http.h"

#define BUFFER_SIZE 1024

#define HTTP_GET "GET /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"
#define HTTP_POST "POST /%s HTTP/1.1\r\nHOST: %s:%d\r\nContent-Type:text/xml;charset=UTF-8\r\nContent-Length: %d\r\n\r\n%s"

static int http_tcpclient_create(const char *host, int port){
    struct hostent *he;
    struct sockaddr_in server_addr; 
    int socket_fd;

    if((he = gethostbyname(host))==NULL){
        return -1;
    }

   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1){
        return -1;
    }

    if(connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
        return -1;
    }

    return socket_fd;
}

static void http_tcpclient_close(int socket)
{
    close(socket);
}

static int http_parse_url(const char *url,char *host,char *file,int *port)
{
    char *ptr1,*ptr2;
    int len = 0;
    if(!url || !host || !file || !port){
        return -1;
    }

    ptr1 = (char *)url;

    if(!strncmp(ptr1,"http://",strlen("http://"))){
        ptr1 += strlen("http://");
    }else{
        return -1;
    }

    ptr2 = strchr(ptr1,'/');
    if(ptr2){
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host,ptr1,len);
        host[len] = '\0';
        if(*(ptr2 + 1)){
            memcpy(file,ptr2 + 1,strlen(ptr2) - 1 );
            file[strlen(ptr2) - 1] = '\0';
        }
    }else{
        memcpy(host,ptr1,strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }
    //get host and ip
    ptr1 = strchr(host,':');
    if(ptr1){
        *ptr1++ = '\0';
        *port = atoi(ptr1);
    }else{
        *port = MY_HTTP_DEFAULT_PORT;
    }

    return 0;
}


static int http_tcpclient_recv(int socket,char *lpbuff){
    int recvnum = 0;

    recvnum = recv(socket, lpbuff,BUFFER_SIZE* 101,0);

    return recvnum;
}

static int http_tcpclient_send(int socket,char *buff,int size){
    int sent=0,tmpres=0;

    while(sent < size){
        tmpres = send(socket,buff+sent,size-sent,0);
        if(tmpres == -1){
            return -1;
        }
        sent += tmpres;
    }
    return sent;
}

static char * http_parse_result2(const char*lpbuf)
{
    char *ptmp = NULL; 
    char *response = NULL;
	
    ptmp = (char*)strstr(lpbuf,"HTTP/1.1");
	if(!ptmp)
	{
        printf("http/1.1 not faind\n");
        return NULL;
    }
	
    if(atoi(ptmp + 9) != 200 && atoi(ptmp + 9) != 500)
	{
        printf("result:%s\n",lpbuf);
        return NULL;
    }
	
    ptmp = (char*)strstr(lpbuf,"\r\n\r\n");
    if(!ptmp){
        printf("ptmp is NULL\n");
        return NULL;
    }
	strcpy(response,ptmp+4);

    return response;
}


static int http_parse_result(int socket, char **response,const char*lpbuf, int ibufLen)
{
	char buf[1024];
    //int i = 0;
	//int n = 0, 
	int m = 0;
	int sum = 0;
    char *ptmp = NULL; 
    //char *response = NULL;
	char *conLength = NULL;
	int ilength = 0;
	char *content_ptr = NULL;
	
    ptmp = (char*)strstr(lpbuf,"HTTP/1.1");
	conLength = (char*)strstr(lpbuf,"Content-Length:");

	if(!ptmp)
	{
        printf("http/1.1 not faind\n");
        return -1;
    }
	
    if(atoi(ptmp + 9) != 200 && atoi(ptmp + 9) != 500)
	{
        printf("result:%s\n",lpbuf);
        return -1;
    }

	if((ilength = atoi(conLength + 16)) < 0)
	{
		printf("ilength:%d\n",ilength);
        return -1;
	}

    ptmp = (char*)strstr(lpbuf,"\r\n\r\n");
    if(!ptmp){
        printf("ptmp is NULL\n");
        return -1;
    }
	
	m =  strlen(ptmp+4);
    *response = (char *)malloc(ilength+1);
    if((*response) == NULL)
	{
        printf("malloc failed \n");
        return -1;
    }
	memset((*response), 0, ilength+1);
	content_ptr = *response;
	
	if(ilength > m)
	{
		strcpy(content_ptr, ptmp+4);
		sum += m;
		
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			int nLen = recv(socket, buf, sizeof(buf), 0);
			if(nLen <= 0)
			{
				close(socket);
				break;                 
			}
			memcpy(content_ptr + sum, buf, nLen);
			sum += nLen;
			if(sum == ilength) break;
		}
	}
	else
	{
		strcpy(*response,ptmp+4);
	}

    return 0;
}

char * http_post(const char *url,const char *post_str){

    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE * 101] = {'\0'};
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    int port = 0;
	char *result = NULL;

    if(!url){
        printf("      failed!\n");
        return NULL;
    }

    if(http_parse_url(url,host_addr,file,&port)){
        printf("http_parse_url failed!\n");
        return NULL;
    }

    socket_fd = http_tcpclient_create(host_addr,port);
    if(socket_fd < 0)
	{
        printf("http_tcpclient_create failed\n");
        return NULL;
    }

	if(post_str != NULL)
	{
		sprintf(lpbuf,HTTP_POST,file,host_addr,port,(int)strlen(post_str),post_str);
	}
	else if(post_str == NULL)
	{
		sprintf(lpbuf,HTTP_POST,file,host_addr,port,0,"");
	}

    if(http_tcpclient_send(socket_fd,lpbuf,strlen(lpbuf)) < 0){
        printf("http_tcpclient_send failed..\n");
        return NULL;
    }

	memset(lpbuf, 0, sizeof(lpbuf));
	if(http_tcpclient_recv(socket_fd,lpbuf) <= 0)
	{
        printf("http_tcpclient_recv failed:%s\n",lpbuf);
		http_tcpclient_close(socket_fd);
        return NULL;
    }
	http_parse_result(socket_fd, &result, lpbuf, (BUFFER_SIZE * 101));
	http_tcpclient_close(socket_fd);
	return result;
	
}

char * http_get(const char *url)
{
    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE*4] = {'\0'};
    //char *ptmp;
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    int port = 0;

    if(!url){
        printf("      failed!\n");
        return NULL;
    }

    if(http_parse_url(url,host_addr,file,&port)){
        printf("http_parse_url failed!\n");
        return NULL;
    }

    socket_fd =  http_tcpclient_create(host_addr,port);
    if(socket_fd < 0){
        printf("http_tcpclient_create failed\n");
        return NULL;
    }

    sprintf(lpbuf,HTTP_GET,file,host_addr,port);

    if(http_tcpclient_send(socket_fd,lpbuf,strlen(lpbuf)) < 0){
        printf("http_tcpclient_send failed..\n");
        return NULL;
    }

    if(http_tcpclient_recv(socket_fd,lpbuf) <= 0){
        printf("http_tcpclient_recv failed\n");
        return NULL;
    }
    http_tcpclient_close(socket_fd);

    return http_parse_result2(lpbuf);
}
