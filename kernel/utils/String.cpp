#include "String.h"

String::String(const char* str) : m_size{strlen(str)}, m_data{new char[m_size + 1]}
{
	ASSERT(m_data);
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
	memcpy(m_data, other.m_data, m_size);
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
		memcpy(m_data, other.m_data, m_size);
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
	return insert(m_size, &c);
}

String& String::pop_back(char c)
{
	return insert(0, &c);
}

String& String::insert(size_t pos, const String& str)
{
	size_t new_len = str.m_size + m_size;
	char* new_data = new char[new_len + 1];
	memcpy(new_data, m_data, pos);
	memcpy(new_data + pos, str.m_data, str.m_size);
	memcpy(new_data + pos + str.m_size, m_data + pos, m_size - pos);

	cleanup();

	m_size = new_len;
	m_data = new_data;
	return *this;
}

String& String::insert(size_t pos, const String& str, size_t subpos, size_t sublen)
{
	return *this;
}

String& String::insert(size_t pos, const char* s)
{
	return *this;
}

String& String::insert(size_t pos, const char* s, size_t len)
{
	return *this;
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
}

inline const char* String::c_str() const
{
}

inline size_t String::length() const
{
}*/

inline void String::cleanup()
{
	if (m_data)
		delete[] m_data;
	m_size = 0;
}