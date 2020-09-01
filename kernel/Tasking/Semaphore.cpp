#include "Semaphore.h"
#include "Lib/Stdlib.h"

Semaphore::Semaphore(size_t t_max_count, size_t t_initial_value) :
    m_spinlock{},
    m_max_count{t_max_count},
    m_count{t_initial_value}
{
	m_spinlock.init();
}

Semaphore::~Semaphore() {}

void Semaphore::acquire()
{
	m_spinlock.acquire();

	if (m_count < m_max_count) {
		m_count++;
		m_spinlock.release();
	} else {
		m_spinlock.release();
		m_queue.wait_on_event();
		acquire();
	}
}

void Semaphore::release()
{
	if (!m_count)
		return;
	m_spinlock.acquire();
	m_queue.wake_up();
	m_count--;
	m_spinlock.release();
}
