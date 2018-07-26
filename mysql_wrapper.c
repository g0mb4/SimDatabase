#include "mysql_wrapper.h"

static MYSQL *mysql = NULL;
static MYSQL_RES *result = NULL;
static double **values = NULL;
static unsigned int num_results = 0;	// global because of deinit()
static unsigned int num_sensors = 0;	// global because of deinit()
static char query[512];
static char fname[128];
static char sensorlist[128];

void mywrapper_init(const char *addr, int port, const char *user, const char *pass, const char *db,
                    const char *table, const char *date_from, const char *time_from, const char *date_until, const char *time_until,
					const char *sensors){

	char foo[128];
	sprintf(foo, "%s%s%s%s%s.bin", date_from, time_from, date_until, time_until, sensors);
	str_trim(fname, foo, "-:, ");

	if(!mywrapper_test_file()){
		mysql = mysql_init(mysql);

		if(!mysql){
			 dconsole_error("mysql_init() failed.");
		} else {
			if(!mysql_real_connect(mysql, addr, user, pass, db, port, NULL, 0)){
				 dconsole_error("mysql_real_connect() failed: %s", mysql_error(mysql));
				 mysql_close(mysql);
				 mysql = NULL;
			} else {
				dconsole_write("connected.");
			}
		}
		sprintf(query, "SELECT * FROM `%s` WHERE CONCAT(`date`,' ', `time`) >= '%s %s' AND CONCAT(`date`,' ', `time`) <= '%s %s'", table, date_from, time_from, date_until, time_until);
		str_trim(sensorlist, sensors, " ");
	}
}

void mywrapper_deinit(void){
	if(values){
		unsigned int i;
		for(i = 0; i < num_results; i++){
			free(values[i]);
		}

		free(values);
	}

	if(mysql){
       mysql_close(mysql);
    }
}

/* str - data stream in the form of "N1=x1;N2=x2"
   sensors - N1,N2
 */
void mywrapper_query(void){
	/* only use the server if local file is not exist */
	if(!mywrapper_load_file()){
		if(mysql){
			dconsole_setcolor(WHITE, BLACK);
			printf(" %s\n", query);
			if(mysql_query(mysql, query)) {
				dconsole_error("mysql_query() failed: %s", mysql_error(mysql));
			} else {
				MYSQL_RES *result = mysql_store_result(mysql);
				FILE *fp = NULL;

	            time_t t, t_offset;
	            bool firstdate = true;
	            char sensorvalue[80], sensorequals[80];

				MYSQL_ROW row;
				unsigned int i = 0, j = 0, n = 0;
				num_results = mysql_num_rows(result);
				unsigned int num_fields = mysql_num_fields(result);
				num_sensors = str_count_char(sensorlist, ',') + 2; // time + last

				fp = fopen(fname, "wb");

				if(!fp){
					dconsole_error("unable to create storage file.");
					return;
				}

				dconsole_setcolor(LIGHTBLUE, BLACK);
				printf("  %u, %u\n", num_results, num_sensors);

				fwrite(&num_results, sizeof(unsigned int), 1, fp);
				fwrite(&num_sensors, sizeof(unsigned int), 1, fp);

				i = 0;
				while ((row = mysql_fetch_row(result))) {
					t = str_date_time_to_time(row[DATE], row[TIME]);

	                /* first entry t=0 */
					if(firstdate){
						t_offset = t;
						firstdate = false;
					}

					t -= t_offset;    // offset from the first entry
	                printf("  %ld\t", ABS( t ));	// bug??
					double td = (double)t;	// for fwrite()
					fwrite(&td, sizeof(double), 1, fp);

					j = 0;

	                char *sensorsfind = strdup(sensorlist);     // save the pointer, because strtok() looses it
	                char *split = strtok(sensorsfind, ",");    // create tokens
	                 /* steping through the tokens */
	                while(split){
	                    sprintf(sensorequals, "%s=", split);  // add '=' to avoid collisions
	                    printf("%s", sensorequals);
	                    /* if value found */
	                    if(mywrapper_get_data(row[DATA], sensorequals, sensorvalue)){
	                        printf("%s\t", sensorvalue);
							double d = atof(sensorvalue);
							fwrite(&d, sizeof(double), 1, fp);
	                    }

	                    split = strtok(NULL, ",");  // step to the next token
	                }
	                printf("\n");
					i++;
	                n++;
				}
	            dconsole_write("query done. %d entries were found. storage file created : %s", n, fname);
				mysql_free_result(result);
				fclose(fp);

				mywrapper_load_file();
			}
		} else {
			dconsole_error("no mysql connection.");
		}
	}
}

