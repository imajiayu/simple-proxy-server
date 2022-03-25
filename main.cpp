#include <getopt.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.h"
#include "ProxyServer.h"
#include "dLog.h"

// int dameon(int nochdir, int noclose)
using namespace std;

int main(int argc, char *argv[])
{
  // daemon(0,0);
  daemon(1, 1); //�Ȳ��������̨
  //Ҫ������
  // myport:����˿ں�
  //port :Զ�������˿ں�
  // ip:������վ��IP��ַ
  int myport = -1;
  int port = -1;
  char ip[IP_LEN] = {0};
  cout << "pid=" << getpid() << endl;
  /***********************************�������******************************************/
  int c;
  while (1)
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {{"port", required_argument, 0, 'p'},
                                           {"ip", required_argument, 0, 'i'},
                                           {"myport", required_argument, 0, 'm'},
                                           {0, 0, 0, 0}};
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
    {
      break;
    }
    switch (c)
    {
    case 'p':
      port = atoi(optarg);
      break;
    case 'i':
      strcpy(ip, optarg);
      break;
    case 'm':
      myport = atoi(optarg);
      break;
    default:
      cerr << "��������" << endl;
      exit(1);
    }
  }
  if (port == -1 || ip == nullptr || myport == -1)
  {
    cerr << "ȱ�ٲ���" << endl;
    return -1;
  }
  /***********************************************************************************/
  cout << "Create ProxyServer..." << endl;
  ProxyServer *serv = new ProxyServer(myport, ip, port); //�������������
  cout << "ProxyServer create successfully,port=" << myport << ",remote ip=" << ip << endl;
  char logText[500];
  sprintf(logText, "ProxyServer create successfully,port=%d,remote ip=%s", myport, ip);
  wLog(logText);
  serv->run();
  delete serv;
  return 0;
}
