#ifndef __CONF_UTIL_H
#define __CONF_UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heartbeat.h"

extern int read_conf(const char *filename, const char ** key, int cnt, struct hb_conf *conf);
extern void write_conf(const char *filename, const struct hb_conf *conf);

#endif
