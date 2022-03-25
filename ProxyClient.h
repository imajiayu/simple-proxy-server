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
    int clnt_sock;                //proxy��Ϊ�ͻ���socket������
    int clnt_port;                //proxy�˿ںţ���ĿҪ�����ûɶ��Ŀǰ
    bool running;                 //����״̬

    void setNonblock(int sockfd);

public:
    ProxyClient(int port, struct sockaddr_in host_addr);
    ~ProxyClient();
    void handleServer(char* remote_ip,struct sockaddr_in clnt_addr,int sockfd); //����ӷ���˷�����������,���͸�ָ���ͻ���
    int initSocket();              //���ɲ�����clnt_sock
    int clnt_connect();            //��Զ�̷��������ӣ��������ӳɹ���clnt_sock��ʧ����ر�clnt_sock
    int getSocket();               //�õ�socket;
    bool isRunning();              //�ж��Ƿ�ProxyClient��������
    void disconnect();             //�ͷ�����
    struct sockaddr_in serv_addr; //Զ�̷�������ַ
};

#endif
