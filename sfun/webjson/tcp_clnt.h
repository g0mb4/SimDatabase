#ifndef __TCP_CLNT_H__
#define __TCP_CLNT_H__

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "dconsole.h"
#include "str_buf.h"

int tcp_connect(const char * addr, int port);
void tcp_disconnect(void);

int tcp_get_data(str_buf_t * buf, const char * start_date,
                 const char * start_time, const char * end_date,
                 const char * end_time, const char * sensors);

#endif
