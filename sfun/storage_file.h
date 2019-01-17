#ifndef __STORAGE_FILE_H__
#define __STORAGE_FILE_H__

#include "common.h"

bool storage_exists(const char * name);

bool storage_open_write(const char * name);
bool storage_open_read(const char * name);

bool storage_save(uint32_t num_rows, uint32_t num_cols, double ** data);

bool storage_load_header(uint32_t * num_rows, uint32_t * num_cols);
bool storage_load_data(double ** data);

#endif
