#ifndef __LOG_UTIL_H__
#define __LOG_UTIL_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

enum {
LOG_MIN,
LOG_DEBUG,
LOG_INFO,
LOG_ERROR,
LOG_MAX
};

extern void log_init(char* file);
extern void log_fini();

extern void debug(const char* fmt, ...);
extern void info(const char* fmt, ...);
extern void error(const char* fmt, ...);

#endif
