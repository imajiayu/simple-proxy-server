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
    int clnt_sock;                //�����socket������
    struct sockaddr_in serv_addr; //ProxyServer�ĵ�ַ
    ProxyClient *p_clnt;          //ProxyClient����ÿ���������������������������󶼵ý�һ��

public:
    Client(int fd, struct sockaddr_in addr);
    ~Client();
    int proxyConnect(char *host_ip, int proclnt_port, int port = HTTPD_PORT); //ProClient���ӵ�������վ
    int getSocket();                                                          //����socket������
    char *getClientIP();                                                      //����IP��ַ��ûɶ��Ŀǰ
    ProxyClient *getProxyClient();                                            //���ض�Ӧ��ProxyClient����
    struct sockaddr_in clnt_addr; //�����socket��ַ
};

#endif
