#include "Spinlock.h"

void StaticSpinlock::init()
{
	m_value = 0;
}

void StaticSpinlock::acquire()
{
	uint32_t temp_eflags = eflags_read();
	DISABLE_INTERRUPTS();
	while (test_and_set(&m_value) != 0) {
	}
	m_eflags = temp_eflags;
}

void StaticSpinlock::release()
{
	uint32_t temp_eflags = m_eflags;
	m_value = 0;
	eflags_write(temp_eflags);
}
