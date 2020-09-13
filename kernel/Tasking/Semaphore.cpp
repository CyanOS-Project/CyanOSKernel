#include "Semaphore.h"
#include "ScopedLock.h"
#include <Clib.h>

Semaphore::Semaphore(size_t t_max_count, size_t t_initial_value) :
    m_lock{},
    m_max_count{t_max_count},
    m_count{t_initial_value}
{
	m_lock.init();
}

Semaphore::~Semaphore() {}

void Semaphore::acquire()
{
	ScopedLock local_lock(m_lock);

	if (m_count < m_max_count) {
		m_count++;
	} else {
		m_queue.wait(local_lock);
	}
}

void Semaphore::release()
{
	if (!m_count)
		return;
	m_lock.acquire();
	m_queue.wake_up();
	m_count--;
	m_lock.release();
}
