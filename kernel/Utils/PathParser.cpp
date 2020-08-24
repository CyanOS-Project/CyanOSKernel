#include "PathParser.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <cstring>
	#define ASSERT(x) assert(x)
#else
	#include "Lib/Stdlib.h"
	#include "Utils/Assert.h"
#endif

PathParser::PathParser(const StringView& path) : m_path{path}, m_count{calc_count()}
{
	ASSERT(is_valid());
}

PathParser::~PathParser()
{
}

size_t PathParser::count()
{
	return m_count;
}

StringView PathParser::element(size_t index)
{
	ASSERT(index < count());
	size_t pos = 0;
	size_t cur = index;
	while (cur--) {
		pos = m_path.find(SPLITER, pos + 1);
	}
	size_t len = [&]() {
		if (index == count() - 1) {
			return m_path.length() - pos - 1;
		} else {
			return m_path.find(SPLITER, pos + 1) - pos - 1;
		}
	}();

	return m_path.substr(pos + 1, len);
}

bool PathParser::is_valid()
{
	if (m_path.length() == 0)
		return false;

	if (m_path[0] != SPLITER)
		return false;

	if (m_path[m_path.length() - 1] == SPLITER)
		return false;

	return true;
}

size_t PathParser::calc_count()
{
	size_t count = 0;
	size_t last_found = 0;

	do {
		last_found = m_path.find(SPLITER, last_found + 1);
		++count;
	} while (last_found != StringView::NOT_FOUND);

	return count;
}