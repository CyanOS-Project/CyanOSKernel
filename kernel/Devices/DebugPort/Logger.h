#pragma once

#include "Arch/x86/Spinlock.h"
#include "DebugPort.h"
#include <IPv4Address.h>
#include <MACAddress.h>
#include <String.h>
#include <StringView.h>
#include <Types.h>

class Hex;
class Hex8;
class Hex16;
class Hex32;
class Hex64;

class Logger
{
  public:
	static void init();
	Logger(DebugColor color = DebugColor::White);
	~Logger();
	Logger& operator<<(const char* str);
	Logger& operator<<(const String& str);
	Logger& operator<<(char str);
	Logger& operator<<(int value);
	Logger& operator<<(unsigned value);
	Logger& operator<<(int32_t value);
	Logger& operator<<(uint32_t value);
	Logger& operator<<(Hex value);
	Logger& operator<<(Hex8 value);
	Logger& operator<<(Hex16 value);
	Logger& operator<<(Hex32 value);
	Logger& operator<<(Hex64 value);
	Logger& operator<<(void* ptr);
	Logger& operator<<(const MACAddress& mac);
	Logger& operator<<(const IPv4Address& ip);

  private:
	static StaticSpinlock lock;
	void print_hex(uint32_t value, uint8_t size = 0);
	void print_unsigned(unsigned value);
	void print_signed(int value);

  protected:
	DebugColor m_color;
};

class dbg : public Logger
{
  public:
	dbg() : Logger{DebugColor::Bright_Blue} {}
};

class info : public Logger
{
  public:
	info() : Logger{DebugColor::Green} {}
};

class warn : public Logger
{
  public:
	warn() : Logger{DebugColor::Yellow} {}
};

class err : public Logger
{
  public:
	err() : Logger{DebugColor::Red} {}
};

class Hex
{
  private:
	uint32_t m_data;
	int m_size;

  public:
	Hex(uint8_t value) : m_data(value), m_size{1} {}
	Hex(uint16_t value) : m_data(value), m_size{2} {}
	Hex(uint32_t value) : m_data(value), m_size{4} {}
	Hex(unsigned int value) : m_data(value), m_size{4} {}
	friend class Logger;
};

class Hex8
{
  private:
	int m_data;

  public:
	Hex8(uint8_t value) : m_data(value) {}
	friend class Logger;
};

class Hex16
{
  private:
	int m_data;

  public:
	Hex16(uint16_t value) : m_data(value) {}
	friend class Logger;
};

class Hex32
{
  private:
	int m_data;

  public:
	Hex32(uint32_t value) : m_data(value) {}
	friend class Logger;
};

class Hex64
{
  private:
	long int m_data;

  public:
	Hex64(uint64_t value) : m_data(value) {}
	friend class Logger;
};
