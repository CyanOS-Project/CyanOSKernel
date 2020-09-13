#pragma once
#include <StringView.h>
#define SPLITER '/'
class PathParser
{
  private:
	const StringView m_path;
	const size_t m_count;
	size_t calc_count();

  public:
	explicit PathParser(const StringView& path);
	~PathParser();
	size_t count() const;
	StringView element(size_t index) const;
	StringView last_element() const;
	bool is_valid() const;
};