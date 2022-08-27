#include "argument_parser.h"

ArgumentParser::ArgumentParser(StringView argument) : m_data{argument}, m_argument_list{3}
{
	// arg1 arg2 arg33 arg444
	if (m_data.length() == 0)
		return;

	trim_arguments();

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

StringView ArgumentParser::operator[](size_t index) const
{
	ASSERT(index < m_argument_list.size());

	return m_argument_list[index];
}

void ArgumentParser::trim_arguments()
{
	// trim repeated spaces
	char last_char = '\0';
	m_data.erase_if([&](char c) {
		if (c == ' ' && c == last_char) {
			return true;
		} else {
			last_char = c;
			return false;
		}
	});
	// trim start space
	if (m_data[0] == ' ') {
		m_data.erase(0, 1);
	}
	// trim last space
	if (m_data[m_data.length() - 1] == ' ') {
		m_data.erase(m_data.length() - 1, 1);
	}
}