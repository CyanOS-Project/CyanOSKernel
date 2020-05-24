#pragma once
#include "memmap.h"
#include "stdlib.h"
#include "types.h"
#include <stdarg.h>

typedef enum {
    MODE_80H25V = 0
} TerminalMode;

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
void displayMemory(char* Address, int Size);
void printSuccess(char* msg, bool Success);
void removeLine();
void pageUp();
void formatEscapeCharacters(unsigned char c, const char* s, va_list ap);
void formatString(va_list ap);
void formatPointer(va_list ap, int size);
void formatHex(va_list ap, int size, unsigned char c);
void formatUnsigned(va_list ap, int size);
void formatDecimal(va_list ap, int size);
void updateCursor();
void insertNewLine();
void insertCharacter(char str);
void insertBackSpace();