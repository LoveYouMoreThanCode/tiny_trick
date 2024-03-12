#ifndef __LOG_H__
#define __LOG_H__
#include <stdio.h>

#define LOG(fmt, ...) printf("[%s:%d] :" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
