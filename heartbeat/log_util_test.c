#include "log_util.h"
#include <pthread.h>
#include <stdlib.h>

void* thread_worker(void *vi)
{
	int i = *(int *)vi;	
	debug("thread:%d, %c %d %s\n", i, 'c', 12, "hello world");
	info("thread:%d, %c %d %s\n", i, 'c', 12, "hello world");
	error("thread:%d, %c %d %s\n", i, 'c', 12, "hello world");
}

void thread_log_test(int nthread)
{
	pthread_t parr[1000000];
	int i;
	for( i=0; i<nthread; i++)
	{
		pthread_create(&parr[i], NULL, thread_worker, (void*)&i);
	}

	for( i=0; i<nthread; i++)
	{
		pthread_join(parr[i], NULL);
	}
}

int main(int argc, char ** argv)
{

	if(argc < 2)
		exit(-1);

	log_init("log.file");
	{
		//debug("%c %d %s\n", 'c', 12, "hello world");
		//info("%c %d %s\n", 'c', 12, "hello world");
		//error("%c %d %s\n", 'c', 12, "hello world");
	}
	thread_log_test(atoi(argv[1]));

	log_fini();

	return 0;
}
