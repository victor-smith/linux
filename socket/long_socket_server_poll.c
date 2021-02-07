#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct hb_conf{
	char *primary_addr;//主机的地址
	char *backup_addr;//备机的地址
	int hb_port;//监听端口
	int hb_timeout;//超时时间
	int try_times;//尝试次数
	char *welcome;//欢迎信息
};


static int str_get_addr(const char * ip)
{
	struct in_addr addr;

	if(inet_aton(ip,&addr))
	  return addr.s_addr;
	return 0;
}
/*
void* do_listen_conn(void *va)
{
	int confd = (int)*((int*)va);
	int n;
	char msg[20];	
	char ret[20]="ok";	

	while(1)
	{
		if( (n=read(confd, msg, sizeof(msg))) == 0){
			printf("server read error");
			break;
		}
		printf("%s:%d server read: %s\n",__func__, __LINE__,  msg);
		write(confd, ret, strlen(ret));
	}
	close(confd);
}
*/

int find_right_index(int arr[], int n)
{
	int ret=-1, i;

	for(i=0; i<n; i++)
	{
		if(arr[i] == 0)
		{
			ret = i;
			break;
		}
	}
	return ret;
}
 
void* listen_work(void *conf)
{
	struct hb_conf *cf = (struct hb_conf *)conf;
	char *ip = cf->primary_addr;
	int port = cf->hb_port;
	int sockfd, connfd, nip=str_get_addr(ip);
	int clilen;
	struct sockaddr_in servaddr, cliaddr;
	ssize_t n;

#ifdef DEBUG
	printf("ip(%s), port(%d)\n", ip, port);
#endif
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = nip==0?INADDR_ANY:nip;
	
	int opt = 1;
	setsockopt(sockfd ,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof( opt ));

	bind(sockfd,(const struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(sockfd, 100000);

	struct pollfd pfds[100000];
	int  maxindex, timeoutms=5000;
	int  closecnt=0, connectcnt=0;
	int confd, nready, nread, i;

	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;
	maxindex = 0;

	char recvmsg[1024];	
	char respmsg[20]="ok";	

	struct rlimit limit;
	limit.rlim_cur = 100000;
	limit.rlim_max = 100000;

	setrlimit(RLIMIT_NOFILE, &limit);


	for(;;)
	{
		nready = poll(pfds, maxindex+1, timeoutms);
		//printf("poll return %d\n", nready);
		for(i=0; i<=maxindex && nready > 0; i++)
		{
			if(pfds[i].fd == sockfd && pfds[i].revents & POLLIN)
			{
				clilen = sizeof(cliaddr);
				if((connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen))>0)
				{
					/*
					   if(errno == EINTR)
					   continue;
					   else
					   printf("%s:%d accept failed\n", __func__, __LINE__);
					   */
					if(maxindex < sizeof(pfds)-1)
					{
						maxindex++;
						pfds[maxindex].fd = connfd;
						pfds[maxindex].events = POLLIN|POLLERR;
					}
					//printf("accect one socket %d\n", connfd);
					connectcnt++;
				}
			}
			else if(pfds[i].revents & POLLIN)
			{
				if( (nread=read(pfds[i].fd, recvmsg, sizeof(recvmsg))) <= 0){
					//printf("fd %d client socket was closed\n", pfds[i].fd);
					close(pfds[i].fd);
					if(maxindex != i)
					{
						pfds[i].fd = pfds[maxindex].fd;
						pfds[i].events = pfds[maxindex].events;
						pfds[i].revents = pfds[maxindex].revents;
					}
					maxindex--;
					i--;
					closecnt++;
				}
				else
				{
					time_t now = time(NULL);
					recvmsg[nread] = '\0';

					if(strncmp(recvmsg, "bye", strlen("bye")) == 0)
					{
						close(pfds[i].fd);
						
						if(maxindex != i)
						{
							pfds[i].fd = pfds[maxindex].fd;
							pfds[i].events = pfds[maxindex].events;
							pfds[i].revents = pfds[maxindex].revents;
						}

						maxindex--;
						i--;
					}
					else
						write(pfds[i].fd, respmsg, strlen(respmsg));
					printf("%s %d server read: %s\n", ctime(&now), pfds[i].fd,  recvmsg);
				}
			}
			else if(pfds[i].fd != sockfd && (pfds[i].revents & POLLERR))
			{
				close(pfds[i].fd);
				if(maxindex != i)
				{
					pfds[i].fd = pfds[maxindex].fd;
					pfds[i].events = pfds[maxindex].events;
					pfds[i].revents = pfds[maxindex].revents;
				}
				maxindex--;
				i--;
				closecnt++;
			}
		}
		printf("maxindex is %d, closecnt %d, connectcnt %d\n", maxindex, closecnt, connectcnt);
	}
}
 
int main(int argc, char** argv)
{
	struct hb_conf conf = {
		.primary_addr = "10.6.6.162",
		.backup_addr = "",
		.hb_port = 8080,
		.hb_timeout = 10,
		.try_times = 3,
		.welcome = ""
	};
	while(1)
	  listen_work((void*)&conf);
	return 0;
}
