#include "PathParser.h"

PathParser::PathParser(const char* path) : m_path(path), m_len(strlen(path))
{
}

PathParser::~PathParser()
{
}

unsigned PathParser::path_element_count()
{
	unsigned count = 0;
	if (m_len == 1 && m_path[0] == m_spliter)
		return 0;

	for (size_t i = 0; i < m_len; i++) {
		if (m_path[i] == m_spliter)
			count++;
	}
	return count;
}

int PathParser::get_element(unsigned element_index, char* element, unsigned len)
{
	const char* current = m_path + 1;
	while (element_index) {
		current = get_next_element(current);
		if (!current)
			return 1; // FIXME:select an non-zero error to return.
		element_index--;
	}
	size_t current_len = size_t(get_next_element(current)) - size_t(current) - 1;
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
	while (path < m_path + m_len) {
		if (path[0] == m_spliter)
			return ++path;
		path++;
	}
	return m_path + m_len + 1;
}