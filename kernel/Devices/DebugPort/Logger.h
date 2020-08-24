#pragma once

#include "Arch/x86/spinlock.h"
#include "DebugPort.h"
#include "utils/String.h"
#include "utils/StringView.h"
#include "utils/types.h"

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
	info()
	{
		m_color = DebugColor::Bright_Blue;
	}
};

class warning : public Logger
{
  public:
	warning()
	{
		m_color = DebugColor::Yellow;
	}
};

class error : public Logger
{
  public:
	error()
	{
		m_color = DebugColor::Red;
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