#include "Logger.h"
#include "Lib/Stdlib.h"

StaticSpinlock Logger::lock;

void Logger::init()
{
	lock.init();
}

Logger::Logger(DebugColor color) : m_color{color}
{
	lock.acquire();
}
Logger::~Logger()
{
	lock.release();
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

Logger& Logger::operator<<(const char str)
{
	DebugPort::write(&str, m_color);
	return *this;
}

Logger& Logger::operator<<(uint64_t num)
{
	char buf[5];
	itoa(buf, num & 0xFFFFFFFF, 16);
	toupper(buf);
	DebugPort::write(buf, m_color);
	itoa(buf, num & 0xFFFFFFFF00000000, 16);
	toupper(buf);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(int num)
{
	char buf[11];
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
	char buf[11];
	itoa(buf, num, 10);
	toupper(buf);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex num)
{
	char buf[5];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(void* ptr)
{
	char buf[5];
	itoa(buf, (int)ptr, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	DebugPort::write(buf, m_color);
	return *this;
}