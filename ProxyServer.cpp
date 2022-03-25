#include "ProxyServer.h"

ProxyServer::ProxyServer(int myport, char *ip, int port)
{
  proxy_port = myport; //����˿ں�
  remote_ip = ip;    //������վip
  remote_port = port;
  clnt_map = new unordered_map<int, Client *>();
}

ProxyServer::~ProxyServer()
{
  delete clnt_map;
  close(epollfd);
  if(serv_sock!=INVALID_SOCKET){
    close(serv_sock);
  }
}

int ProxyServer::initSocket(int port)
{
  // socket()
  serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)
  {
    cerr << strerror(errno) << "ProxyServer: socket error" << endl;
    return -1;
  }

  // resue
  int reuse = 1;
  setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  // nonblock
  int flags;
  if ((flags = fcntl(serv_sock, F_GETFL, 0)) < 0)
  {
    cout << "fctnl error errno=" << errno << " " << strerror(errno) << endl;
    return -1;
  }
  flags |= O_NONBLOCK;
  if (fcntl(serv_sock, F_SETFL, flags) < 0)
  {
    cout << "fctnl error errno=" << errno << " " << strerror(errno) << endl;
    return -1;
  }

  // bind()
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serv_sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
  {
    cout << "ProxyServer: bind error errno=" << errno << " " << strerror(errno)
         << endl;
    return -1;
  }

  // listen()
  if (listen(serv_sock, SOMAXCONN) == -1)
  {
    cout << "ProxyServer: listen error errno=" << errno << " "
         << strerror(errno) << endl;
    return -1;
  }
  return serv_sock;
}

void ProxyServer::closeSocket()
{
  //�Ȱѻ��ڵĿͻ��˹���
  for(auto it =clnt_map->begin();it != clnt_map->end(); it++){
    Client *cl=it->second;      //unodered_map��ɾ��
    disconnect(cl);
  }
  //cout << "ProxyServer: try to close socket " << serv_sock << endl;
  //cout << "ready to close all connections" << endl;
  clnt_map->clear();  //ֻ��գ�������
  close(serv_sock);
  serv_sock=INVALID_SOCKET;
  //cout << "ProxyServer: close sockets successful" << endl;
}

int ProxyServer::serv_accept()
{
  struct sockaddr_in clnt_addr;
  unsigned int clntaddr_size = sizeof(clnt_addr);
  //��������ѧ���˵�����
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clntaddr_size);
  if (clnt_sock == -1)
  {
    cerr << strerror(errno) << "ProxyServer: accept error" << endl;
    exit(-1);
  }
  cout << "����������:" << inet_ntoa(clnt_addr.sin_addr) << " " << ntohs(clnt_addr.sin_port) << endl;
  //char logText[100];
 // sprintf(logText,"connect from:%s:%d",inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));
 // wLog(logText);
  //���÷�����
  setNonblock(clnt_sock);

  cout << "ProxyServer: accept successfully,clnt_sock=" << clnt_sock << endl;
  //ÿ��ѧ�������ӵ������������������һ�������������������վ��tcp����
  //cout << "ProxyServer: create new Client" << endl;
  Client *cl = new Client(clnt_sock, clnt_addr);
  //cout << "ProxyServer: setting ProxyClient..." << endl;
  if (cl->proxyConnect(remote_ip, proxy_port, remote_port) == -1)
  {
    cerr << "ProxyServer: New Client set Proxy error" << strerror(errno) << endl;
    close(clnt_sock);
    delete cl;
    exit(-1);
  }
  //cout << "ProxyClient :getsocket=" << cl->getProxyClient()->getSocket() << endl;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = cl->getProxyClient()->getSocket();
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cl->getProxyClient()->getSocket(), &ev) == -1)
  {
    cerr << "ProxyServer: ProxyClient epoll_ctl(add error)" << endl;
    exit(-1);
  }
  clnt_map->insert({clnt_sock, cl});
  //clnt_map->insert(pair<int, Client*>(clnt_sock,c1));
  //cout << "ProxyServer: ProxyClient set successfully" << endl;
  return clnt_sock;
}

Client *ProxyServer::getClient(int fd)
{
  unordered_map<int, Client *>::const_iterator it;
  it = clnt_map->find(fd);
  if (it == clnt_map->end())
  {
    return nullptr;
  }
  else
  {
    return it->second;
  }
}

void ProxyServer::disconnect(Client *cl)
{
  if (cl == NULL)
  {
    return;
  }
  char logText[500];
  if(cl)
  {
  	sprintf(logText,"disconnect from %s:%d",inet_ntoa(cl->clnt_addr.sin_addr),ntohs(cl->clnt_addr.sin_port));
  	 wLog(logText);
  }
  //�ر����������ѧ���˴���֮���socket��ѧ���˴����������վ֮���socket��ProxyClient���
  close(cl->getSocket());
  //��ôɾ�������е�����
  /*ev.events == EPOLLHUP;
  ev.data.fd = cl->getSocket();
  if (epoll_ctl(epollfd, EPOLL_CTL_DEL, cl->getSocket(), &ev) == -1)
  {
    cerr << "ProxyServer: epoll_ctl(disconnect client) error" << endl;
    exit(-1);
  }
  ev.data.fd = cl->getProxyClient()->getSocket();
  if (epoll_ctl(epollfd, EPOLL_CTL_DEL, cl->getProxyClient()->getSocket(), &ev) == -1)
  {
    cerr << "ProxyServer: epoll_ctl(disconnect proxyclient) error" << endl;
    exit(-1);
  }*/
  clnt_map->erase(cl->getSocket());
  delete cl;
}

