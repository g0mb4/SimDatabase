#include "dconsole.h"

void dconsole_init(void){
#ifdef _MSC_VER
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *stream;
		freopen_s(&stream, "CON", "w", stdout);
		SetConsoleTitle("Debug Console");

		dconsole_setcolor(WHITE, BLACK);

		printf(" Debug Console\n\n");
#endif
}

void dconsole_deinit(void){
#ifdef _MSC_VER
		FreeConsole();
#endif
}

void _dconsole_write_color(uint8_t fg, uint8_t bg, const char *msg, va_list argp){
    dconsole_setcolor(fg, bg);

    time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime(buffer, 80, "[%Y-%m-%dT%H:%M:%S] ", timeinfo);
	printf("%s", buffer);

  	vprintf(msg, argp);

	printf("\n");
}

void dconsole_write(uint8_t fg, uint8_t bg, const char *msg, ...){
    va_list argp;
    va_start(argp, msg);
    _dconsole_write_color(fg, bg, msg, argp);
    va_end(argp);
}

void dconsole_info(const char *msg, ...){
    va_list argp;
    va_start(argp, msg);
    _dconsole_write_color(WHITE, BLACK, msg, argp);
    va_end(argp);
}

void dconsole_warn(const char *msg, ...){
    va_list argp;
    va_start(argp, msg);
    _dconsole_write_color(YELLOW, BLACK, msg, argp);
    va_end(argp);
}

void dconsole_error(const char *msg, ...){
    va_list argp;
    va_start(argp, msg);
    _dconsole_write_color(RED, BLACK, msg, argp);
    va_end(argp);
}

void dconsole_setcolor(unsigned char foreground, unsigned char background){
#ifdef _MSC_VER
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (foreground | (background << 4)));
#endif
}
