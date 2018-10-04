#ifndef __SIMDATABASE_H__
#define __SIMDATABASE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "dconsole.h"
#include "mysql_wrapper.h"
#include "local_database.h"

#ifdef __GNUC__
#define EXLIB_API
#else
#define EXLIB_API __declspec(dllexport)
#endif

EXLIB_API extern void simdatabase_init(void);

EXLIB_API extern void simdatabase_step_mysql(const char *addr, int port, const char *user, const char *pass, const char *db, const char *table, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors, double time, bool lin_interpolate, double *out);
EXLIB_API extern void simdatabase_step_local(const char *file, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors, double time, bool lin_interpolate, double *out);

EXLIB_API extern void simdatabase_term(void);

#endif
