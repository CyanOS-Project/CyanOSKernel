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

PathParser::PathParser(const char* path) : m_path(path), m_len(strlen(path))
{
}

unsigned PathParser::path_element_count()
{
	unsigned count = 0;
	for (size_t i = 0; i < m_len; i++) {
		if (m_path[i] == m_spliter)
			count++;
	}
	return count;
}

int PathParser::get_element(unsigned element_index, char* element, unsigned len)
{
	const char* current = m_path;
	while (element_index) {
		current = get_next_element(current);
		if (!current)
			return 1; // FIXME:select an non-zero error to return.
		element_index++;
	}
	size_t current_len = get_next_element(current) - current;
	element[current_len] = 0;
	memcpy(element, current, current_len);
	return 0;
}

bool PathParser::is_valid()
{
	if (strlen(m_path) == 0)
		return false;

	if (m_path[0] != m_spliter)
		return false;

	return true;
}

const char* PathParser::get_next_element(const char* path)
{
	for (size_t i = 0; i < m_len; i++) {
		if (path[i] == m_spliter)
			return path + 1;
	}
	return nullptr;
}