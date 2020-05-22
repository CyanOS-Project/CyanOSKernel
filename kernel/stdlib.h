#pragma once
#include "types.h"

void memcpy(char* dest, char* source, unsigned int len);
void memset(char* dest, char value, unsigned int len);
bool memcmp(char* source, char* dest, unsigned int len);
bool strncmp(char* source, char* dest, unsigned int len);
bool strcmp(char* source, char* dest, unsigned int len);
int strcp(char* dst, const char* src);
int strlen(char* str);
int toupper(char* str);
int tolower(char* str);
void itoa(char* buf, unsigned long int n, int base);

