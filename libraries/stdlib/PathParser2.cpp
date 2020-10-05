#include "PathParser2.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <cstring>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif

PathParser2::PathParser2(const StringView& absolute_path) :
    m_working_dir{""},
    m_working_dir_count{0},
    m_path{absolute_path},
    m_count{calculate_count(m_path)}
{
	ASSERT(get_type(absolute_path) == PathType::Absolute);
}

PathParser2::PathParser2(const StringView& relative_path, const StringView& working_dir) :
    m_working_dir{working_dir},
    m_working_dir_count{calculate_count(m_working_dir)},
    m_path{relative_path},
    m_count{calculate_count(m_path)}
{
	ASSERT(get_type(m_working_dir) == PathType::Absolute);
	ASSERT(get_type(relative_path) != PathType::Absolute);
}

PathParser2::~PathParser2() {}

size_t PathParser2::count() const
{
	return m_count + m_working_dir_count;
}

StringView PathParser2::last_element() const
{
	return element(m_count - 1);
}

StringView PathParser2::element(size_t index) const
{
	ASSERT(index < count());

	if (index < m_working_dir_count) {
		return get_element_from_full_path(m_working_dir, index);
	} else {
		return get_element_from_full_path(m_path, index - m_working_dir_count);
	}
}

bool PathParser2::is_valid(const StringView& path)
{
	if (path.length() == 0)
		return false;

	if (path[path.length() - 1] == SPLITER)
		return false;

	return true;
}

PathType PathParser2::get_type(const StringView& path)
{
	ASSERT(is_valid(path));
	if (path[0] == SPLITER) {
		return PathType::Absolute;
	} else if (path.find("..") != StringView::END) {
		return PathType::RelativeRecursive;
	} else {
		return PathType::Relative;
	}
}

StringView PathParser2::get_element_from_full_path(const StringView& path, size_t index) const
{
	size_t pos = 0;
	if (path[0] == SPLITER) {
		pos = 1;
	}

	size_t cur = index;
	while (cur--) {
		pos = path.find(SPLITER, pos) + 1;
	}

	size_t len = [&]() {
		size_t next_splitter = path.find(SPLITER, pos);
		if (next_splitter == StringView::END) {
			return path.length() - pos;
		} else {
			return next_splitter - pos;
		}
	}();

	return path.substr(pos, len);
}

size_t PathParser2::calculate_count(const StringView& path) const
{
	size_t count = 0;
	size_t last_found = 0;

	do {
		last_found = path.find(SPLITER, last_found + 1);
		++count;
	} while (last_found != StringView::NOT_FOUND);

	return count;
}