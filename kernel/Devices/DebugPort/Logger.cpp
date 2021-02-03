#include "Logger.h"
#include <Assert.h>
#include <Clib.h>

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
	DebugPort::write("\n", m_color);
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

Logger& Logger::operator<<(char str)
{
	DebugPort::write(&str, m_color);
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

Logger& Logger::operator<<(int32_t num)
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

Logger& Logger::operator<<(uint32_t num)
{
	char buf[11];
	itoa(buf, num, 10);
	toupper(buf);
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex num)
{
	char buf[9];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = num.m_size * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex8 num)
{
	char buf[9];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = sizeof(uint8_t) * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex16 num)
{
	char buf[9];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = sizeof(uint16_t) * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex32 num)
{
	char buf[9];
	itoa(buf, num.m_data, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = sizeof(uint32_t) * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(Hex64 num)
{
	char buf[9];
	itoa(buf, (num.m_data & 0xFFFFFFFF00000000) >> 32, 16);
	itoa(buf + strlen(buf), num.m_data & 0xFFFFFFFF, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = sizeof(uint32_t) * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}

Logger& Logger::operator<<(void* ptr)
{
	char buf[9];
	itoa(buf, (int)ptr, 16);
	toupper(buf);
	DebugPort::write("0x", m_color);
	size_t leading_zeros = sizeof(uintptr_t) * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
	return *this;
}