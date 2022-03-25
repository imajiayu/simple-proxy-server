#ifndef _PROXYCLIENT_H_
#define _PROXYCLIENT_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "dLog.h"

using namespace std;

class ProxyClient
{
private:
    int clnt_sock;                //proxy作为客户端socket描述符
    int clnt_port;                //proxy端口号，题目要求绑定吗？没啥用目前
    bool running;                 //运行状态

    void setNonblock(int sockfd);

public:
    ProxyClient(int port, struct sockaddr_in host_addr);
    ~ProxyClient();
    void handleServer(char* remote_ip,struct sockaddr_in clnt_addr,int sockfd); //处理从服务端发送来的内容,发送给指定客户端
    int initSocket();              //生成并返回clnt_sock
    int clnt_connect();            //与远程服务器连接，返回连接成功的clnt_sock，失败则关闭clnt_sock
    int getSocket();               //得到socket;
    bool isRunning();              //判断是否ProxyClient还在运行
    void disconnect();             //释放连接
    struct sockaddr_in serv_addr; //远程服务器地址
};

#endif
