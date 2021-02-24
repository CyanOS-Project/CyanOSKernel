#include "Semaphore.h"
#include "ScopedLock.h"
#include <Clib.h>

Semaphore::Semaphore(int initial_value) : m_lock{UniquePointer<Spinlock>::make_unique()}, m_count{initial_value} {}

Semaphore::Semaphore(Semaphore&& other) :
    m_lock{move(other.m_lock)},
    m_count{other.m_count},
    m_queue{move(other.m_queue)}
{
	other.is_moved = true;
}

Semaphore& Semaphore::operator=(Semaphore&& other)
{
	m_lock = move(other.m_lock);
	m_count = other.m_count;
	m_queue = move(other.m_queue);

	other.is_moved = true;
	return *this;
}

Semaphore::~Semaphore() {}

void Semaphore::acquire()
{
	ScopedLock local_lock(*m_lock);

	m_count--;
	m_queue.wait(local_lock);
}

void Semaphore::release()
{
	ScopedLock local_lock(*m_lock);

	if (m_count < 0) {
		m_queue.wake_up();
	}
	m_count++;
}
