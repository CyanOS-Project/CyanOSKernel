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
	const char* current = m_path;
	if (m_path[0] == SPLITER) // discard the first '/' if any.
		++current;
	size_t len = strlen(current);
	if (len == 0)
		return 0;

	while (current < m_path + len) {
		if (current[0] == SPLITER)
			++count;
		++current;
	}
	return count + 1;
}

int PathParser::get_element(unsigned element_index, char* element, unsigned len)
{
	const char* current = m_path;
	if (*current == SPLITER)
		current++;

	while (element_index) {
		current = get_next_element(current);
		if (!current)
			return 1; // FIXME:select an non-zero error to return.
		element_index--;
	}
	size_t current_len = size_t(get_next_element(current)) - size_t(current) - 1;
	memcpy(element, current, current_len);
	element[current_len] = 0;
	return 0;
}

bool PathParser::is_valid()
{
	if (strlen(m_path) == 0)
		return false;

	if (m_path[0] != SPLITER)
		return false;

	return true;
}

const char* PathParser::get_next_element(const char* path)
{
	while (path < m_path + m_len) {
		if (path[0] == SPLITER)
			return ++path;
		path++;
	}
	return m_path + m_len + 1;
}