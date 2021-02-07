#include "log_util.h" 

#define MAXLEN 1024
#define POLL_WAIT 500

static pthread_mutex_t lu_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *logfp=NULL;
static int log_level=0;
static char* str_log_level[] = {
	"  min",
	"debug",
	" info",
	"error",
	"  max"
};

static int can_write_now()
{
	struct pollfd ufds;
	int write_timeout = POLL_WAIT;
	int rc;
	char temp[2];

	ufds.fd     = fileno(logfp);
	ufds.events = POLLOUT;
	while ((rc = poll(&ufds, 1, write_timeout)) < 0) {
		switch (errno) {
		case EINTR:
		case EAGAIN:
			continue;
		default:
			return -1;
		}
	}
	if (rc == 0)
		return 0;

	return 1;
}


static void log_file(int log_level, const char* fmt, va_list args)
{
	if(logfp)
	{
		if(!can_write_now())
		  return;

	   char log_fmt[MAXLEN] = "";
	   sprintf(log_fmt, "[%s] %s", str_log_level[log_level], fmt);
	   vfprintf(logfp, log_fmt, args);
	   fflush(logfp);
	}
}

void log_init(char* file)
{
	if(logfp == NULL)
	{
		logfp = fopen(file, "a+");
		if(logfp)
		{
			info("logfp init sucess...\n");
		}
	}
}

void log_fini()
{
	if(logfp)
	{
		fclose(logfp);
	}
}

void debug(const char* fmt, ...)
{
	va_list vlist;
	va_start(vlist,fmt);
	pthread_mutex_lock(&lu_mutex);
	log_file(LOG_DEBUG, fmt,vlist);
	pthread_mutex_unlock(&lu_mutex);
	va_end(vlist);
}

void info(const char* fmt, ...)
{
	va_list vlist;
	va_start(vlist,fmt);
	pthread_mutex_lock(&lu_mutex);
	log_file(LOG_INFO, fmt,vlist);
	pthread_mutex_unlock(&lu_mutex);
	va_end(vlist);
}

void error(const char* fmt, ...)
{
	va_list vlist;
	va_start(vlist,fmt);
	pthread_mutex_lock(&lu_mutex);
	log_file(LOG_ERROR, fmt,vlist);
	pthread_mutex_unlock(&lu_mutex);
	va_end(vlist);
}

