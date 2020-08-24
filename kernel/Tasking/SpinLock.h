#pragma once

#include "Arch/x86/spinlock.h"

class Spinlock : public StaticSpinlock
{
  public:
	Spinlock() : StaticSpinlock{}
	{
		init();
	}
	~Spinlock() = default;
};
