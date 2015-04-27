#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define DATA_LEN 20 
#define BUF_LENGTH 1024
#define IP_LEN 20
#define NAME_LEN 20
#define MAXSIZE 30
#define DEF_SLEEP	5
#define MAXLINE	10

#define DEBUG
#define file_name "/dev/nick_adc"

static const char *config = "config/config.txt";

int getconf(const char *pathname,char *name,char * ip,short *port);

int main(int argc, char *argv[])
{
	int sleep_time = DEF_SLEEP;
	int fd, count,n;
	char vol[DATA_LEN] = {0};
	char buf[MAXLINE] = {0};
	struct sockaddr_in servaddr;
	int sockfd;
	char *str;
	short port;
	char ip[IP_LEN];
	char name[NAME_LEN];
	char xml_buf[BUF_LENGTH];
	float volt_data;
	char strtime[MAXSIZE];


	getconf(config, name, ip, &port);

	fd = open(file_name, O_RDONLY);
	if(fd < 0)
	{
		perror("open /dev/nick_adc");
		exit(1);

	}
	while(1)
	{
		count = read(fd, vol, DATA_LEN);
		if(count < 0)
		{
			perror("read /dev/nick_adc");
			exit(1);
		}
#ifdef DEBUG
		printf("[%x]\n", (*(short *)vol));
#endif
		volt_data = (*(short *)vol)*3.3/1024;
		time_t tm;
		time(&tm);
		struct tm *tm_str;
		tm_str = localtime(&tm);
		//	char *time = ctime(&tm);

		strftime(strtime, MAXSIZE,"%F-%T", tm_str);
		//	vol[count] = '\0';

		sprintf(xml_buf, "<sensor><name>%s</name><vol>%.2f</vol><time>%s</time></sensor>",
				name, volt_data, strtime);

#ifdef DEBUG
		printf("[%s]\n", xml_buf);
#endif
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton(AF_INET, ip, &servaddr.sin_addr);
		servaddr.sin_port = htons(port);

		if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
			goto sleep_sub;

		write(sockfd, xml_buf, strlen(xml_buf));

		bzero(buf, sizeof(buf));
		if ((n = read(sockfd, buf, MAXLINE)) <=0 )
			goto sleep_sub;

		/* read from server get sleep time*/
		if ((sleep_time = atoi(buf)) <= 0)
			sleep_time = DEF_SLEEP;
#ifdef DEBUG		
		printf("Response from server [%d]\n", sleep_time);
#endif

		close(sockfd);
sleep_sub:
		sleep(sleep_time);
	}
	return 0;
}

int getconf(const char *pathname,char *name,char * ip,short *port)
{
	FILE *file;
	char buffer[BUF_LENGTH];
	char *p;

	file = fopen(pathname,"r");
	if(file == NULL){
		perror("open file");
		return -1;
	}

	while(fgets(buffer,BUF_LENGTH,file)){
		buffer[strlen(buffer)-1]='\0';
		p = strchr(buffer,'=');
		if(p == NULL)
			return -1;
		*p = '\0';
		if(!strcmp(buffer,"port"))
			*port = (short)atoi(p+1);
		else if(!strcmp(buffer,"ip"))
			strcpy(ip,p+1);
		else if(!strcmp(buffer, "name"))
		{
			strcpy(name, p+1);
		}
	}

	fclose(file);

	return 0;
}