Client *ProxyServer::getProxyClientOwner(int fd)
{
  for (auto it = clnt_map->begin(); it != clnt_map->end(); it++)
  {
    if ((it->second != NULL) && (it->second->getProxyClient()->getSocket() == fd))
    {
      return it->second;
    }
  }
  return nullptr;
}

void ProxyServer::handleClient(Client *cl)
{
  char *readbuf = new char[RECV_BUFSIZE];
  while (1) //һ�δ�����̽��������ݽ��ղ�����
  {
    int rval = recv(cl->getSocket(), readbuf, RECV_BUFSIZE, 0);
    if (rval < 0)
    {
      //cout << "some error occured when recv data from Client" << cl->getSocket() << endl;
      //disconnect(cl);
      break;
    }
    else if(rval==0){
      cout << "Client disconnected(handleClient1)��" << cl->getSocket() << endl;
      disconnect(cl);
    }
    else
    {
      ProxyClient *pCl = cl->getProxyClient();
      if (pCl == nullptr)
      {
        cout << "pCl is not existed,fatal error" << endl; //Ӧ��û�����������
      }
      int total_sent = 0;        //�ܷ����ֽ�
      int left = rval;           //ʣ��δ�����ֽ�
      while (total_sent <= rval) //��û������
      {
        int wval = send(pCl->getSocket(), readbuf + total_sent, left, 0);
        if (wval < 0)
        {
          cerr << "Client disconnected(handleClient2)" << endl;
          disconnect(cl);
          break;
        }
        else if (wval == 0)
        {
          break; //������û��
        }
        total_sent += wval;
        left -= wval;
	 char logContent[16384];
 	 memcpy(logContent,readbuf+total_sent-wval,wval);
 	 char logText[20000];
	 sprintf(logText,"send from %s:%d to %s:%d,Number of bytes:%d\n\n%s",inet_ntoa(cl->clnt_addr.sin_addr),ntohs(cl->clnt_addr.sin_port),inet_ntoa(pCl->serv_addr.sin_addr),ntohs(pCl->serv_addr.sin_port),wval,logContent);
	 wLog(logText);
      }
    }
  }
  delete readbuf;
}

void ProxyServer::run()
{
  //���������������server��
  cout << "ProxyServer: init socket..." << endl;
  if (initSocket(proxy_port) == -1)
  {
    cerr << strerror(errno) << "ProxyServer: initSocket error" << endl;
    exit(-1);
  }
  cout << "ProxyServer: init OK" << endl
       << endl;
  //����epoll
  int nfds;
  int clnt_sock;
  epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    cerr << strerror(errno) << "ProxyServer: epoll_create1 error" << endl;
    exit(-1);
  }
  ev.events = EPOLLIN;
  ev.data.fd = serv_sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serv_sock, &ev) == -1)
  {
    cerr << strerror(errno) << "ProxyServer: epoll_ctl(serv_sock) error" << endl;
    exit(-1);
  }
  //cout << "ProxyServer: epoll ready" << endl;

  //��ʼ���
  while (1)
  {
    cout << "ProxyServer: begin epoll_wait..." << endl;
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
      cerr << strerror(errno) << "ProxyServer: epoll_wait error" << endl;
      exit(-1);
    }
    for (int n = 0; n < nfds; n++)
    {
      if (events[n].data.fd == serv_sock)
      { //����������
        //cout << "ProxyServer: accept connection..." << endl;
        clnt_sock = serv_accept();
        //cout << "ProxyServer: accept successfully" << endl;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = clnt_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clnt_sock, &ev) == -1)
        {
          cerr << strerror(errno) << "ProxyServer: epoll_ctl(clnt_sock) error" << strerror(errno) << endl;
          exit(-1);
        }
      }
      else
      {
        cout << "ProxyServer: handle transactions..." << endl;
        Client *cl = getClient(events[n].data.fd);
        if (cl != nullptr)
        {
          //����ͻ��˷���
          handleClient(cl);
        }
        else
        { 
          //����ProxyClient����
          Client *cl = getProxyClientOwner(events[n].data.fd);
          if (cl == NULL)
          {
            cerr<<"unexcepted error when handle ProxyClient transaction"<<endl;
            continue;     //��һ˵һ�������ܳ���
          }
          ProxyClient *pCl = cl->getProxyClient();
          pCl->handleServer(remote_ip,cl->clnt_addr,cl->getSocket());
          if(pCl->isRunning()==false){        //�Ѿ������ˣ��ͷ�
            disconnect(cl);
          }
        }
      }
    }
  }
  closeSocket();
}

void ProxyServer::setNonblock(int sockfd)
{
  // nonblock
  int flags;
  if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
  {
    cout << "ProxyServer: fctnl error errno=" << errno << " " << strerror(errno)
         << endl;
    exit(-1);
  }

  flags |= O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) < 0)
  {
    cout << "ProxyServer: fctnl error errno=" << errno << " " << strerror(errno)
         << endl;
    exit(-1);
  }
}
