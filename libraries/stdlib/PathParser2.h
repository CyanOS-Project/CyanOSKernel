#pragma once
#include "StringView.h"
#define SPLITER '/'

enum class PathType { Absolute, Relative, RelativeRecursive };

class PathParser2
{
  private:
	StringView m_working_dir;
	const size_t m_working_dir_count;
	StringView m_path;
	const size_t m_count;
	size_t calculate_count(const StringView& path) const;
	StringView get_element_from_full_path(const StringView& path, size_t index) const;

  public:
	static bool is_valid(const StringView& path);
	static PathType get_type(const StringView& path);

	explicit PathParser2(const StringView& absolute_path);
	explicit PathParser2(const StringView& relative_path, const StringView& working_dir);
	~PathParser2();
	size_t count() const;
	StringView element(size_t index) const;
	StringView last_element() const;
};