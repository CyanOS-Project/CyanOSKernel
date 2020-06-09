#include "stdlib.h"

void memcpy(char* dest, char* source, unsigned int len)
{
	for (size_t i = 0; i < len; i++)
		dest[i] = source[i];
}
void memset(char* dest, char value, unsigned int len)
{
	for (size_t i = 0; i < len; i++)
		dest[i] = value;
}
bool memcmp(char* source, char* dest, unsigned int len)
{
	for (size_t i = 0; i < len; i++)
		if (source[i] != dest[i])
			return 1;
	return 0;
}
bool strncmp(char* source, char* dest, unsigned int len)
{
	return memcmp(source, dest, len);
}
bool strcmp(char* source, char* dest, unsigned int len)
{
	int i = 0;
	while ((source[i] != 0 || dest[i] != 0) && (i < len)) {
		if (source[i] != dest[i])
			return 1;
		i++;
	}
	return 0;
}
int strcp(char* dst, const char* src)
{
	int i = 0;
	while ((dst[i] = src[i++]))
		;
	return i;
}
int strlen(char* str)
{
	int len = 0;
	while ((str[len]))
		len++;
	return len;
}
int toupper(char* str)
{
	int i = 0;
	while (str[i]) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] += 'A' - 'a';
		i++;
	}
	return i;
}
int tolower(char* str)
{
	int i = 0;
	while (str[i]) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] -= 'A' - 'a';
		i++;
	}
	return i;
}
void itoa(char* buf, unsigned long int n, int base)
{
	unsigned long int tmp;
	int i, j;

	tmp = n;
	i = 0;

	do {
		tmp = n % base;
		buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
	} while (n /= base);
	buf[i--] = 0;

	for (j = 0; j < i; j++, i--) {
		tmp = buf[j];
		buf[j] = buf[i];
		buf[i] = tmp;
	}
}
