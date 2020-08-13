#pragma once
#include "types.h"

class String
{
  private:
	size_t m_size = 0;
	char* m_data = nullptr;
	void cleanup();

  public:
	String(const char* str);
	String(const char* str, size_t len);
	String(String&& other);
	String(const String& other);
	String& operator=(String&& other);
	String& operator=(const String& other);
	String& operator=(const char* other);
	~String();

	String& operator+=(const String& other);
	String& operator+=(const char* other);
	String& operator+=(char other);
	String operator+(const String& other) const;
	char operator[](size_t off) const;

	String& push_back(char c);
	String& pop_back();
	String& insert(size_t pos, const String& str);
	String& insert(size_t pos, const String& str, size_t subpos, size_t sublen);
	String& insert(size_t pos, const char* str);
	String& insert(size_t pos, const char* str, size_t subpos, size_t sublen);

	bool operator==(const String& other) const;
	bool operator==(const char* other) const;
	bool operator!=(const String& other) const;
	bool operator!=(const char* other) const;
	String substr(size_t pos, size_t len) const;
	size_t find(const String& str, size_t pos = 0) const;
	size_t find(const char* s, size_t pos = 0) const;
	size_t find(char c, size_t pos = 0) const;
	size_t rfind(const String& str, size_t pos = END) const;
	size_t rfind(const char* s, size_t pos = END) const;
	size_t rfind(char c, size_t pos = END) const;
	const char* c_str() const;
	size_t length() const;

	static const size_t END;
	static const size_t NOT_FOUND;
};