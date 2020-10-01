#include "ArgumentParser.h"

ArgumentParser::ArgumentParser(StringView argument) : m_data{argument}, m_argument_list{3}
{
	if (argument.length() == 0)
		return;

	size_t pos = 0;
	size_t next_pos = 0;
	do {
		next_pos = m_data.find(' ', pos);
		if (next_pos != StringView::NOT_FOUND) {
			m_data[next_pos] = 0;
			m_argument_list.emplace_back(&m_data.c_str()[pos], next_pos - pos);
			pos = next_pos + 1;
		} else {
			m_argument_list.emplace_back(&m_data.c_str()[pos], StringView::END);
		}
	} while (next_pos != StringView::NOT_FOUND);
}

size_t ArgumentParser::count() const
{
	return m_argument_list.size();
}

StringView ArgumentParser::operator[](int index) const
{
	ASSERT(index < m_argument_list.size());

	return m_argument_list[index];
}