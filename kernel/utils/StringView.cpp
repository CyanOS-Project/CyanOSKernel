#include "StringView.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#define ASSERT(x) assert(x)
#else
	#include "Lib/stdlib.h"
	#include "utils/assert.h"
#endif

const size_t StringView::END = -1;
const size_t StringView::NOT_FOUND = -1;

StringView::StringView(const char* str) : m_size{strlen(str)}, m_data{str}
{
	ASSERT(m_data);
}

StringView::StringView(const char* str, size_t len) : m_size{len}, m_data{str}
{
	ASSERT(m_data);
}

StringView::StringView(const StringView& other) : m_size{other.m_size}, m_data{other.m_data}
{
}

StringView& StringView::operator=(const StringView& other)
{
	m_data = other.m_data;
	m_size = other.m_size;
	return *this;
}

StringView& StringView::operator=(const char* str)
{
	ASSERT(str);
	m_size = strlen(str);
	m_data = str;
	return *this;
}

StringView::~StringView()
{
}

char StringView::operator[](size_t off) const
{
	return m_data[off];
}

bool StringView::operator==(const StringView& other) const
{
	if (m_size != other.m_size)
		return false;
	if (strncmp(m_data, other.m_data, m_size))
		return false;
	return true;
}

bool StringView::operator==(const char* other) const
{
	if (m_size != strlen(other))
		return false;
	if (strncmp(m_data, other, m_size))
		return false;
	return true;
}

bool StringView::operator!=(const StringView& other) const
{
	if (m_size == other.m_size)
		return false;
	if (!strncmp(m_data, other.m_data, m_size))
		return false;
	return true;
}

bool StringView::operator!=(const char* other) const
{
	if (m_size == strlen(other))
		return false;
	if (!strncmp(m_data, other, m_size))
		return false;
	return true;
}

StringView StringView::substr(size_t pos, size_t len) const
{
	if (pos == END)
		pos = m_size;

	ASSERT(pos + len <= m_size);
	return StringView(m_data + pos, len);
}

size_t StringView::find(const StringView& str, size_t pos) const
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

size_t StringView::find(const char* str, size_t pos) const
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

size_t StringView::find(char c, size_t pos) const
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return find(str, pos);
}

size_t StringView::rfind(const StringView& str, size_t pos) const
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

size_t StringView::rfind(const char* str, size_t pos) const
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

size_t StringView::rfind(char c, size_t pos) const
{
	char str[2];
	str[0] = c;
	str[1] = 0;
	return rfind(str, pos);
}
