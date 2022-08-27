#pragma once

#include <rule5.h>
#include <types.h>

class StaticSpinlock
{
  public:
	NON_COPYABLE(StaticSpinlock)
	NON_MOVABLE(StaticSpinlock)

	void init();
	void acquire();
	void release();
	StaticSpinlock() = default;
	~StaticSpinlock() = default;

  private:
	u32 m_value;
	u32 m_eflags;
};
