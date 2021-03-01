#include "WaitQueue.h"
#include "ScopedLock.h"
#include "Thread.h"
#include <TypeTraits.h>

WaitQueue::WaitQueue() : m_lock{UniquePointer<Spinlock>::make_unique()}, m_threads{} {}

WaitQueue::WaitQueue(WaitQueue&& other) : m_lock{move(other.m_lock)}, m_threads{move(other.m_threads)}
{
	other.is_moved = true;
}

WaitQueue& WaitQueue::operator=(WaitQueue&& other)
{
	m_lock = move(other.m_lock);
	m_threads = move(other.m_threads);
	other.is_moved = true;
	return *this;
}

WaitQueue::~WaitQueue()
{
	if (is_moved) {
		return;
	}

	wake_up_all(); // FIXME: is it okay to do so ?
}

void WaitQueue::terminate_blocked_thread(Thread& thread)
{
	ScopedLock local_lock(*m_lock);

	m_threads.remove_if([&thread](auto i) { return i->tid() == thread.tid(); });
}

void WaitQueue::wake_up(size_t num)
{
	ScopedLock local_lock(*m_lock);

	while ((num--) && (!m_threads.is_empty())) {
		wake_up_one();
	}
}

void WaitQueue::wake_up_all()
{
	ScopedLock local_lock(*m_lock);

	while (!m_threads.is_empty()) {
		wake_up_one();
	}
}

void WaitQueue::wake_up_one()
{
	if (!m_threads.size()) {
		return;
	}
	m_threads.head()->wake_up();
	m_threads.pop_front();
}