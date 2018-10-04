#include "local_database.h"

static double **values = NULL;
static unsigned int num_results = 0;	// global because of deinit()
static unsigned int num_sensors = 0;	// global because of deinit()
static unsigned int written_results = 0;

static linked_t *header_table = NULL;
static char sensorlist[MAXIMUM_NUMBER_OF_OUTPUTS][128];
static unsigned int sensor_ctr = 0;

void localdb_init(const char *file, const char *date_from, const char *time_from, const char *date_until, const char *time_until, const char *sensors){
	FILE *fp = NULL;
	char line[MAX_LINE_SIZE];
	unsigned int i, j;

	time_t t_start = str_date_time_to_time(date_from, time_from);
	time_t t_end = str_date_time_to_time(date_until, time_until);

	dconsole_setcolor(LIGHTGRAY, BLACK);

	printf("  %s %s = %ld\n", date_from, time_from, t_start);
	printf("  %s %s = %ld\n", date_until, time_until, t_end);

	fp = fopen(file, "r");
	if(!fp){
		dconsole_error("fopen() failed.");
	}

	num_results = localdb_count_lines(fp) - 1; // header
	localdb_load_sensor_list(sensors);
	localdb_print_sensor_list();
	num_sensors = sensor_ctr + 1;	// + time

	// printf("  %u, %u\n", num_results, num_sensors);

	values = (double **)malloc(num_results * sizeof(double));

	for(i = 0; i < num_results; i++){
		values[i] = (double *)malloc(num_sensors * sizeof(double));
	}

	/* header */
	fgets(line, MAX_LINE_SIZE, fp);
	localdb_load_header_table(line);
	localdb_print_header_table();
	time_t t_offset = 0;
	bool firstdate = true;

	while(fgets(line, MAX_LINE_SIZE, fp)){
		j = 0;
		bool timeinrange = false;
		// printf("%s\n", line);

		char trimmedline[MAX_LINE_SIZE];
		str_trim(trimmedline, line, " \t\n\r");
		unsigned int column = 0, sen;
		char date[128], time[128];

		time_t t = 0;
		char *split = strtok(trimmedline, ";");    // create tokens
		while(split){
			// printf("%s\n", split);

			/* date and time */
			if(column == 0){
				strcpy(date, split);
			} else if(column == 1){
				strcpy(time, split);
				t = str_date_time_to_time(date, time);

				if(t >= t_start && t <= t_end){
					timeinrange = true;
				}

				if(timeinrange){
					if(firstdate){
						t_offset = t;
						firstdate = false;
					}

					t -= t_offset;
					values[written_results][j++] = (double)t;
				} else {
					break;
				}
			/* rest */
			} else {
				if(timeinrange){
					linked_t *current = header_table;
					/* search in the header*/
					while(current){
						/* search in the sensorlist */
						for(sen = 0; sen < sensor_ctr; sen++){
							/* required sensor found */
							if(!strcmp(current->name, sensorlist[sen]) && column == current->index){
								// printf("found: %s = %s\n", current->name, split);
								values[written_results][j++] = atof(split);
								break;
							}
						}
						current = current->next;
					}
				}
			}

			column++;
			split = strtok(NULL, ";");  // step to the next token
		}

		if(timeinrange)
			written_results++;
	}

	fclose(fp);

	dconsole_write("file is loaded:");
	dconsole_setcolor(GREEN, BLACK);
	printf("  %u, %u\n", written_results, num_sensors);
	for(i = 0; i < written_results; i++){
		printf("  ");
		for(j = 0; j < num_sensors; j++){
			printf("%.2f\t", values[i][j]);
		}
		printf("\n");
	}
}

void localdb_deinit(void){
	localdb_destroy_header_table();

	if(values){
		unsigned int i;
		for(i = 0; i < num_results; i++){
			free(values[i]);
		}

		free(values);
	}
}

void localdb_step(double t, bool lin_interpolate, double *out){
	unsigned int i, j;
	if(values){
		uint8_t num_outs = num_sensors <= MAXIMUM_NUMBER_OF_OUTPUTS ? num_sensors : MAXIMUM_NUMBER_OF_OUTPUTS;

		for(i = 0; i < written_results - 1; i++){
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
			if(t >= values[written_results - 1][0]){
				for(j = 1; j <= num_outs; j++){
					out[(j - 1)] = values[written_results - 1][j];
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

unsigned int localdb_count_lines(FILE *fp){
	char ch;
	unsigned int lines = 0;

	while(!feof(fp)){
		ch = fgetc(fp);
		if(ch == '\n')
			lines++;
	}

	fseek(fp, 0, SEEK_SET);	// reset fp

	return lines;
}

void localdb_load_header_table(const char *header){
	char trimmedhead[MAX_LINE_SIZE];
	str_trim(trimmedhead, header, " \t\n\r");
	unsigned int index = 0;

	linked_t *current;
	char *split = strtok(trimmedhead, ";");    // create tokens
	while(split){
		if(header_table == NULL){
			header_table = (linked_t *)malloc(sizeof(linked_t));
			strcpy(header_table->name, split);
			header_table->index = index;
			header_table->next = NULL;
			current = header_table;
		} else {
			current->next = (linked_t *)malloc(sizeof(linked_t));
			strcpy(current->next->name, split);
			current->next->index = index;
			current->next->next = NULL;
			current = current->next;
		}
		index++;
		split = strtok(NULL, ";");  // step to the next token
	}
}

void localdb_print_header_table(void){
	linked_t *current = header_table;
	dconsole_setcolor(YELLOW, BLACK);
	printf(" Header table:\n");
	while(current) {
		printf("  %s : %d\n", current->name, current->index);
		current = current->next;
	}
}

void localdb_destroy_header_table(void){
	linked_t *current;
	while(header_table) {
		current = header_table;
		header_table = header_table->next;
		free(current);
	}
}

void localdb_load_sensor_list(const char *sensors){
	char trimmedsensors[MAX_LINE_SIZE];
	str_trim(trimmedsensors, sensors, " \t\n\r");
	char *split = strtok(trimmedsensors, ",");    // create tokens
	while(split){
		strcpy(sensorlist[sensor_ctr++], split);
		split = strtok(NULL, ";");  // step to the next token
	}
}

void localdb_print_sensor_list(void){
	unsigned int i;
	dconsole_setcolor(CYAN, BLACK);
	printf(" Sensor list:\n");
	for(i = 0; i < sensor_ctr; i++){
		printf("  %s\n", sensorlist[i]);
	}
}
