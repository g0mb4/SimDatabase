#ifndef __STR_BUF_H__
#define __STR_BUF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct S_STR_BUF {
	uint32_t len;
	uint32_t cap;
	char * str;
} str_buf_t;

str_buf_t * str_buf_create(void);
void str_buf_destroy(str_buf_t * sb);

int str_buf_add(str_buf_t * sb, const char * s);
int str_buf_addn(str_buf_t * sb, const char * s, uint32_t len);

void str_buf_clear(str_buf_t * sb);
int str_buf_copy(str_buf_t * sb_dest, const str_buf_t * sb_src);

#endif
