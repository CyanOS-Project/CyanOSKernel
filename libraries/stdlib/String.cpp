#include "String.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <cstring>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif
#include "StringView.h"

const size_t String::END = -1;
const size_t String::NOT_FOUND = -1;

String::String(const StringView& str) : m_size{str.length()}, m_data{new char[m_size + 1]}
{
	ASSERT(m_data);
	memcpy(m_data, str.m_data, m_size);
	m_data[m_size] = 0;
}

String::String(const char* str) : m_size{strlen(str)}, m_data{new char[m_size + 1]}
{
	ASSERT(m_data);
	memcpy(m_data, str, m_size + 1);
}

String::String(const char* str, size_t len) : m_size{len}, m_data{new char[m_size + 1]}
{
	ASSERT(m_data);
	memcpy(m_data, str, m_size);
	m_data[m_size] = 0;
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
	ASSERT(this != &other);

	cleanup();
	m_size = other.m_size;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
}

String& String::operator=(const String& other)
{
	// FIXME: Optimize this, if other.m_size is less than m_size, no need for allocating memory.
	ASSERT(this != &other);
	cleanup();
	m_size = other.m_size;
	m_data = new char[m_size + 1];
	memcpy(m_data, other.m_data, m_size + 1);
	return *this;
}

String& String::operator=(const char* str)
{
	m_size = strlen(str);
	m_data = new char[m_size + 1];
	ASSERT(m_data);
	memcpy(m_data, str, m_size + 1);
	return *this;
}

String::~String()
{
	cleanup();
}

String& String::operator+=(const String& other)
{
	return insert(END, other);
}

String& String::operator+=(const char* other)
{
	return insert(END, other);
}

String& String::operator+=(char other)
{
	return insert(END, &other);
}

String String::operator+(const String& other) const
{
	String new_string{*this};
	new_string += other;
	return new_string;
}

char String::operator[](size_t off) const
{
	return m_data[off];
}

String& String::push_back(char c)
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return insert(END, str);
}

String& String::pop_back()
{
	ASSERT(m_size);
	m_size--;
	m_data[m_size] = 0;
	return *this;
}

String& String::insert(size_t pos, const String& str)
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);

	const size_t new_len = str.m_size + m_size;
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
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);

	const size_t str_len = strlen(str);
	const size_t new_len = str_len + m_size;
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
	if (subpos == END)
		subpos = str.m_size;
	if (pos == END)
		pos = m_size;

	ASSERT(subpos + sublen <= str.m_size);
	ASSERT(pos <= m_size);

	const size_t str_len = str.m_size > sublen ? sublen : str.m_size;
	const size_t new_len = str_len + m_size;
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

String& String::insert(size_t pos, const char* str, size_t subpos, size_t sublen)
{
	if (subpos == END)
		subpos = strlen(str);
	if (pos == END)
		pos = m_size;

	ASSERT(subpos + sublen <= strlen(str));
	ASSERT(pos <= m_size);

	const size_t new_len = sublen + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str + subpos, sublen);
	memcpy(new_data + pos + sublen, m_data + pos, m_size - pos);
	new_data[new_len] = 0;

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

void String::erase(size_t pos, size_t len)
{
	ASSERT(pos <= m_size);

	memcpy(m_data + pos, m_data + pos + len, m_size - pos + len);
	m_size -= len;
}

char& String::operator[](size_t off)
{
	ASSERT(off < m_size);
	return m_data[off];
}

bool String::operator==(const StringView& other) const
{
	if (m_size != other.m_size)
		return false;
	if (memcmp(m_data, other.m_data, m_size))
		return false;
	return true;
}

bool String::operator==(const String& other) const
{
	if (m_size != other.m_size)
		return false;
	if (strcmp(m_data, other.m_data))
		return false;
	return true;
}

bool String::operator==(const char* other) const
{
	if (strcmp(m_data, other))
		return false;
	return true;
}

bool String::operator!=(const StringView& other) const
{
	if (m_size == other.m_size)
		return false;
	if (!memcmp(m_data, other.m_data, m_size))
		return false;
	return true;
}

bool String::operator!=(const String& other) const
{
	if (m_size == other.m_size)
		return false;
	if (!strcmp(m_data, other.m_data))
		return false;
	return true;
}

bool String::operator!=(const char* other) const
{
	if (!strcmp(m_data, other))
		return false;
	return true;
}

String String::substr(size_t pos, size_t len) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos + len <= m_size);
	return String(m_data + pos, len);
}

size_t String::find(const String& str, size_t pos) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);
	for (size_t i = pos; i < (m_size - str.m_size) + 1; i++) {
		if (!strncmp(m_data + i, str.m_data, str.m_size))
			return i;
	}
	return NOT_FOUND;
}

size_t String::find(const char* str, size_t pos) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);
	const size_t str_len = strlen(str);
	for (size_t i = pos; i < (m_size - str_len) + 1; i++) {
		if (!strncmp(m_data + i, str, str_len))
			return i;
	}
	return NOT_FOUND;
}

size_t String::find(char c, size_t pos) const
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return find(str, pos);
}

size_t String::rfind(const String& str, size_t pos) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);
	for (int i = pos - str.m_size; i >= 0; i--) {
		if (!strncmp(m_data + i, str.m_data, str.m_size))
			return i;
	}
	return NOT_FOUND;
}

size_t String::rfind(const char* str, size_t pos) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos <= m_size);
	const size_t str_len = strlen(str);
	for (int i = pos - str_len; i >= 0; i--) {
		if (!strncmp(m_data + i, str, str_len))
			return i;
	}
	return NOT_FOUND;
}

size_t String::rfind(char c, size_t pos) const
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return rfind(str, pos);
}

const char* String::c_str() const
{
	return m_data;
}

size_t String::length() const
{
	return m_size;
}

void String::cleanup()
{
	delete[] m_data;
	m_data = nullptr;
	m_size = 0;
}