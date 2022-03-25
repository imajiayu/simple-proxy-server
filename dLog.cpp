#include "dLog.h"

int getCurrTime(char* outTimeStr)
{
	time_t tTime;
	struct tm *tmTime;
	struct timeval mTime;
	time(&tTime);
	tmTime=localtime(&tTime);
	gettimeofday( &mTime, NULL );
	sprintf( outTimeStr, "%04d %02d %02d %02d:%02d:%02d",
		tmTime->tm_year + 1900, tmTime->tm_mon + 1,
		tmTime->tm_mday, tmTime->tm_hour,
		tmTime->tm_min, tmTime->tm_sec );
	return 0;
}

int wLog(char* text)
{	
	char Ctime[20];
	getCurrTime(Ctime);
	ofstream out(logFilePath,ios::app);
	if(out.fail())
		cout<<"error"<<endl;
	out<<Ctime<<"	";
	out<<text<<endl;
	out.close();
	return 0;
}
