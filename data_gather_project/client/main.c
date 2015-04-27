#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define DEBUG

#define filename "/dev/nick_adc"
#define MAX_BUF	128
#define NUM		10

#define SET_UART	1
#define GET_UART	2


int main (void)
{
	int fd,n,i;
	int file_num;
	char buf[MAX_BUF] = {0};
	int test_num;

	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		printf("fail to open %s\n", filename);
		return -1;
	}
	for (test_num = NUM; test_num; test_num--)
	{
		n = read(fd, buf, MAX_BUF);
		printf("n = %d\n",n);
		if (n<=0)
		{
			printf("fail to read from %s\n", filename);
		}
		else
		{
			printf("[");
			for (i=0; i<n; i++)
				printf("%x",buf[i]);
			printf("]\n");
		}
		sleep(1);
	}
/*	
	for (test_num = NUM; test_num; test_num--)
	{
		bzero(buf, MAX_BUF);
		printf("test starting please input string:\n");
		fgets(buf, MAX_BUF-1, stdin);
		n = strlen(buf);
		n = write(fd, buf, n);
		if (n <= 0)
			printf("fail to write %s\n",filename);
	}
*/
	return 0;
}
