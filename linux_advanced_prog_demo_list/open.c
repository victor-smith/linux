#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fcntl.h>
#include <errno.h>


void open_func(const char* fname, int flags, int mode)
{
	int fd1 = open(fname, flags, mode);
	if(fd1 == -1)
	{
		printf("%s: open faild:%s\n", fname, strerror(errno));
		return;
	}
	close(fd1);
}

int main(int argc, char ** argv)
{
	int flags, mode;
	//不存在就创建，存在就报错
	open_func("test1.txt", O_CREAT|O_EXCL, S_IRUSR);
	open_func("test1.txt", O_CREAT|O_EXCL, S_IRUSR);
	//不存在就创建，存在就直接打开
	open_func("test2.txt", O_CREAT|O_RDONLY, S_IRUSR);
	open_func("test2.txt", O_CREAT|O_RDONLY, S_IRUSR);

	//不存在就创建，存在就报错
	open_func("test3.txt", O_CREAT|O_RDONLY|O_EXCL, S_IRUSR);
	open_func("test3.txt", O_CREAT|O_RDONLY|O_EXCL, S_IRUSR);

	//不存在就报错，存在就打开
	open_func("test4.txt", O_RDONLY|O_EXCL, S_IRUSR);
	open_func("test4.txt", O_RDONLY|O_EXCL, S_IRUSR);
	
	return 0;
}
