/*File : http.h
 *Auth : sjin
 *Date : 20141206
 *Mail : 413977243@qq.com
 */
#ifndef _MY_HTTP_H
#define _MY_HTTP_H

#define MY_HTTP_DEFAULT_PORT 8332

char * http_get(const char *url);
char * http_post(const char *url,const char *charset,const int post_str_len,const char *post_str);

#endif
