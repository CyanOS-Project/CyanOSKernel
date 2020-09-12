#pragma once

#include "Arch/x86/Spinlock.h"
#include "DebugPort.h"
#include "String.h"
#include "StringView.h"
#include "Types.h"

class Hex;

class Logger
{
  private:
	static StaticSpinlock lock;

  protected:
	DebugColor m_color;

  public:
	static void init();
	Logger(DebugColor color = DebugColor::White);
	~Logger();
	Logger& operator<<(const char* str);
	Logger& operator<<(const String& str);
	Logger& operator<<(const char str);
	Logger& operator<<(int num);
	Logger& operator<<(unsigned num);
	Logger& operator<<(uint64_t num);
	Logger& operator<<(Hex num);
	Logger& operator<<(void* ptr);
};

class dbg : public Logger
{
  public:
	dbg() : Logger{DebugColor::Green} {}
};

class info : public Logger
{
  public:
	info() : Logger{DebugColor::Bright_Blue} {}
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
	int m_data;

  public:
	Hex(int num) : m_data(num) {}
	friend class Logger;
};