#include "logger.h"
#include <assert.h>
#include <clib.h>

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

Logger& Logger::operator<<(int value)
{
	print_signed(value);
	return *this;
}

Logger& Logger::operator<<(unsigned value)
{
	print_unsigned(value);
	return *this;
}

Logger& Logger::operator<<(int32_t value)
{
	print_signed(value);
	return *this;
}

Logger& Logger::operator<<(u32 value)
{
	print_unsigned(value);
	return *this;
}

Logger& Logger::operator<<(Hex value)
{

	DebugPort::write("0x", m_color);
	print_hex(value.m_data, value.m_size);
	return *this;
}

Logger& Logger::operator<<(Hex8 value)
{
	DebugPort::write("0x", m_color);
	print_hex(value.m_data, 1);
	return *this;
}

Logger& Logger::operator<<(Hex16 value)
{
	DebugPort::write("0x", m_color);
	print_hex(value.m_data, 2);
	return *this;
}

Logger& Logger::operator<<(Hex32 value)
{
	DebugPort::write("0x", m_color);
	print_hex(value.m_data, 4);
	return *this;
}

Logger& Logger::operator<<(Hex64 value)
{
	DebugPort::write("0x", m_color);
	print_hex((value.m_data & 0xFFFFFFFF00000000) >> 32, 4);
	print_hex(value.m_data & 0xFFFFFFFF, 4);

	return *this;
}

Logger& Logger::operator<<(void* ptr)
{
	DebugPort::write("0x", m_color);
	print_hex(uptr(ptr), sizeof(uptr));
	return *this;
}

Logger& Logger::operator<<(MACAddress mac)
{
	u8 mac_data[6];
	mac.copy(mac_data);
	for (size_t i = 0; i < 6; i++) {
		print_hex(mac_data[i], 1);
		if (i != 5) {
			DebugPort::write(":", m_color);
		}
	}
	return *this;
}

Logger& Logger::operator<<(IPv4Address mac)
{
	u8 mac_data[6];
	mac.copy(mac_data);
	for (size_t i = 0; i < 4; i++) {
		print_unsigned(mac_data[i]);
		if (i != 3) {
			DebugPort::write(".", m_color);
		}
	}
	return *this;
}

void Logger::print_hex(u32 value, u8 size)
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

void Logger::print_unsigned(unsigned value)
{
	char buf[11];
	itoa(buf, value, 10);
	DebugPort::write(buf, m_color);
}

void Logger::print_signed(int value)
{
	char buf[11];
	if (value < 0) {
		value = 0 - value;
		DebugPort::write("-", m_color);
	}

	itoa(buf, value, 10);
	DebugPort::write(buf, m_color);
}