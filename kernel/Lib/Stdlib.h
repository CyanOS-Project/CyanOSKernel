#pragma once
#include "Utils/Types.h"

void memcpy(void* dest, const void* source, unsigned int len);
void memset(void* dest, char value, unsigned int len);
bool memcmp(const void* source, const void* dest, unsigned int len);
bool strncmp(const char* source, const char* dest, unsigned int len);
bool strcmp(const char* source, const char* dest);
int strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int toupper(char* str);
int tolower(char* str);
void itoa(char* buf, unsigned long int n, int base);