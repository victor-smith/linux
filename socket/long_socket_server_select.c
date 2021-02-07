#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>
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

	fd_set curset,allset;
	struct timeval timeout;
	int sfds[1024] = {0}, curindex;
	int nready, i, curmaxfd = sockfd;

	int confd, nread;
	char recvmsg[1024];	
	char respmsg[20]="ok";	



	FD_ZERO(&allset);
	FD_SET(sockfd, &allset);
	sfds[0] = sockfd;
	curindex = 0;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	for(;;)
	{
		curset = allset;
		nready = select(curmaxfd+1, &curset, NULL, NULL, &timeout);
		if(nready > 0)
			printf("sockfd is %d, is in allset?%d\n", sockfd, FD_ISSET(sockfd, &allset));

		if(FD_ISSET(sockfd, &curset))
		{
			nready--;
			clilen = sizeof(cliaddr);
			if((connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen))<0)
			{
				/*
				   if(errno == EINTR)
				   continue;
				   else
				   printf("%s:%d accept failed\n", __func__, __LINE__);
				   */
			}
			else{
				curindex = find_right_index(sfds, 1024);
				if(curindex != -1)
				{
					FD_SET(connfd, &allset);
					if(connfd > curmaxfd)
					  curmaxfd = connfd;
					sfds[curindex] = connfd;
				}
				else
				{
					printf("Woo!!! i am out of resources, all 1024 fd is used\n");
				}
			}
		}

		for(i=1; i< 1024 && nready > 0; i++)
		{
			if(sfds[i] == 0)
			  continue;

		    if(FD_ISSET(sfds[i], &curset))
			{
				nready --;
				if( (nread=read(sfds[i], recvmsg, sizeof(recvmsg))) <= 0){
					printf("fd %d client socket was closed\n", sfds[i]);
					close(sfds[i]);
				    FD_CLR(sfds[i], &allset);	
					sfds[i] = 0;
				}
				else
				{
					time_t now = time(NULL);
					recvmsg[nread] = '\0';

					if(strncmp(recvmsg, "bye", strlen("bye")) == 0)
					{
						close(sfds[i]);
						FD_CLR(sfds[i], &allset);	
						sfds[i] = 0;
					}
					else
						write(sfds[i], respmsg, strlen(respmsg));
					printf("%s %d server read: %s\n",ctime(&now), sfds[i],  recvmsg);
				}
			}
		}
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
