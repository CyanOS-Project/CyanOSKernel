#pragma once

#include <Types.h>

class StaticSpinlock
{
  public:
	void init();
	void acquire();
	void release();
	StaticSpinlock() = default;
	~StaticSpinlock() = default;

  private:
	u32 m_value;
	u32 m_eflags;
};
