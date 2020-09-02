#include "WaitQueue.h"
#include "ScopedLock.h"

WaitQueue::WaitQueue() : m_lock{}, m_threads{} {}

WaitQueue::~WaitQueue() {}

void WaitQueue::wait_on_event()
{
	ScopedLock local_lock(m_lock);

	Thread::current->block();
	m_threads.push_back(*Thread::current);

	local_lock.release();

	Thread::yield();
}

void WaitQueue::wake_up(size_t num)
{
	ScopedLock local_lock(m_lock);

	while ((num--) && (!m_threads.is_empty())) {
		wake_up_one();
	}
}

void WaitQueue::wake_up_all()
{
	ScopedLock local_lock(m_lock);

	while (!m_threads.is_empty()) {
		wake_up_one();
	}
}

void WaitQueue::wake_up_one()
{
	if (!m_threads.size()) {
		return;
	}
	auto* thread_to_wake = m_threads.pop_front();
	if (thread_to_wake)
		thread_to_wake->wake_up_from_queue();
}