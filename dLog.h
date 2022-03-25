#ifndef _DLOG_H_
#define _DLOG_H_

#define logFilePath "./proxy.log"

#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
using namespace std;

int getCurrTime(char* outTimeStr);
int wLog(char* text);


#endif
