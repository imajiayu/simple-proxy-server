#ifndef _PROXYSERVER_H_
#define _PROXYSERVER_H_

#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <unordered_map>
#include "client.h"
#include "ProxyClient.h"
#include "utils.h"
#include "dLog.h"


using namespace std;

class ProxyServer {
private:
  int serv_sock;                          //proxy作为服务器端的socket描述符
  struct sockaddr_in serv_addr;           //服务器端的地址
  int proxy_port;                         //代理端口号(20530~20539)
  char *remote_ip;                         //测试网站的ip
  int remote_port;                        //测试网站的端口号
  unordered_map<int, Client*> *clnt_map;
  int epollfd;                            //epollfd标识符
  struct epoll_event ev, events[MAX_EVENTS];
  /*     functions()   */

private:
  int initSocket(int port);     //生成并返回serv_sock
  void closeSocket();            //关闭socket
  int serv_accept();            //接受从客户端发送的连接请求
  Client *getClient(int fd);    //返回socket=fd的Client对象
  Client *getProxyClientOwner(int fd);//
  void handleClient(Client *cl);
  void setNonblock(int sockfd);   //设置非阻塞
public:
    ProxyServer(int myport, char *ip, int port);
    ~ProxyServer();
    void disconnect(Client *cl);
    void run();
};

#endif
