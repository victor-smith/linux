#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "heartbeat.h"

#define LOGFILE "/root/code/heartbeat/hblog.log"
pthread_t main_thread = NULL;
static char *conf_file = "hb.conf";
static int exit_main = 0;

char* hb_conf_key[] = 
{
	"primary_addr", 
	"backup_addr",
	"hb_port",
	"hb_timeout",
	"try_times",
	"welcome"
};

static void usage()
{
	printf("%s [-d] [-f <filename>]\n");
}

static char flag_work = 1;

void deal_sig(int sig)
{
#ifdef DEBUG
	printf("rcv sig %d %s\n", sig, strsignal(sig));
#endif 
	info("rcv sig %d %s\n", sig, strsignal(sig));
	if(main_thread)
		pthread_cancel(main_thread);
	exit_main = 1;	
}

void write_pid2file(pid_t pid)
{
#ifdef DEBUG
	printf("process id =%d\n", pid);
#endif
	info("process id =%d\n", pid);
}

// 主机监听备用机
pthread_t do_work_main( const struct hb_conf *conf)
{
	pthread_t pd;
	pthread_create(&pd, NULL, listen_work,(void*)conf);
	pthread_detach(pd);
	return pd;
}

// 备用机的工作，ping 主机
int do_work_vice( const struct hb_conf *conf)
{
	int ret = 1;
	int try_times = conf->try_times;
	while(ret || try_times)
	{
		sleep(5);
		ping_work(conf->primary_addr, conf->hb_port,&ret);
		if(!ret)
		{
#ifdef DEBUG
			printf("did not recive main controller responses... try times %d\n", try_times);
#endif
			info("did not recive main controller responses... try times %d\n", try_times);
			try_times--;
		}
	}
	return ret;
}

//设置工作状态
static void set_work_mode(char *flag_work, struct hb_conf *conf)
{
	if( socket_is_primary(conf->primary_addr) )
		*flag_work = 1;
	else
		*flag_work = 0; 
} 
		
int main(int argc, char **argv) { 
	int opt;
	struct hb_conf conf;
	pthread_t main_tid;
	pid_t pid = getpid();
	write_pid2file(pid);
	log_init(LOGFILE);

	signal(SIGINT,deal_sig);
	signal(SIGKILL,deal_sig);

	while((opt=getopt(argc, argv,"f:d"))!=-1)
	{
		switch(opt)
		{
			case 'f':
				conf_file = optarg;
				break;
			case 'd':
#ifdef DEBUG
				printf("program background running\n");
#endif
				info("program background running\n");
				break;
			default:
				usage();
		}
	}
	//1. 读配置文件
	int flag = read_conf(conf_file, hb_conf_key, 6, &conf);
	if(flag == 1)
	{
#ifdef DEBUG
		printf("请检查配置文件\n");
		exit(1);
#endif
		error("read %s configuration failed!\n", conf_file);
	}
#ifdef DEBUG
	printf("conf info: %s,%s,%d,%d,%d,%s\n",
				conf.primary_addr,
				conf.backup_addr,
				conf.hb_port,
				conf.hb_timeout,
				conf.try_times,
				conf.welcome);
#endif
	info("conf info: %s,%s,%d,%d,%d,%s\n",
				conf.primary_addr,
				conf.backup_addr,
				conf.hb_port,
				conf.hb_timeout,
				conf.try_times,
				conf.welcome);
	//2. 启动模式（主or备）
	set_work_mode(&flag_work, &conf);
	//2.1 主 接受备用机器的ping并作回复，接收到信号后停止socket
	if(flag_work)
	{
		main_tid = do_work_main(&conf);
	}
	//2.2 备 ping 主，无回应后接管服务。
	else
	{
		if(!do_work_vice(&conf))
		{
#ifdef DEBUG
		  printf("vice controller to work as main...\n");
#endif
		  info("vice controller to work as main...\n");
		  flag_work = 1;
		}
	}
	//3. while 没做啥实际工作
	while(1)
	{
		sleep(10);
#ifdef DEBUG
		printf("do while work, working in main? %s\n", flag_work?"Yes":"No");
#endif
		info("do while work, working in main? %s\n", flag_work?"Yes":"No");

		if(exit_main)
			break;
	}
	log_fini();

	return 0;
}
