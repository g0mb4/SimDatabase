#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAXIMUM_NUMBER_OF_OUTPUTS	10

#define ABS( x ) ( x = x < 0 ? x * -1 : x )

int str_count_char(const char *str, char ch);
void str_trim(char *out, const char *str, const char *trim);

time_t str_date_time_to_time(const char *date, const char *time);

double interpolation_linear(double x0, double y0, double x1, double y1, double x);

#endif
