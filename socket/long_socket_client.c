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
 
#define IP "10.6.6.162"
#define PORT 8080
long time_last_connected = 0;

/*
参数解释
fd:       网络连接描述符
start:    首次心跳侦测包发送之间的空闲时间
interval: 两次心跳侦测包之间的间隔时间
count:    探测次数，即将几次探测失败判定为TCP断开
*/
int set_tcp_keepAlive(int fd, int start, int interval, int count) 
{
	int keepAlive = 1;
	//入口参数检查 ，编程的好习惯。
	if (fd < 0 || start < 0 || interval < 0 || count < 0)
	  return -1;   
	//启用心跳机制，如果您想关闭，keepAlive置零即可
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*) &keepAlive, sizeof(keepAlive)) == -1) 
	{
		perror("setsockopt");
		return -1;
	}
	/*
	//启用心跳机制开始到首次心跳侦测包发送之间的空闲时间
	if (setsockopt(fd, IPPROTO_TCP , TCP_KEEPIDLE, (void *) &start, sizeof(start)) == -1) 
	{
		perror("setsockopt");
		return -1;
	}
	//两次心跳侦测包之间的间隔时间
	if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *) &interval, sizeof(interval)) == -1) 
	{
		perror("setsockopt");
		return -1;
	}
	//探测次数，即将几次探测失败判定为TCP断开
	if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void *) &count, sizeof(count)) == -1) 
	{
		perror("setsockopt");
		return -1;
	}
	*/
	return 0;
}
/**
 *read_timeout - 读超时检测函数, 不包含读操作
 *@fd: 文件描述符
 *@waitSec: 等待超时秒数, 0表示不检测超时
 *成功(未超时)返回0, 失败返回-1, 超时返回-1 并且 errno = ETIMEDOUT
 **/
int read_timeout(int fd, long waitSec) 
{
    int returnValue = 0;
    if (waitSec > 0) 
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd, &readSet);    //添加
 
        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;       //将微秒设置为0(不进行设置),如果设置了,时间会更加精确
        do
        {
            returnValue = select(fd + 1, &readSet, NULL, NULL, &waitTime);
        }
        while (returnValue < 0 && errno == EINTR);   //等待被(信号)打断的情况, 重启select
 
        if (returnValue == 0) 
        {   //在waitTime时间段中一个事件也没到达，超时
            returnValue = -1;   //返回-1
            errno = ETIMEDOUT;
        }
        else if (returnValue == 1) 
        {  //在waitTime时间段中有事件产生
            returnValue = 0; //返回0,表示成功
        }
    // 如果(returnValue == -1) 并且 (errno != EINTR), 则直接返回-1(returnValue)
    }
    printf("TCP read_timeout:%d\n",returnValue);
    return returnValue;
}
// /* activate_nonblock - 设置IO为非阻塞模式
//  * fd: 文件描述符
//  */
// void activate_nonblock(int fd)
// {
//     int ret;
//     int flags = fcntl(fd, F_GETFL);
//     if (flags == -1)
//     {
//         perror("fcntl error");
//     }
//     flags |= O_NONBLOCK;
//     ret = fcntl(fd, F_SETFL, flags);
//     if (ret == -1)
//     {
//         perror("fcntl error");
//     }
// }
// /* deactivate_nonblock - 设置IO为阻塞模式
//  * fd: 文件描述符
//  */
// void deactivate_nonblock(int fd)
// {
//     int ret;
//     int flags = fcntl(fd, F_GETFL);
//     if (flags == -1)
//     {
//         perror("fcntl error");
//     }
//     flags &= ~O_NONBLOCK;
//     ret = fcntl(fd, F_SETFL, flags);
//     if (ret == -1)
//     {
//         perror("fcntl error");
//     }
// }
 
