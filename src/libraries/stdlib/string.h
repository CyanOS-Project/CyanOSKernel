#pragma once
#include "types.h"

class StringView;
class String
{
  private:
	size_t m_size = 0;
	char* m_data = nullptr;
	void cleanup();
	const char m_empty_str = '\0'; // TODO: optimize using empty string if an empty string is entered.
	                               // TODO : implement capacity.

  public:
	String(StringView str);
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

	bool operator==(StringView other) const;
	bool operator==(const String& other) const;
	bool operator==(const char* other) const;
	bool operator!=(StringView other) const;
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

	String& push_back(char c);
	String& pop_back();
	char& operator[](size_t off);
	String& insert(size_t pos, const String& str);
	String& insert(size_t pos, const String& str, size_t subpos, size_t sublen);
	String& insert(size_t pos, const char* str);
	String& insert(size_t pos, const char* str, size_t subpos, size_t sublen);
	void erase(size_t pos, size_t len);
	template <typename Predictor> void erase_if(Predictor predictor)
	{
		size_t i = 0;
		while (i < m_size) {
			if (predictor(m_data[i])) {
				erase(i, 1);
			} else {
				i++;
			}
		}
	}

	static const size_t END;
	static const size_t NOT_FOUND;

	friend class StringView;
};