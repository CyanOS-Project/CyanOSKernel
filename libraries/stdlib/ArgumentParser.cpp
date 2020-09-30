#include "ArgumentParser.h"
#include "Assert.h"

ArgumentParser::ArgumentParser(StringView argument) : m_argument{argument}
{
	size_t count = 1;
	size_t pos = 0;
	while ((pos = m_argument.find(' ', pos)) != StringView::NOT_FOUND) {
		++pos;
		++count;
	}
	m_count = count;
}

size_t ArgumentParser::count() const
{
	return m_count;
}

StringView ArgumentParser::operator[](int index) const
{
	// Hello this_is_arg1 this_is_arg2
	ASSERT(index < m_count);

	size_t pos = 0;
	while (index--) {
		pos = m_argument.find(' ', pos);
		++pos;
	}

	size_t next_space;
	if ((next_space = m_argument.find(' ', pos + 1)) == StringView::NOT_FOUND) {
		return m_argument.substr(pos, StringView::END);
	} else {
		return m_argument.substr(pos, next_space - pos);
	}
	return m_argument.substr(pos, m_argument.find(' ', pos + 1) - pos);
}