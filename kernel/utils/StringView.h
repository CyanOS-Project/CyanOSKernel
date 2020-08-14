#pragma once
#include "types.h"

class String;

class StringView
{
  private:
	size_t m_size = 0;
	const char* m_data = nullptr;

  public:
	StringView(const String& str);
	StringView(const char* str);
	StringView(const char* str, size_t len);
	StringView(const StringView& other);
	StringView& operator=(const StringView& other);
	StringView& operator=(const char* str);
	~StringView();

	char operator[](size_t off) const;
	bool operator==(const StringView& other) const;
	bool operator==(const char* other) const;
	bool operator!=(const StringView& other) const;
	bool operator!=(const char* other) const;
	StringView substr(size_t pos, size_t len) const;
	size_t find(const StringView& str, size_t pos = 0) const;
	size_t find(const char* s, size_t pos = 0) const;
	size_t find(char c, size_t pos = 0) const;
	size_t rfind(const StringView& str, size_t pos = END) const;
	size_t rfind(const char* s, size_t pos = END) const;
	size_t rfind(char c, size_t pos = END) const;
	size_t length() const
	{
		return m_size;
	}
	static const size_t END;
	static const size_t NOT_FOUND;

	friend class String;
};