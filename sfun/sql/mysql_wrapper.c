#include "mysql_wrapper.h"

static MYSQL * mysql = NULL;
static double ** values = NULL;
static unsigned int num_results = 0; // global because of deinit()
static unsigned int num_sensors = 0; // global because of deinit()

int mywrapper_init(const char * addr, int port, const char * user,
                   const char * pass, const char * db, const char * table,
                   const char * date_from, const char * time_from,
                   const char * date_until, const char * time_until,
                   const char * sensors) {
    int status = 0;

    char fname[128], foo[128];
    sprintf(foo, "%s%s%s%s%s.bin", date_from, time_from, date_until, time_until,
            sensors);
    str_trim(fname, foo, "-:, ");

    if (!_mywrapper_test_file(fname)) {
        mysql = mysql_init(mysql);

        if (!mysql) {
            dconsole_error("mysql_init() failed");
            status = -1;
            goto end;
        } else {
            if (!mysql_real_connect(mysql, addr, user, pass, db, port, NULL,
                                    0)) {
                dconsole_error("mysql_real_connect() failed: %s",
                               mysql_error(mysql));
                status = -2;
                goto end;
            } else {
                dconsole_info("connected to the server");

                char query[512];
                sprintf(query,
                        "SELECT * FROM `%s` WHERE CONCAT(`date`,' ', `time`) "
                        ">= '%s %s' AND CONCAT(`date`,' ', `time`) <= '%s %s'",
                        table, date_from, time_from, date_until, time_until);

                char * sensorlist =
                    (char *)malloc(sizeof(char) * strlen(sensors));
                if (sensorlist == NULL) {
                    dconsole_error("unable to create sensor list");
                    status = -3;
                    goto end;
                }

                str_trim(sensorlist, sensors, " ");
                int err = _mywrapper_query(query, sensorlist, fname);
                free(sensorlist);

                if (err < 0) {
                    status = -4;
                    goto end;
                }

                if (storage_open_write(fname)) {
                    if (!storage_save(num_results, num_sensors, values)) {
                        dconsole_error("unable to save the storage file");
                    } else {
                        dconsole_info("storage file created: %s", fname);
                    }
                    storage_close();
                } else {
                    dconsole_warn("unable to create storage file");
                }
            }
        }
    } else {
        if (!_mywrapper_load_file(fname)) {
            status = -5;
            goto end;
        }
    }

end:
    if (mysql) {
        mysql_close(mysql);
    }

    return status;
}

void mywrapper_deinit(void) {
    if (values) {
        uint32_t i;
        for (i = 0; i < num_results; i++) {
            if (values[i]) {
                free(values[i]);
            }
        }
        free(values);
    }
}

/*  str - data stream in the form of "N1=x1;N2=x2"
   sensors - N1,N2
   ret - 0 on succes, < 0 on error
 */
int _mywrapper_query(const char * query, const char * sensorlist,
                     const char * fname) {
    /*  only use the server if local file is not exist */
    if (mysql) {
        dconsole_setcolor(WHITE, BLACK);
        printf(" %s\n", query);
        if (mysql_query(mysql, query)) {
            dconsole_error("mysql_query() failed: %s", mysql_error(mysql));
            return -2;
        } else {
            MYSQL_RES * result = mysql_store_result(mysql);

            uint32_t i = 0, j = 0;
            time_t t, t_offset;
            bool firstdate = true;
            char sensorvalue[80], sensorequals[80];

            MYSQL_ROW row;
            uint32_t n = 0;
            num_results = mysql_num_rows(result);
            uint32_t num_fields = mysql_num_fields(result);
            num_sensors = str_count_char(sensorlist, ',') + 2; // time + last

            dconsole_setcolor(LIGHTBLUE, BLACK);
            printf("  %u, %u\n", num_results, num_sensors);

            values = (double **)malloc(num_results * sizeof(double *));

            if (values == NULL) {
                dconsole_error("malloc() failed");
                return -3;
            }

            for (i = 0; i < num_results; i++) {
                values[i] = (double *)malloc(num_sensors * sizeof(double));
                if (values[i] == NULL) {
                    dconsole_error("malloc() failed");
                    return -4;
                }
            }

            i = 0;
            while ((row = mysql_fetch_row(result))) {
                j = 0;
                t = str_date_time_to_time(row[DATE], row[TIME]);

                /*  first entry t=0 */
                if (firstdate) {
                    t_offset = t;
                    firstdate = false;
                }

                t -= t_offset; // offset from the first entry
                printf("  %ld\t", ABS(t)); // bug??
                double td = (double)t;

                values[i][j++] = td;

                char * sensorsfind = strdup(
                    sensorlist); // save the pointer, because strtok() looses it
                char * split = strtok(sensorsfind, ","); // create tokens
                /*  steping through the tokens */
                while (split) {
                    sprintf(sensorequals,
                            "%s=", split); // add '=' to avoid collisions
                    /*  if value found */
                    if (_mywrapper_get_data(row[DATA], sensorequals,
                                            sensorvalue)) {
                        double d = atof(sensorvalue);
                        values[i][j++] = d;

                        printf("%s = %f, ", split, d);
                    }

                    split = strtok(NULL, ","); // step to the next token
                }
                printf("\n");
                n++;
                i++;
            }
            dconsole_info("query done, %d entries were found", n);
            mysql_free_result(result);
            return 0;
        }
    } else {
        dconsole_error("no mysql connection");
        return -1;
    }
}

