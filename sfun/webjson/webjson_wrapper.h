#ifndef __WEBJSON_WRAPPER_H__
#define __WEBJSON_WRAPPER_H__

#include "common.h"
#include "dconsole.h"
#include "tcp_clnt.h"
#include "str_buf.h"
#include "cJSON.h"
#include "storage_file.h"

int wjwrapper_init(const char * addr, int port, const char * date_from, const char * time_from, const char * date_until, const char * time_until, const char * sensors);
void wjwrapper_deinit(void);
void wjwrapper_step(double t, bool lin_interpolate, double * out);
uint32_t wjwrapper_get_data_size(void);

bool _wjwrapper_test_file(const char * fname);
bool _wjwrapper_load_file(const char * fname);

bool _wjwrapper_remove_header(str_buf_t * buf);
int _wjwrapper_load_json(str_buf_t * buf, const char * sensorlist);

#endif
