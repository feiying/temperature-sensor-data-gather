#include<syslog.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<signal.h>

void daemonize(const char *);
int main(void){
#ifdef ARM_DAEMON
	char *cmd = "/server/config/sleep.conf"
#else
	char *cmd = "/home/akaedu/zhbo/netgather/config/sleep.conf";
#endif

	daemonize(cmd);

	return 0;
}
void daemonize(const char *cmd)
{
	int  i,fd0,fd1,fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;
	int fd;
	time_t mtime = 0;
	struct stat buf;

	umask(0);

	if(getrlimit(RLIMIT_NOFILE, &rl) < 0){
		printf("%s : can't get file limit", cmd);
		exit(1);
	}

	if((pid = fork()) < 0){
		printf("%s : can't fork",cmd);
		exit(1);
	}
	else if(pid > 0)
		exit(0);
	setsid();

	signal(SIGHUP,SIG_IGN);
	if((pid = fork()) < 0){
		printf("%s:can't fork", cmd);
		exit(1);
	}
	else if(pid > 0)
		exit(0);

	if(chdir("/") < 0){
		printf("%s: can't change directory to /");
		exit(1);
	}

	if(rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for(i = 0; i < rl.rlim_max; i++)
		close(i);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	while(1){
		if((fd = open(cmd,O_RDWR))<0)
			perror("open file");
		fstat(fd,&buf);
		if(mtime != buf.st_mtime)
#ifdef ARM_DAEMON
			system("/server/daemonprocess/refresh.sh");
#else
			system("/home/akaedu/zhbo/netgather/daemonprocess/refresh.sh");
#endif
		sleep(10);
		mtime = buf.st_mtime;
	}
}
