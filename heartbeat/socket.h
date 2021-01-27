#ifndef __SOCKET_H
#define __SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "heartbeat.h"

/*
 * IN ip, server ip
 * IN port, server port
 * */
void* listen_work(void* conf);

/*
 * IN ip, server ip
 * IN port, server port
 * OUT ret, 1 ok, 0 not ok
 * */
void ping_work(const char * ip, int port, int *ret);

/*
 * get local ip地址
 */
int socket_is_primary(const char *ip);

#endif
