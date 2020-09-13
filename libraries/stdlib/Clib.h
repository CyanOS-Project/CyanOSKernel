#pragma once
#include <Types.h>

extern "C" void memcpy(void* dest, const void* source, unsigned int len);
extern "C" void memset(void* dest, char value, unsigned int len);
extern "C" bool memcmp(const void* source, const void* dest, unsigned int len);
extern "C" bool strncmp(const char* source, const char* dest, unsigned int len);
extern "C" bool strcmp(const char* source, const char* dest);
extern "C" int strcpy(char* dst, const char* src);
extern "C" unsigned strlen(const char* str);
extern "C" int toupper(char* str);
extern "C" int tolower(char* str);
extern "C" void itoa(char* buf, unsigned long int n, int base);