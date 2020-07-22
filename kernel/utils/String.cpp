#include "String.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#define ASSERT(x) assert(x)
#else
	#include "Lib/stdlib.h"
	#include "utils/assert.h"
#endif

String::String(const char* str) : m_size{strlen(str)}, m_data{new char[m_size + 1]}
{
	ASSERT(m_data);
	memcpy(m_data, str, m_size + 1);
}

String::String(String&& other)
{
	cleanup();
	m_size = other.m_size;
	m_data = other.m_data;
	other.m_data = nullptr;
}

String::String(const String& other)
{
	m_size = other.m_size;
	m_data = new char[m_size + 1];
	memcpy(m_data, other.m_data, m_size + 1);
}

String& String::operator=(String&& other)
{
	if (this != &other) {
		cleanup();

		m_size = other.m_size;
		m_data = other.m_data;
		other.m_data = nullptr;
	}
	return *this;
}

String& String::operator=(const String& other)
{
	if (this != &other) {
		m_size = other.m_size;
		m_data = new char[m_size + 1];
		memcpy(m_data, other.m_data, m_size + 1);
	}
	return *this;
}

String::~String()
{
	cleanup();
}

String& String::operator+=(const String& other)
{
	return insert(m_size, other);
}

String& String::operator+=(const char* other)
{
	return insert(m_size, other);
}

String& String::operator+=(char other)
{
	return insert(m_size, &other);
}

String String::operator+(const String& other)
{
	String new_string{*this};
	new_string += other;
	return new_string;
}

char String::operator[](size_t off)
{
	return m_data[off];
}

String& String::push_back(char c)
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return insert(m_size, str);
}

String& String::pop_back(char c)
{
	// FIXME:
	return *this;
}

String& String::insert(size_t pos, const String& str)
{
	size_t new_len = str.m_size + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str.m_data, str.m_size);
	memcpy(new_data + pos + str.m_size, m_data + pos, m_size - pos);
	new_data[new_len] = 0;

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

String& String::insert(size_t pos, const char* str)
{
	size_t str_len = strlen(str);
	size_t new_len = str_len + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str, str_len);
	memcpy(new_data + pos + str_len, m_data + pos, m_size - pos);
	new_data[new_len] = 0;

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

String& String::insert(size_t pos, const String& str, size_t subpos, size_t sublen)
{
	ASSERT(subpos <= str.m_size);
	size_t str_len = str.m_size > sublen ? sublen : str.m_size;
	size_t new_len = str_len + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str.m_data + subpos, str_len);
	memcpy(new_data + pos + str_len, m_data + pos, m_size - pos);
	new_data[new_len] = 0;

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

String& String::insert(size_t pos, const char* str, size_t subpos)
{
	ASSERT(subpos <= strlen(str));
	size_t str_len = strlen(str) - subpos;

	size_t new_len = str_len + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str + subpos, str_len);
	memcpy(new_data + pos + str_len, m_data + pos, m_size - pos);
	new_data[new_len] = 0;

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

bool String::operator==(const String& other)
{
	if (strcmp(m_data, other.m_data))
		return false;
	if (m_size != other.m_size)
		return false;
	return true;
}

bool String::operator==(const char* other)
{
	if (strcmp(m_data, other))
		return false;
	return true;
}

bool String::operator!=(const String& other)
{
	if (!strcmp(m_data, other.m_data))
		return false;
	if (m_size == other.m_size)
		return false;
	return true;
}

bool String::operator!=(const char* other)
{
	if (!strcmp(m_data, other))
		return false;
	return true;
}
/*
String String::substr(size_t pos = 0, size_t len = 0) const
{
}

size_t String::find(const String& str, size_t pos = 0) const
{
}

size_t String::find(const char* s, size_t pos = 0) const
{
}

size_t String::find(char c, size_t pos = 0) const
{
}

size_t String::rfind(const String& str, size_t pos = 0) const
{
}

size_t String::rfind(const char* s, size_t pos = 0) const
{
}

size_t String::rfind(char c, size_t pos = 0) const
{
}*/

inline void String::cleanup()
{
	if (m_data)
		delete[] m_data;
	m_size = 0;
}