bool mywrapper_test_file(void){
	FILE *fp = NULL;

	fp = fopen(fname, "r");

	if(!fp){
		dconsole_error("storage file does not exist.");
		return false;
	} else {
		fclose(fp);
	}

	return true;
}

bool mywrapper_load_file(void){
	FILE *fp = NULL;
	unsigned int i = 0, j = 0;

	fp = fopen(fname, "r");

	if(!fp){
		dconsole_error("storage file does not exist.");
		return false;
	} else {
		dconsole_write("storage file is found.");
	}

	fread(&num_results, sizeof(unsigned int), 1, fp);
	fread(&num_sensors, sizeof(unsigned int), 1, fp);

	values = (double **)malloc(num_results * sizeof(double));

	for(i = 0; i < num_results; i++){
		values[i] = (double *)malloc(num_sensors * sizeof(double));
	}

	for(i = 0; i < num_results; i++){
		for(j = 0; j < num_sensors; j++){
			double d;
			fread(&d, sizeof(double), 1, fp);
			values[i][j] = d;
		}
	}

	fclose(fp);

	dconsole_write("storage file is loaded:");
	dconsole_setcolor(GREEN, BLACK);
	printf("  %u, %u\n", num_results, num_sensors);
	for(i = 0; i < num_results; i++){
		printf("  ");
		for(j = 0; j < num_sensors; j++){
			printf("%.2f\t", values[i][j]);
		}
		printf("\n");
	}

	return true;
}

void mywrapper_step(double t, bool lin_interpolate, double *out){
	unsigned int i, j;
	if(values){
		uint8_t num_outs = num_sensors <= MAXIMUM_NUMBER_OF_OUTPUTS ? num_sensors : MAXIMUM_NUMBER_OF_OUTPUTS;

		for(i = 0; i < num_results - 1; i++){
			if(t >= values[i][0] && t <= values[i + 1][0]){
				if(lin_interpolate){
					/* output the data  */
					for(j = 1; j <= num_outs; j++){
						out[(j - 1)] = interpolation_linear(values[i][0], values[i][j], values[i + 1][0], values[i + 1][j], t);
					}
				} else {
					/* output the data  */
					for(j = 1; j <= num_outs; j++){
						out[(j - 1)] = values[i][j];
					}
				}

				/* padding zeros */
				for(--j; j <= MAXIMUM_NUMBER_OF_OUTPUTS; j++){
					out[(j - 1)] = 0.0;
				}
				break;
			}

			/* time passed the last point, use last data */
			if(t >= values[num_results - 1][0]){
				for(j = 1; j <= num_outs; j++){
					out[(j - 1)] = values[num_results - 1][j];
				}

				for(--j; j <= MAXIMUM_NUMBER_OF_OUTPUTS; j++){
					out[(j - 1)] = 0.0;
				}
			}
		}

		dconsole_setcolor(CYAN, BLACK);
		printf(" %.2f : ", t);
		dconsole_setcolor(YELLOW, BLACK);
		for(j = 0; j < MAXIMUM_NUMBER_OF_OUTPUTS; j++){
			printf(" %.2f,", out[j]);
		}
		printf("\n");
	/* prevent crash */
	} else {
		dconsole_error("no values were loaded, outputing zeros.");
		for(i = 0; i < MAXIMUM_NUMBER_OF_OUTPUTS; i++){
			out[i] = 0.0;
		}
	}
}

/* str - data stream in fom of "N1=x1;N2=x2"
   data - N1
   out - output buffer for x1
   returns - value found
 */
bool mywrapper_get_data(const char *str, const char *sensor, char *out){
    char *ptr = strstr(str, sensor);  // find N1
    uint16_t i = 0;

    /* if N1 found */
    if(ptr){
        ptr += strlen(sensor);    // pointer to the char AFTER N1

        /* steping through the stream until it reaches ';' of the end */
        while(*ptr && *ptr != ';'){
            out[i++] = *ptr;    // put x1 to the buffer
            ptr++;              // step the pointer
        }
        out[i] = '\0';  // close the output buffer

        return true;    // found
    }

    return false;   // not found
}
