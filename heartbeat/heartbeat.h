#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "socket.h"
#include "log_util.h"
//#include "conf_util.h"

enum e_hb
{
	P_ADDR,
	B_ADDR,
	HB_PORT,
	HB_TIMEOUT,
    HB_TRY_TIMES,
	HB_WELCOME
};

extern char* hb_conf_key[];
/*
{
	"primary_addr",
	"backup_addr",
	"hb_port",
	"hb_timeout",
	"try_times",
	"welcome"
};
*/

struct hb_conf{
	char *primary_addr;//主机的地址
	char *backup_addr;//备机的地址
	int hb_port;//监听端口
	int hb_timeout;//超时时间
	int try_times;//尝试次数
	char *welcome;//欢迎信息
};

#endif
