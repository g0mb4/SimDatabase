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

void dconsole_write(const char *msg, ...){
	dconsole_setcolor(WHITE, BLACK);

	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime(buffer, 80, "[%Y-%m-%dT%H:%M:%S] ", timeinfo);
	printf("%s", buffer);

	va_list arglist;

  	va_start(arglist, msg);
  	vprintf(msg, arglist);
  	va_end(arglist );

	printf("\n");
}

void dconsole_error(const char *msg, ...){
	dconsole_setcolor(RED, BLACK);

	time_t rawtime;
  	struct tm *timeinfo;
  	char buffer[80];

  	time (&rawtime);
  	timeinfo = localtime (&rawtime);
	strftime(buffer, 80, "[%Y-%m-%dT%H:%M:%S] ", timeinfo);
	printf("%s", buffer);

	va_list arglist;

	printf("ERROR: " );

  	va_start(arglist, msg);
  	vprintf(msg, arglist);
  	va_end(arglist);

	printf("\n");
}

void dconsole_setcolor(unsigned char foreground, unsigned char background){
#ifdef _MSC_VER
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (foreground | (background << 4)));
#endif
}
