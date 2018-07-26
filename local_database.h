#ifndef __LOCAL_DATABASE_H__
#define __LOCAL_DATABASE_H__

#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "dconsole.h"

typedef struct S_LINKED {
	char name[128];
	unsigned int index;
	struct S_LINKED *next;
} linked_t;

#define MAX_LINE_SIZE	1024

void localdb_init(const char *file, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors);
void localdb_deinit(void);

void localdb_step(double t, bool lin_interpolate, double *out);

unsigned int localdb_count_lines(FILE *fp);

void localdb_load_header_table(const char *header);
void localdb_print_header_table(void);
void localdb_destroy_header_table(void);

void localdb_load_sensor_list(const char *sensors);
void localdb_print_sensor_list(void);

#endif
