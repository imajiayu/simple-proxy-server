#include "client.h"

Client::Client(int fd, struct sockaddr_in addr)
{
    clnt_sock = fd;   //���е��������socket������
    clnt_addr = addr; //��Ӧ�ĵ�ַ
    p_clnt = nullptr; //����nullptr
}

Client::~Client()
{
    //���������������ʣ���ôӦ��Ҫ�ͷ�clnt_sock������ֻ�ͷ�p_clnt��socket������
    if (p_clnt != nullptr)
    {
        p_clnt->disconnect();
        delete p_clnt;
    }
}

int Client::proxyConnect(char *host_ip, int proclnt_port, int port)
{
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(host_ip);
    p_clnt = new ProxyClient(proclnt_port, serv_addr);
    p_clnt->initSocket();
    if (p_clnt->clnt_connect() == -1)
    { //����ʧ��
        cerr << "Client: clnt_connect error" << endl;
        return -1;
    }
    return 0;
}

int Client::getSocket()
{
    return clnt_sock;
}

char *Client::getClientIP()
{
    return inet_ntoa(clnt_addr.sin_addr);
}

ProxyClient *Client::getProxyClient()
{
    if (p_clnt == nullptr)
    {
        cerr << "fatal error when getProxyClient" << endl; //�����ڵġ�һ�㶼�����˵�ַ
    }
    return p_clnt;
}