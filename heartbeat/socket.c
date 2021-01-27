#include "socket.h"

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

	if( (n=read(confd, msg, sizeof(msg))) == 0){
		printf("server read error");
		return;
	}
	printf("%s:%d server read: %s\n",__func__, __LINE__,  msg);
	write(confd, ret, strlen(ret));
	close(confd);
}
/*
 * IN ip, server ip
 * IN port, server port
 * */
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

/*
 * IN ip, server ip
 * IN port, server port
 * OUT ret, ping ok is 1, otherwise 0 
 * */
void ping_work(const char * ip, int port, int *ret)
{

	int sockfd, nip = str_get_addr(ip);
	struct sockaddr_in servaddr;

#ifdef DEBUG
	printf("%s: ip(%s) port(%d) nip(%0x)\n", __func__, ip, port, nip);
#endif

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = nip;	
	if(connect(sockfd,(const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		printf("connect main controller %s failed\n", ip);
		*ret = 0;
		return;
	}

	char sendline[20]="hello";
	char recvline[20];
	int len;
	write(sockfd, sendline, strlen(sendline));
	if((len=read(sockfd, recvline, 1024)) == 0)
	{
		printf("%s:%d read end\n", __func__, __LINE__);
		exit(0);
	}
	else
	{
		recvline[len]='\0';	
		printf("%s:recv %s\n", __func__, recvline);
	}
	close(sockfd);

	if(strncmp(recvline,"ok",2)==0)
	  *ret = 1;
	else 
	  *ret = 0;	
}

int socket_is_primary(const char* ip)
{
	int i=0;
	int sockfd;
	struct ifconf ifc;
	char buf[1024]={0};
	char ipbuf[20]={0};
	struct ifreq *ifr;

	ifc.ifc_len = 1024;
	ifc.ifc_buf = buf;

	if((sockfd = socket(AF_INET, SOCK_DGRAM,0))<0)
	{
		printf("socket error\n");
		return 0;
	}
	ioctl(sockfd,SIOCGIFCONF, &ifc);
	ifr = (struct ifreq*)buf;

	for(i=(ifc.ifc_len/sizeof(struct ifreq)); i > 0; i--)
	{
		printf("net name: %s\n",ifr->ifr_name);
		inet_ntop(AF_INET,&((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr,ipbuf,20);
		printf("ip: %s \n",ipbuf);

		if(strcmp(ip, ipbuf)==0)
		  return 1;		

		ifr = ifr +1;
	}
	return 0;
}
