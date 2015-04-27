#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>

#ifndef _SERVER_
#define _SERVER_

int getconf(char *,short *);
void ser_bind(int);
void add_arr(int *,int);
void signal_usr(int);

extern pthread_mutex_t name_list_mutex;
extern pthread_mutex_t database_mutex;
extern char DataBasePath[256];
extern char sleep_time[12];
extern int transducer_arr[1024];
extern int data_recv_flag;
#endif

