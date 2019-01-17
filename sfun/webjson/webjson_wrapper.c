#include "webjson_wrapper.h"

static double ** values = NULL;
static unsigned int num_results = 0;	// global because of deinit()
static unsigned int num_sensors = 0;	// global because of deinit()

int wjwrapper_init(const char * addr, int port, const char * date_from,
    const char * time_from, const char * date_until, const char * time_until,
    const char * sensors){

    int status = 0;
    char fname[128], foo[128];
	sprintf(foo, "%s%s%s%s%s.bin", date_from, time_from, date_until, time_until, sensors);
	str_trim(fname, foo, "-:, ");
    str_buf_t * buf = str_buf_create();

	if(!_wjwrapper_test_file(fname)){
		if(tcp_connect(addr, port) < 0){
            status = -1;
            goto end;
        }

        if(tcp_get_data(buf, date_from, time_from, date_until, time_until, sensors) < 0){
            status = -2;
            goto end;
    	}

        if(_wjwrapper_remove_header(buf) < 0){
            status = -3;
            goto end;
        }

        char * sensorlist = (char *)malloc(sizeof(char) * strlen(sensors));
        if(sensorlist == NULL){
             dconsole_error("unable to create sensor list");
             status = -4;
             goto end;
        }

        str_trim(sensorlist, sensors, " ");
        int err = _wjwrapper_load_json(buf, sensorlist);
        free(sensorlist);

        if(err < 0){
            status = -5;
            goto end;
        }

        if(storage_open_write(fname)){
            if(!storage_save(num_results, num_sensors, values)){
                dconsole_error("unable to save the storage file");
            } else {
                dconsole_info("storage file created: %s", fname);
            }
            storage_close();
        } else {
            dconsole_warn("unable to create storage file");
        }

	} else {
        if(!_wjwrapper_load_file(fname)){
            status = -5;
            goto end;
        }
    }

end:
    str_buf_destroy(buf);
    tcp_disconnect();
	return status;
}

void wjwrapper_deinit(void){
    if(values){
		uint32_t i;
		for(i = 0; i < num_results; i++){
			if(values[i]){
				free(values[i]);
			}
		}
		free(values);
	}
}

void wjwrapper_step(double t, bool lin_interpolate, double * out){
    unsigned int i, j;
	uint8_t num_outs = num_sensors - 1;	// no time needed
	if(values){
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
				break;
			}

			/* time passed the last point, use last data */
			if(t >= values[num_results - 1][0]){
				for(j = 1; j <= num_outs; j++){
					out[(j - 1)] = values[num_results - 1][j];
				}
			}
		}

		dconsole_setcolor(CYAN, BLACK);
		printf(" %.2f : ", t);
		dconsole_setcolor(YELLOW, BLACK);
		for(j = 0; j < num_outs; j++){
			printf(" %.2f,", out[j]);
		}
		printf("\n");
	/* prevent crash */
	} else {
		dconsole_warn("no values were loaded, outputing zeros");
		for(i = 0; i < num_outs; i++){
			out[i] = 0.0;
		}
	}
}

bool _wjwrapper_test_file(const char * fname){
    bool b = storage_exists(fname);
    if(!b){
        dconsole_warn("storage file does not exist");
    }

    return b;
}

