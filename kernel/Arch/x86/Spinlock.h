#pragma once

#include "Utils/Types.h"

class StaticSpinlock
{
  private:
	uint32_t m_value;
	uint32_t m_eflags;

  public:
	void init();
	void acquire();
	void release();
	StaticSpinlock() = default;
	~StaticSpinlock() = default;
};
