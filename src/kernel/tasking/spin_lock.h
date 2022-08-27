#pragma once

#include "arch/x86/spinlock.h"
#include <rule5.h>

class Spinlock : public StaticSpinlock
{
  public:
	NON_COPYABLE(Spinlock)
	NON_MOVABLE(Spinlock)

	Spinlock() : StaticSpinlock{} { init(); }
	~Spinlock() = default;
};
