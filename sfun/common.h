#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
    #define strdup  _strdup
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>	// - 'false': macro redefinition

#include <string.h>
#include <time.h>

#define ABS( x ) ( x = x < 0 ? x * -1 : x )

int str_count_char(const char * str, char ch);
void str_trim(char * out, const char * str, const char * trim);

time_t str_date_time_to_time(const char * date, const char * time);

double interpolation_linear(double x0, double y0, double x1, double y1, double x);

#endif
