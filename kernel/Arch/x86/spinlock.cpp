#include "spinlock.h"

void Spinlock::init()
{
	m_value = 0;
}

void Spinlock::acquire()
{
	m_eflags = eflags_read();
	DISABLE_INTERRUPTS();
	while (test_and_set(&m_value) != 0) {
	}
}

void Spinlock::release()
{
	m_value = 0;
	eflags_write(m_eflags);
}
