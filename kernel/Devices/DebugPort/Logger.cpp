#include "Logger.h"
#include "Lib/stdlib.h"
#include <cstdarg>

Logger::Logger(DebugColor color) : m_color{color}
{
}

Logger& Logger::operator<<(const char* str)
{
	DebugPort::write(str, m_color);
	return *this;
}

Logger& Logger::operator<<(const String& str)
{
	DebugPort::write(str.c_str(), m_color);
	return *this;
}

Logger& Logger::operator<<(int num)
{
	char buf[16];
	if (num < 0) {
		num = 0 - num;
		DebugPort::write("-", m_color);
	}

	itoa(buf, num, 10);
	toupper(buf);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(unsigned num)
{
	char buf[16];
	itoa(buf, num, 10);
	toupper(buf);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex num)
{
	char buf[16];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(void* ptr)
{
	char buf[16];
	itoa(buf, (int)ptr, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	DebugPort::write(buf, m_color);
	return *this;
}