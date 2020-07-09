#pragma once

#include "Lib/stdlib.h"
class PathParser
{
  private:
	const char m_spliter = '/';
	const char* m_path;
	const unsigned m_len;
	const char* get_next_element(const char* path);

  public:
	PathParser(const char* path);
	~PathParser();
	unsigned path_element_count();
	int get_element(unsigned element_index, char* element, unsigned len);
	bool is_valid();
};