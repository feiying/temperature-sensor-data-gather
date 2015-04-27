#include "server.h"
#include "link.h"

#define MAXLINE 2048
#define DEBUG

char DataBasePath[256];
char sleep_time[12];
int transducer_arr[1024];
int data_recv_flag = 0;
sensor_link wait_handler;
//pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t database_mutex = PTHREAD_MUTEX_INITIALIZER;


/* the handler to deal with the connect from client */
void *tfn(void* arg)
{
	char buf[256];
	int connfd = *(int *)arg;
	int n;
	struct rx_info recv_data;

	/* add the connect descriptor to a array */
//	add_arr(connfd);

	/* receive the data from client node */
	n =	read(connfd,buf,sizeof(buf)-1);
	if(n <= 0)
	{
//		del_arr(connfd);
		return NULL;
	}
	buf[n] = '\0';
	printf("%s\n",buf);

	/* parse the xml form data from the client node */
	parse_xml(buf,&recv_data);

#ifdef QTGUI
	/* add the buf to list(the list will be used by QT application)  */
	insert_node(wait_handler,recv_data);
	/* tell the qt app the number of the data wait to deal with */
	data_recv_flag++;
#endif

#ifdef DEBUG
	printf("writing\n");
#endif

	/* write the data to the database */
	write_database(recv_data);

#ifdef DEBUG
	printf("write ok\n");
#endif

	write(connfd,sleep_time,strlen(sleep_time));
	close(connfd);
	return NULL;
}


/* the handler to setup a query GUI */
#ifdef QT_GUI
void *tfn_qt(void *arg)
{
	qt_handeler();

	return NULL;
}
#endif 


int main(void)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;
    int  connfd,listenfd;
    char str[INET_ADDRSTRLEN];
    int err;
    pthread_t tid;
	fd_set fd;

	/* setup a socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
	/* bind the socket to local IP and port */
    ser_bind(listenfd);
    listen(listenfd, 30);

	/* handler the signal */
    signal(SIGPIPE,SIG_IGN);
	signal(SIGUSR1,signal_usr);

	/* init wait_handler list */
	wait_handler = init_list();

	/* init the freq of the node send data */
	freq_init();
#ifdef DEBUG
	printf("sleep_time:%s\n",sleep_time);
#endif

	/* creat a thread to deal with GUI */
#ifdef QT_GUI
    err = pthread_create(&tid,NULL,tfn_qt,NULL);
	if(err != 0){
		fprintf(stderr,"can't create thread:%s\n",strerror(err));
		return -1;
	}
#endif

	printf("Accepting connections...\n");
	while(1){
		FD_ZERO(&fd);
		FD_SET(listenfd,&fd);
		if(select(listenfd + 1, &fd, NULL,NULL,NULL) < 0)
		{
			continue;
		}

		/* there is a connect arrival */
		if(FD_ISSET(listenfd,&fd))
		{
			cliaddr_len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

			/* creat a thread to handler the connect infomation */
			err = pthread_create(&tid,NULL,tfn,&connfd);
			if(err != 0){
				fprintf(stderr,"can't create thread:%s\n",strerror(err));
				return -1;
			}
		}
	}

	return 0;
}
