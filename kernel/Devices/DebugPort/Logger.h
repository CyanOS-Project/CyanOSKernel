#pragma once

#include "DebugPort.h"
#include "Utils/String.h"
#include "Utils/StringView.h"
#include "Utils/Types.h"

class Hex;

class Logger
{
  protected:
	DebugColor m_color;

  public:
	Logger(DebugColor color = DebugColor::White);
	Logger& operator<<(const char* str);
	Logger& operator<<(const String& str);
	Logger& operator<<(int num);
	Logger& operator<<(unsigned num);
	Logger& operator<<(uint64_t num);
	Logger& operator<<(Hex num);
	Logger& operator<<(void* ptr);
};

class dbg : public Logger
{
  public:
	dbg()
	{
		m_color = DebugColor::Green;
	}
};

class info : public Logger
{
  public:
	info() : Logger{DebugColor::Bright_Blue}
	{
	}
};

class warn : public Logger
{
  public:
	warn() : Logger{DebugColor::Yellow}
	{
	}
};

class err : public Logger
{
  public:
	err() : Logger{DebugColor::Red}
	{
	}
};

class Hex
{
  private:
	int m_data;

  public:
	Hex(int num) : m_data(num)
	{
	}
	friend class Logger;
};