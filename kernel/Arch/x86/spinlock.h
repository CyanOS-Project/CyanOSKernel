#pragma once

#include "asm.h"
#include "utils/types.h"

class Spinlock
{
  private:
	uint32_t m_value;
	uint32_t m_eflags;

  public:
	void init();
	void acquire();
	void release();
	Spinlock() = default;
	~Spinlock() = default;
};
