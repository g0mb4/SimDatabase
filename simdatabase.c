#include "simdatabase.h"

static bool firststep = true;

void simdatabase_init(){
	dconsole_init();

	dconsole_write("started.");
}

void simdatabase_step_mysql(const char *addr, int port, const char *user, const char *pass, const char *db, const char *table, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors, double time, bool lin_interpolate, double *out){
	if(firststep){
		mywrapper_init(addr, port, user, pass, db, table, date_from, time_from, date_until, time_until, sensors);
		mywrapper_query();
		firststep = false;
	}

	mywrapper_step(time, lin_interpolate, out);
}


void simdatabase_step_local(const char *file, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors, double time, bool lin_interpolate, double *out){
	if(firststep){
		localdb_init(file, date_from, time_from, date_until, time_until, sensors);
		firststep = false;
	}

	localdb_step(time, lin_interpolate, out);
}

void simdatabase_term(void){
	mywrapper_deinit();
	localdb_deinit();

	dconsole_write("stoped.");
	dconsole_deinit();
}
