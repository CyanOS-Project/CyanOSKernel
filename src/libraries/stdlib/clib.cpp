#include "clib.h"

void memcpy(void* dest, const void* source, unsigned int len)
{
	char* _dest = static_cast<char*>(dest);
	const char* _source = static_cast<const char*>(source);
	for (size_t i = 0; i < len; i++)
		_dest[i] = _source[i];
}

void memmove(void* dest, const void* source, unsigned int len)
{
	char* _dest = static_cast<char*>(dest);
	const char* _source = static_cast<const char*>(source);

	if (_source < _dest) {
		_source += len;
		_dest += len;
		while (len--)
			*--_dest = *--_source;
	} else {
		while (len--)
			*_dest++ = *_source++;
	}
}

void memset(void* dest, char value, unsigned int len)
{
	char* _dest = (char*)dest;
	for (size_t i = 0; i < len; i++)
		_dest[i] = value;
}

unsigned strlen(const char* str)
{
	unsigned len = 0;
	while ((str[len]))
		len++;
	return len;
}

bool memcmp(const void* source, const void* dest, unsigned int len)
{
	char* _dest = (char*)dest;
	char* _source = (char*)source;
	for (size_t i = 0; i < len; i++)
		if (_source[i] != _dest[i])
			return 1;
	return 0;
}

bool strncmp(const char* source, const char* dest, unsigned int len)
{
	return memcmp(source, dest, len);
}

bool strcmp(const char* source, const char* dest)
{
	unsigned src_len = strlen(source);
	if (src_len != strlen(dest))
		return 1;
	for (size_t i = 0; i < src_len; i++) {
		if (source[i] != dest[i])
			return 1;
	}
	return 0;
}

int strcpy(char* dst, const char* src)
{
	unsigned i = 0;
	while ((dst[i] = src[i]))
		i++;
	return i;
}

int toupper(char* str)
{
	unsigned i = 0;
	while (str[i]) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] += 'A' - 'a';
		i++;
	}
	return i;
}

int tolower(char* str)
{
	unsigned i = 0;
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
	unsigned i, j;

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
