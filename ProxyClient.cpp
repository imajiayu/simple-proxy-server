#include "ProxyClient.h"

ProxyClient::ProxyClient(int port, struct sockaddr_in host_addr)
{
  clnt_port = port;             //�Լ��˿ں�
  serv_addr = host_addr;        //��Ҫ���ӵķ�������ַ
}

ProxyClient::~ProxyClient()
{
  //�ر�socket���������ͷ���Դ
  if(clnt_sock!=INVALID_SOCKET){
    disconnect();
  }
}

int ProxyClient::initSocket()
{
  clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (clnt_sock == -1)
  {
    cerr << "ProxyClient socket error" << endl;
    return 0;
  }
  //��κ����üӣ�����û�����Ȳ����ˣ������˿ں�Ӧ�ò��ܹ��òŶ�
  // resue
  /*int reuse = 1;
  setsockopt(clnt_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  // bind()
 /* struct sockaddr_in clnt_addr;
  clnt_addr.sin_family = AF_INET;
  clnt_addr.sin_port = htons(clnt_port);
  clnt_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(clnt_sock, (sockaddr *)&clnt_addr, sizeof(clnt_addr)) == -1) {
    cout << "ProxyClient: bind error errno=" << errno << " " << strerror(errno)
         << endl;
    return -1;
  }*/
  return clnt_sock;
}

int ProxyClient::clnt_connect()
{
  //cout << "ProxyClient: try to connect to Remote host" << endl;

  if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ==
      -1)
  {
    cerr << "ProxyClient: connect error" << endl;
    close(clnt_sock);
    clnt_sock=INVALID_SOCKET;
    return -1;
  }
  // nonblock
  setNonblock(clnt_sock);

  cout << "ProxyClient: ���ӵ�" << inet_ntoa(serv_addr.sin_addr) << " port=" << ntohs(serv_addr.sin_port) << endl;
  //cout << "ProxyClient: connect successful" << endl;
  running = true;     //������
  return clnt_sock;
}

int ProxyClient::getSocket()
{
  return clnt_sock;
}

bool ProxyClient::isRunning()
{
  return running;
}

void ProxyClient::setNonblock(int sockfd)
{
  int flags;
  if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
  {
    cout << "ProxyClient: fctnl error errno=" << errno << " " << strerror(errno) << endl;
  }
  flags |= O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) < 0)
  {
    cout << "ProxyClient: fctnl error errno=" << errno << " " << strerror(errno) << endl;
  }
}

void ProxyClient::handleServer(char* remote_ip,struct sockaddr_in clnt_addr,int sockfd)
{
  char *readbuf = new char[RECV_BUFSIZE];
  while (1)
  {
    int rval = recv(clnt_sock, readbuf, RECV_BUFSIZE, 0);
    if (rval == 0)
    {
      running = false; //�����Ѿ��Ͽ���
      break;
    }
    else if (rval == -1)
    {
      break; //��Client�˶Ͽ������������ͷ�socket
    }
    else
    {
      int total_sent = 0;
      int left = rval;
      while (total_sent <= rval)
      {
        int wval = send(sockfd, readbuf + total_sent, left, 0);
        if (wval < 0)
        {
          //�ø�ɶ��ɶ������Client�ͷ�
          return ;
        }
        else if (wval == 0)
        {
          break; //ûɶ�ã����Ű�
        }
        total_sent += wval;
        left -= wval;
	
	char logContent[16384];
	memcpy(logContent,readbuf+total_sent-wval,wval);
        char logText[20000];
        sprintf(logText,"send from %s:%d to %s:%d,Number of bytes:%d\n\n%s",remote_ip,HTTPD_PORT,inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port),wval,logContent);
        wLog(logText);
      }
    }
  }
  delete readbuf;
}

void ProxyClient::disconnect()
{
  close(clnt_sock);
  clnt_sock=INVALID_SOCKET;
}
