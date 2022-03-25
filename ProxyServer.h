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
  int serv_sock;                          //proxy��Ϊ�������˵�socket������
  struct sockaddr_in serv_addr;           //�������˵ĵ�ַ
  int proxy_port;                         //����˿ں�(20530~20539)
  char *remote_ip;                         //������վ��ip
  int remote_port;                        //������վ�Ķ˿ں�
  unordered_map<int, Client*> *clnt_map;
  int epollfd;                            //epollfd��ʶ��
  struct epoll_event ev, events[MAX_EVENTS];
  /*     functions()   */

private:
  int initSocket(int port);     //���ɲ�����serv_sock
  void closeSocket();            //�ر�socket
  int serv_accept();            //���ܴӿͻ��˷��͵���������
  Client *getClient(int fd);    //����socket=fd��Client����
  Client *getProxyClientOwner(int fd);//
  void handleClient(Client *cl);
  void setNonblock(int sockfd);   //���÷�����
public:
    ProxyServer(int myport, char *ip, int port);
    ~ProxyServer();
    void disconnect(Client *cl);
    void run();
};

#endif
