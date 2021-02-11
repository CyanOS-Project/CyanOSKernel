#pragma once

#include <Rule5.h>
#include <Types.h>

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