void mywrapper_step(double t, bool lin_interpolate, double * out) {
    unsigned int i, j;
    uint8_t num_outs = num_sensors - 1; // no time needed
    if (values) {
        for (i = 0; i < num_results - 1; i++) {
            if (t >= values[i][0] && t <= values[i + 1][0]) {
                if (lin_interpolate) {
                    /* output the data  */
                    for (j = 1; j <= num_outs; j++) {
                        out[(j - 1)] =
                            interpolation_linear(values[i][0], values[i][j],
                                                 values[i + 1][0],
                                                 values[i + 1][j], t);
                    }
                } else {
                    /* output the data  */
                    for (j = 1; j <= num_outs; j++) {
                        out[(j - 1)] = values[i][j];
                    }
                }
                break;
            }

            /* time passed the last point, use last data */
            if (t >= values[num_results - 1][0]) {
                for (j = 1; j <= num_outs; j++) {
                    out[(j - 1)] = values[num_results - 1][j];
                }
            }
        }

        dconsole_setcolor(CYAN, BLACK);
        printf(" %.2f : ", t);
        dconsole_setcolor(YELLOW, BLACK);
        for (j = 0; j < num_outs; j++) {
            printf(" %.2f,", out[j]);
        }
        printf("\n");
        /* prevent crash */
    } else {
        dconsole_warn("no values were loaded, outputing zeros");
        for (i = 0; i < num_outs; i++) {
            out[i] = 0.0;
        }
    }
}

bool _mywrapper_test_file(const char * fname) {
    bool b = storage_exists(fname);
    if (!b) {
        dconsole_warn("storage file does not exist");
    }

    return b;
}

bool _mywrapper_load_file(const char * fname) {
    unsigned int i = 0, j = 0;
    if (!storage_open_read(fname)) {
        dconsole_error("storage file does not exist");
        return false;
    } else {
        dconsole_info("storage file is found");
    }

    if (!storage_load_header(&num_results, &num_sensors)) {
        dconsole_error("unable to read the header");
        return false;
    }

    values = (double **)malloc(num_results * sizeof(double *));

    if (values == NULL) {
        dconsole_error("malloc() failed");
        return false;
    }

    for (i = 0; i < num_results; i++) {
        values[i] = (double *)malloc(num_sensors * sizeof(double));
        if (values[i] == NULL) {
            dconsole_error("malloc() failed");
            return false;
        }
    }

    if (!storage_load_data(values)) {
        dconsole_error("unable to read the data");
        return false;
    }

    storage_close();

    dconsole_info("storage file is loaded:");
    dconsole_setcolor(GREEN, BLACK);
    printf("  %u, %u\n", num_results, num_sensors);
    for (i = 0; i < num_results; i++) {
        printf("  ");
        for (j = 0; j < num_sensors; j++) {
            printf("%.2f\t", values[i][j]);
        }
        printf("\n");
    }

    return true;
}

/*  str - data stream in fom of "N1=x1;N2=x2"
   data - N1
   out - output buffer for x1
   returns - value found
 */
bool _mywrapper_get_data(const char * str, const char * sensor, char * out) {
    char * ptr = strstr(str, sensor); // find N1
    uint16_t i = 0;

    /*  if N1 found */
    if (ptr) {
        ptr += strlen(sensor); // pointer to the char AFTER N1

        /*  steping through the stream until it reaches ';' of the end */
        while (*ptr && *ptr != ';') {
            out[i++] = *ptr; // put x1 to the buffer
            ptr++; // step the pointer
        }
        out[i] = '\0'; // close the output buffer

        return true; // found
    }

    return false; // not found
}

uint32_t mywrapper_get_data_size(void) {
    if (num_sensors <= 1) {
        return 1;
    } else {
        return (uint32_t)(num_sensors - 1);
    }
}
