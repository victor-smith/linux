#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
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

	bind(sockfd,(const struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(sockfd, 1024);

	for(;;)
	{
		clilen = sizeof(cliaddr);
		if((connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen))<0)
		{
			if(errno == EINTR)
			  continue;
			else
			  printf("%s:%d accept failed\n", __func__, __LINE__);
		}
		pthread_t pt;
		pthread_create(&pt, NULL, do_listen_conn, (void*)&connfd);
	    pthread_detach(pt);	
	}
}
 
int main(int argc, char** argv)
{
	struct hb_conf conf = {
		.primary_addr = "127.0.0.1",
		.backup_addr = "",
		.hb_port = 8080,
		.hb_timeout = 10,
		.try_times = 3,
		.welcome = ""
	};

	listen_work((void*)&conf);
	return 0;
}
