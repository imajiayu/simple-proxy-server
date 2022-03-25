#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "utils.h"
#include "ProxyClient.h"

class Client
{
private:
    int clnt_sock;                //浏览器socket描述符
    struct sockaddr_in serv_addr; //ProxyServer的地址
    ProxyClient *p_clnt;          //ProxyClient对象，每次浏览器给代理服务器发连接请求都得建一个

public:
    Client(int fd, struct sockaddr_in addr);
    ~Client();
    int proxyConnect(char *host_ip, int proclnt_port, int port = HTTPD_PORT); //ProClient连接到测试网站
    int getSocket();                                                          //返回socket描述符
    char *getClientIP();                                                      //返回IP地址，没啥用目前
    ProxyClient *getProxyClient();                                            //返回对应的ProxyClient对象
    struct sockaddr_in clnt_addr; //浏览器socket地址
};

#endif
