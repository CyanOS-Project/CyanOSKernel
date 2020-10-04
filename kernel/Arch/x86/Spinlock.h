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
	uint32_t m_value;
	uint32_t m_eflags;
};