bool _wjwrapper_load_file(const char * fname){
    unsigned int i = 0, j = 0;
    if(!storage_open_read(fname)){
        dconsole_error("storage file does not exist");
        return false;
    } else {
        dconsole_info("storage file is found");
    }

    if(!storage_load_header(&num_results, &num_sensors)){
        dconsole_error("unable to read the header");
        return false;
    }

    values = (double **)malloc(num_results * sizeof(double *));

    if(values == NULL){
        dconsole_error("malloc() failed");
        return false;
    }

    for(i = 0; i < num_results; i++){
        values[i] = (double * )malloc(num_sensors * sizeof(double));
        if(values[i] == NULL){
            dconsole_error("malloc() failed");
            return false;
        }
    }

    if(!storage_load_data(values)){
        dconsole_error("unable to read the data");
        return false;
    }

    storage_close();

    dconsole_info("storage file is loaded:");
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

uint32_t wjwrapper_get_data_size(void){
	if(num_sensors <= 1){
		return 1;
	} else {
		return (uint32_t)(num_sensors - 1);
	}
}

bool _wjwrapper_remove_header(str_buf_t * buf){
    char * tmp = (char *) malloc(sizeof(char) * (buf->len + 1));
    if(!tmp){
        return false;
    }
    strcpy(tmp, buf->str);

    char * head_start = strstr(tmp, "\r\n\r\n");
    if(head_start) {
        str_buf_clear(buf);
        str_buf_add(buf, (head_start + 4));
    }
    free(tmp);

    dconsole_setcolor(CYAN, BLACK);
    printf("\nwithout header:\n%s\n", buf->str);

    return true;
}

int _wjwrapper_load_json(str_buf_t * buf, const char * sensorlist){
    int status = 0;
    cJSON * obj_data = NULL;
    cJSON * obj_data_elem = NULL;
    cJSON * obj = cJSON_Parse(buf->str);

    if (obj == NULL)
    {
        const char * error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL){
            dconsole_error("cJSON_Parse() failed: %s\n", error_ptr);
        }
        status = -1;
        goto end;
    }

    char * string = cJSON_Print(obj);
    if (string == NULL) {
       dconsole_warn("cJSON_Print() failed.");
    } else {
       dconsole_write(MAGENTA, BLACK, "parsed data:\n%s", string);
    }
    free(string);

    uint32_t i = 0, j = 0;
    time_t t, t_offset;
    bool firstdate = true;
    unsigned int n = 0;
    char sensorvalue[80], sensorequals[80];

    obj_data = cJSON_GetObjectItemCaseSensitive(obj, "data");

    num_results = cJSON_GetArraySize(obj_data);
    num_sensors = str_count_char(sensorlist, ',') + 2; // time + last

    dconsole_setcolor(LIGHTBLUE, BLACK);
    printf("  %u, %u\n", num_results, num_sensors);

    values = (double **)malloc(num_results * sizeof(double *));

    if(values == NULL){
        dconsole_error("malloc() failed");
        status = -2;
        goto end;
    }

    for(i = 0; i < num_results; i++){
        values[i] = (double * )malloc(num_sensors * sizeof(double));
        if(values[i] == NULL){
            dconsole_error("malloc() failed");
            status = -3;
            goto end;
        }
    }

    i = 0;
    cJSON_ArrayForEach(obj_data_elem, obj_data) {
        j = 0;
        cJSON * date = cJSON_GetObjectItemCaseSensitive(obj_data_elem, "date");
        cJSON * time = cJSON_GetObjectItemCaseSensitive(obj_data_elem, "time");

        if (!cJSON_IsString(date) || (date->valuestring == NULL)){
            dconsole_warn("unable to parse date");
        }

        if (!cJSON_IsString(time) || (time->valuestring == NULL)){
            dconsole_warn("unable to parse time");
        }

        t = str_date_time_to_time(date->valuestring, time->valuestring);

        /*  first entry t=0 */
        if(firstdate){
            t_offset = t;
            firstdate = false;
        }

        t -= t_offset;    				// offset from the first entry
        printf("  %ld\t", ABS( t ));	// bug??
        double td = (double)t;			// for fwrite()
        values[i][j++] = td;

        char * sensorsfind = strdup(sensorlist);    // save the pointer, because strtok() looses it
        char * split = strtok(sensorsfind, ",");    // create tokens

        /*  steping through the tokens */
       while(split){
           cJSON * sensor_val = cJSON_GetObjectItemCaseSensitive(obj_data_elem, split);
           double val = 0.0;
           if(cJSON_IsNumber(sensor_val)){
                 val = sensor_val->valuedouble;
           }
           free(sensor_val);

           values[i][j++] = val;
           printf("%s = %f, ", split, val);

           split = strtok(NULL, ",");  // step to the next token
       }
       printf("\n");
       n++;
       i++;
   }
   dconsole_info("query done. %d entries were found.", n);

end:
    cJSON_Delete(obj);
    return status;
}
