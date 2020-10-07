#pragma once
#include "String.h"
#include "StringView.h"

#define SPLITER '/'

enum class PathType { Absolute, Relative, RelativeRecursive };

class PathView
{
  public:
	static bool is_valid(const StringView& path);
	static PathType get_type(const StringView& path);

	PathView(const String& path);
	PathView(StringView path);
	PathView(const char* path);
	PathView(StringView working_dir, StringView relative_path);
	~PathView();
	void set_absolute_path(PathView path);
	size_t count() const;
	PathType type() const;
	StringView operator[](int) const;
	PathView sub_path(int start_index, int end_index) const;
	String full_path() const;

  private:
	PathType m_type;
	StringView m_absolute_path;
	StringView m_relative_path;
	size_t m_absolute_path_count;
	size_t m_relative_path_count;
	size_t count_elements(const StringView& path) const;
	StringView get_element_from_full_path(const StringView& path, size_t index) const;
	StringView sub_absolute_path(size_t start_index, size_t end_index) const;
	StringView sub_relative_path(size_t start_index, size_t end_index) const;
	StringView get_sub_path(StringView path, size_t start_index, size_t end_index) const;
};