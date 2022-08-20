#pragma once

#include "Arch/x86/Spinlock.h"
#include <Rule5.h>

class Spinlock : public StaticSpinlock
{
  public:
	NON_COPYABLE(Spinlock)
	NON_MOVABLE(Spinlock)

	Spinlock() : StaticSpinlock{} { init(); }
	~Spinlock() = default;
};
