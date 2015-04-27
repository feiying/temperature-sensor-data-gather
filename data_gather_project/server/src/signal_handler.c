#include "server.h"
#define DEBUG

/* handler the SIGUSR1 signal */
void signal_usr(int signo)
{
	FILE *fp;
	char buf[12];

	if(signo == SIGUSR1)
	{
		fp = fopen("./config/sleep.conf","r");

		if(fp == NULL)
		{
			printf("failed open ./config/sleep.conf\n");
			return;
		}

		fgets(buf,sizeof(buf)-1,fp);
		buf[strlen(buf)-1] = '\0';

#ifdef DEBUG
		printf("sleep_time:%s\n",buf);
#endif

		strcpy(sleep_time,buf);
		
		fclose(fp);
	}

	return;
}
