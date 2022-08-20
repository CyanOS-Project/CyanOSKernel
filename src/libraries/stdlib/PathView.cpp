#include "PathView.h"
#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <cstring>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif

PathView::PathView(const String& path) : PathView{StringView{path}} {}

PathView::PathView(const char* path) : PathView{StringView{path}} {}

PathView::PathView(StringView path) :
    m_type{get_type(path)},
    m_absolute_path{m_type == PathType::Absolute ? path : ""},
    m_relative_path{m_type != PathType::Absolute ? path : ""},
    m_absolute_path_count{m_type == PathType::Absolute ? count_elements(m_absolute_path) : 0},
    m_relative_path_count{m_type != PathType::Absolute ? count_elements(m_relative_path) : 0}
{
}

PathView::PathView(StringView working_dir, StringView relative_path) :
    m_type{PathType::Absolute},
    m_absolute_path{working_dir},
    m_relative_path{relative_path},
    m_absolute_path_count{count_elements(m_absolute_path)},
    m_relative_path_count{count_elements(m_relative_path)}
{
	ASSERT(get_type(m_absolute_path) == PathType::Absolute);
	ASSERT(get_type(relative_path) != PathType::Absolute);
}

PathView::PathView(const PathView& other) :
    m_type{other.m_type},
    m_absolute_path{other.m_absolute_path},
    m_relative_path{other.m_relative_path},
    m_absolute_path_count{other.m_absolute_path_count},
    m_relative_path_count{other.m_relative_path_count}
{
}

PathView& PathView::operator=(const PathView& other)
{
	m_type = other.m_type;
	m_absolute_path = other.m_absolute_path;
	m_relative_path = other.m_relative_path;
	m_absolute_path_count = other.m_absolute_path_count;
	m_relative_path_count = other.m_relative_path_count;
	return *this;
}

PathView::~PathView() {}

void PathView::set_absolute_path(PathView path)
{
	ASSERT(m_type != PathType::Absolute);
	ASSERT(path.m_type == PathType::Absolute);
	ASSERT(path.m_absolute_path_count > 0);
	ASSERT(path.m_relative_path_count == 0);
	m_absolute_path = path.m_absolute_path;
	m_absolute_path_count = path.m_absolute_path_count;
}

size_t PathView::count() const
{
	return m_relative_path_count + m_absolute_path_count;
}

PathType PathView::type() const
{
	return m_type;
}

StringView PathView::operator[](int index) const
{
	ASSERT(index < int(count()));
	ASSERT(index > -int(count()) - 1);
	if (index < 0) {
		index = count() + index;
	}

	if (index < int(m_absolute_path_count)) {
		return get_element_from_full_path(m_absolute_path, index);
	} else {
		return get_element_from_full_path(m_relative_path, index - m_absolute_path_count);
	}
}

PathView PathView::sub_path(int start_index, int end_index) const
{
	ASSERT((start_index < int(count())) && (start_index > -int(count()) - 1));
	ASSERT((end_index < int(count())) && (end_index > -int(count()) - 1));

	if (start_index < 0) {
		start_index = count() + start_index;
	}

	if (end_index < 0) {
		end_index = count() + end_index;
	}
	ASSERT(end_index > start_index);

	auto new_absolute_path = sub_absolute_path(start_index, end_index);
	auto new_relative_path = sub_relative_path(start_index, end_index);

	if (new_absolute_path == "") {
		return PathView(new_relative_path);
	} else if (new_relative_path == "") {
		return PathView(new_absolute_path);
	} else {
		return PathView(new_absolute_path, new_relative_path);
	}
}

String PathView::full_path() const
{
	String full_path_str(m_absolute_path);
	if (m_relative_path_count) {
		full_path_str += "/";
		full_path_str += m_relative_path;
	}
	return full_path_str;
}

bool PathView::is_valid(StringView path)
{
	if (path.length() == 0)
		return false;

	if (path.length() != 1 && path[path.length() - 1] == SPLITER)
		return false;

	return true;
}

PathType PathView::get_type(StringView path)
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

StringView PathView::get_element_from_full_path(StringView path, size_t index) const
{
	int pos = 0;
	if (path[0] == SPLITER) {
		pos = 1;
	}

	int cur = index;
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

StringView PathView::sub_absolute_path(size_t start_index, size_t end_index) const
{
	if (start_index >= m_absolute_path_count) {
		return "";
	}

	int start_pos = 1;
	int start_index_search = start_index;
	while (start_index_search--) {
		start_pos = m_absolute_path.find(SPLITER, start_pos) + 1;
	}

	if (end_index < m_absolute_path_count - 1) {
		int end_pos = start_pos;
		int end_index_search = end_index - start_index + 1;
		while (end_index_search--) {
			end_pos = m_absolute_path.find(SPLITER, end_pos) + 1;
		}
		return m_absolute_path.substr(start_pos - 1, size_t(end_pos - start_pos));
	} else {
		return m_absolute_path.substr(start_pos - 1, StringView::END);
	}
}

StringView PathView::sub_relative_path(size_t start_index, size_t end_index) const
{
	if (end_index < m_absolute_path_count) {
		return "";
	} else {
		if (start_index < m_absolute_path_count) {
			start_index = 0;
		} else {
			start_index -= m_absolute_path_count;
		}
		end_index -= m_absolute_path_count;
	}

	int start_pos = 0;
	int start_index_search = start_index;
	while (start_index_search--) {
		start_pos = m_absolute_path.find(SPLITER, start_pos) + 1;
	}

	if (end_index < m_relative_path_count - 1) {
		int end_pos = start_pos;
		int end_index_search = end_index - start_index + 1;
		while (end_index_search--) {
			end_pos = m_relative_path.find(SPLITER, end_pos) + 1;
		}
		return m_relative_path.substr(start_pos, size_t(end_pos - start_pos) - 1);
	} else {
		return m_relative_path.substr(start_pos, StringView::END);
	}
}

size_t PathView::count_elements(StringView path) const
{
	if (path[0] == SPLITER && path.length() == 1) {
		return 0;
	}

	int last_found = 0;
	if (path[0] == SPLITER) {
		last_found = 1;
	}

	int count = 0;
	while ((last_found = path.find(SPLITER, last_found)) != int(StringView::NOT_FOUND) && ++last_found) {
		++count;
	}

	return count + 1;
}