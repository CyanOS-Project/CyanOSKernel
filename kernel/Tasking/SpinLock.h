#pragma once

#include "Arch/x86/Spinlock.h"

class Spinlock : public StaticSpinlock
{
  public:
	Spinlock() : StaticSpinlock{}
	{
		init();
	}
	~Spinlock() = default;
};
