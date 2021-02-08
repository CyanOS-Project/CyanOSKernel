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

	DebugPort::write("0x", m_color);
	print_hex(num.m_data, num.m_size);
	return *this;
}

Logger& Logger::operator<<(Hex8 num)
{
	DebugPort::write("0x", m_color);
	print_hex(num.m_data, 1);
	return *this;
}

Logger& Logger::operator<<(Hex16 num)
{
	DebugPort::write("0x", m_color);
	print_hex(num.m_data, 2);
	return *this;
}

Logger& Logger::operator<<(Hex32 num)
{
	DebugPort::write("0x", m_color);
	print_hex(num.m_data, 4);
	return *this;
}

Logger& Logger::operator<<(Hex64 num)
{
	DebugPort::write("0x", m_color);
	print_hex((num.m_data & 0xFFFFFFFF00000000) >> 32, 4);
	print_hex(num.m_data & 0xFFFFFFFF, 4);

	return *this;
}

Logger& Logger::operator<<(void* ptr)
{
	DebugPort::write("0x", m_color);
	print_hex(uintptr_t(ptr), sizeof(uintptr_t));
	return *this;
}

Logger& Logger::operator<<(const MACAddress& mac)
{
	uint8_t mac_data[6];
	mac.copy(mac_data);
	for (size_t i = 0; i < 6; i++) {
		print_hex(mac_data[i], 1);
		if (i != 5) {
			DebugPort::write(":", m_color);
		}
	}
	return *this;
}

void Logger::print_hex(uint32_t value, uint8_t size)
{
	char buf[9];
	itoa(buf, value, 16);
	toupper(buf);
	size_t leading_zeros = size * 2 - strlen(buf);
	for (size_t i = 0; i < leading_zeros; i++) {
		DebugPort::write("0", m_color);
	}
	DebugPort::write(buf, m_color);
}