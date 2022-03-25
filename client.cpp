#include "client.h"

Client::Client(int fd, struct sockaddr_in addr)
{
    clnt_sock = fd;   //已有的浏览器端socket描述符
    clnt_addr = addr; //对应的地址
    p_clnt = nullptr; //先置nullptr
}

Client::~Client()
{
    //如果不是浏览器访问，那么应该要释放clnt_sock，这里只释放p_clnt的socket描述符
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
    { //连接失败
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
        cerr << "fatal error when getProxyClient" << endl; //不存在的。一般都申请了地址
    }
    return p_clnt;
}