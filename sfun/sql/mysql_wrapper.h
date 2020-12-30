/*
 compiled with: mysql-connector-c-6.1.11
*/

#ifndef __MYSQL_WRAPPER_H__
#define __MYSQL_WRAPPER_H__

#include "mysql.h"

#include "common.h"
#include "dconsole.h"
#include "storage_file.h"

#define ID   0
#define DATE 1
#define TIME 2
#define DATA 3

int mywrapper_init(const char * addr, int port, const char * user,
                   const char * pass, const char * db, const char * table,
                   const char * date_from, const char * time_from,
                   const char * date_until, const char * time_until,
                   const char * sensors);
void mywrapper_deinit(void);
void mywrapper_step(double t, bool lin_interpolate, double * out);
uint32_t mywrapper_get_data_size(void);

int _mywrapper_query(const char * query, const char * sensorlist,
                     const char * fname);
bool _mywrapper_test_file(const char * fname);
bool _mywrapper_load_file(const char * fname);

bool _mywrapper_get_data(const char * str, const char * data, char * out);

#endif
