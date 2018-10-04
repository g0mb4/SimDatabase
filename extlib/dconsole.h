#ifndef __DEBUGCONSOLE_H__
#define __DEBUGCONSOLE_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	#define BLACK			0
	#define BLUE			1
	#define GREEN			2
	#define CYAN			3
	#define RED				4
	#define MAGENTA			5
	#define BROWN			6
	#define LIGHTGRAY		7
	#define DARKGRAY		8
	#define LIGHTBLUE		9
	#define LIGHTGREEN		10
	#define LIGHTCYAN		11
	#define LIGHTRED		12
	#define LIGHTMAGENTA	13
	#define YELLOW			14
	#define WHITE			15
#endif

void dconsole_init(void);
void dconsole_deinit(void);

void dconsole_write(const char *msg, ...);
void dconsole_error(const char *msg, ...);

void dconsole_setcolor(unsigned char foreground, unsigned char background);


#endif
