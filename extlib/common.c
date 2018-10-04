#include "common.h"

int str_count_char(const char *str, char ch){
	int ret = 0;

	while(*str++){
		if(*str == ch)
			ret++;
	}

	return ret;
}

void str_trim(char *out, const char *str, const char *trim){
	unsigned int tlen = strlen(trim);
	unsigned int i, j = 0;
	bool found;

	while(*str){
		found = false;
		for(i = 0; i < tlen; i++){
			if(*str == trim[i]){
				found = true;
				break;
			}
		}

		if(!found){
			out[j++] = *str;
		}
		str++;
	}

	out[j] = '\0';
}

time_t str_date_time_to_time(const char *date, const char *time){
	 struct tm tm;

	 sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);    // convert DATE
	 sscanf(time, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);     // convert TIME
	 tm.tm_year -= 1900;     // struct tm counts years AFTER 1900
	 tm.tm_mon -= 1;		// struct tm counts months from 0
	 return mktime(&tm);
}

/* https://en.wikipedia.org/wiki/Linear_interpolation */
double interpolation_linear(double x0, double y0, double x1, double y1, double x){
	/* prevent division by 0 */
	if(x1 == x0){
		return y0;
	} else {
		return (y0 * (x1 - x) + y1 * (x - x0)) / (x1 - x0);
	}
}