/* connect_timeout - 带超时的connect
 * fd: 套接字
 * addr: 输出参数，返回对方地址
 * wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
// int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
// {
//     int ret;
//     socklen_t addrlen = sizeof(struct sockaddr_in);
 
//     if (wait_seconds > 0)
//     {
//         activate_nonblock(fd);          //设置为非阻塞模式
//     }
//     printf("\n连接服务器开始\n");
//     ret = connect(fd, (struct sockaddr *) addr, addrlen);
//     printf("\n连接服务器开始[%d,%d]\n", ret,errno);
//     if (ret < 0 && errno == EINPROGRESS)
//     {
//         printf("connect EINPROGRESS\n");   //异步连接
//         fd_set connect_fdset;
//         struct timeval timeout;
//         FD_ZERO(&connect_fdset);
//         FD_SET(fd, &connect_fdset);
 
//         timeout.tv_sec = wait_seconds;
//         timeout.tv_usec = 0;
 
//         do
//         {
//         /* 一旦连接建立，套接字就可写 */
//             ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
//         }
//         while (ret < 0 && errno == EINTR);
 
//         if (ret == 0)
//         {
//             errno = ETIMEDOUT;
//             return -1;
//         }
//         else if (ret < 0)
//         return -1;
 
//         else if (ret == 1)
//         {
//       /* ret返回为1，可能有两种情况，一种是连接建立成功，一种是套接字产生错误
//        * 此时错误信息不会保存至errno变量中（select没出错）,因此，需要调用
//        * getsockopt来获取 */
//             int err;
//             socklen_t socklen = sizeof(err);
//             int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
//             if (sockoptret == -1)
//             {
//                 return -1;
//             }
//             if (err == 0)
//             {
//                 ret = 0;
//             }
//             else
//             {
//                 errno = err;
//                 ret = -1;
//             }
//         }
//     }
//     if (wait_seconds > 0)
//     {
//         deactivate_nonblock(fd);
//     }
//     return ret;
// }
int tcp()
{
    printf("\n\n**********TCP开始连接%s:%d\n", IP, PORT);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("socket创建失败：%d\n", sockfd);
        close(sockfd);
        sleep(1);
        return -1;
    }
    // printf("socket创建成功：%d\n", sockfd);
    struct sockaddr_in ser;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(PORT);
    ser.sin_addr.s_addr = inet_addr(IP);
    int res = connect(sockfd, (struct sockaddr *)&ser, sizeof(ser));
    if (res < 0)
    {
        printf("socket连接失败：%d\n", res);
        close(sockfd);
        sleep(1);
        return -1;
    }
    printf("socket连接成功：%d\n", res);
	//set_tcp_keepAlive(sockfd, 10, 3, 3);
    while (1)
    {
        //发送
        //printf("please input:");
	    //fflush(stdin);//清理标准输入流，把多余的未被保存的数据丢掉。
	
        char buff[128] = "1234";
        //fgets(buff, 128, stdin);
        //buff[strlen(buff) - 1] = 0;
        if (strcmp(buff, "end") == 0) //end正常退出
        {
            close(sockfd);
            printf("end正常退出socket\n");
            return 0;
        }
        int sendret = send(sockfd, buff, strlen(buff), 0);
        if (sendret < 0)
        {
            printf("socket发送失败：%d (0服务端断开,-1网络故障)\n", sendret);
            close(sockfd);
            return -1;
        }
        printf("socket发送成功：%d,内容：%s\n", sendret, buff);
 
        //接收
        char recvbuff[1024] = {0};
        printf("socket接收等待中\n");
	    fflush(stdout);//清空输出缓冲区，并把缓冲区内容输出。
        int ret = read_timeout(sockfd, 10);
        if(ret==0)
        {
            int recvret = recv(sockfd, recvbuff, 1023, 0);
            if (recvret <= 0)
            {
                printf("socket接收失败：%d (0服务端断开,-1网络故障)\n", recvret);
                close(sockfd);
                return -1;
            }
            printf("socket接收成功：%d,内容：%s\n", recvret, recvbuff);
        }
		sleep(10);
    }
}
int main()
{
    while (1)
    {
        tcp();
		break;
    }
}
