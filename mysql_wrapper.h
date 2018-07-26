/*
 compiled with: mysql-connector-c-6.1.11
*/

#ifndef __MYSQL_WRAPPER_H__
#define __MYSQL_WRAPPER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <mysql.h>

#include "common.h"
#include "dconsole.h"

#define	ID		0
#define	DATE	1
#define	TIME	2
#define	DATA	3

void mywrapper_init(const char *addr, int port, const char *user, const char *pass, const char *db, const char *table, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors);
void mywrapper_deinit(void);

void mywrapper_step(double t, bool lin_interpolate, double *out);

void mywrapper_query(void);
bool mywrapper_test_file(void);
bool mywrapper_load_file(void);

bool mywrapper_get_data(const char *str, const char *data, char *out);

#endif
