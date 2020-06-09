#pragma once
#include "Lib/stdlib.h"
#include "kernel_map.h"
#include "types.h"
#include <stdarg.h>

#define VGATEXTMODE_BUFFER 0x000B8000

typedef enum { MODE_80H25V = 0 } TerminalMode;

typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} VGAColor;

void setMode(TerminalMode Mode);
void setColor(VGAColor color, VGAColor backcolor);
void printf(const char* s, ...);
void putChar(char str);
void clearScreen();
void initiate_console();
void displayMemory(const char* Address, int Size);
void printStatus(const char* msg, bool Success);
void removeLine();
void pageUp();
void formatEscapeCharacters(unsigned char c, const char* s, int cur_arg);
void formatString(char* arg);
void formatPointer(int arg, int size);
void formatHex(int arg, int size, unsigned char c);
void formatUnsigned(int arg, int size);
void formatDecimal(int arg, int size);
void updateCursor();
void insertNewLine();
void insertCharacter(char str);
void insertBackSpace();