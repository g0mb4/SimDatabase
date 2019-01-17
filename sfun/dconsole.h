#ifndef __DEBUGCONSOLE_H__
#define __DEBUGCONSOLE_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

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
#else
    #define BLACK			0
    #define BLUE			0
    #define GREEN			0
    #define CYAN			0
    #define RED				0
    #define MAGENTA			0
    #define BROWN			0
    #define LIGHTGRAY		0
    #define DARKGRAY		0
    #define LIGHTBLUE		0
    #define LIGHTGREEN		0
    #define LIGHTCYAN		0
    #define LIGHTRED		0
    #define LIGHTMAGENTA	0
    #define YELLOW			0
    #define WHITE			0
#endif

void dconsole_init(void);
void dconsole_deinit(void);

void _dconsole_write_color(uint8_t fg, uint8_t bg, const char *msg, va_list argp);

void dconsole_write(uint8_t fg, uint8_t bg, const char *msg, ...);
void dconsole_info(const char *msg, ...);
void dconsole_warn(const char *msg, ...);
void dconsole_error(const char *msg, ...);

void dconsole_setcolor(unsigned char foreground, unsigned char background);

#endif
