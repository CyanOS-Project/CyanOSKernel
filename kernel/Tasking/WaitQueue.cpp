#include "WaitQueue.h"

WaitQueue::WaitQueue()
{
}

void WaitQueue::enqueue(Thread& thread)
{
	m_threads.push_back(&thread);
}

void WaitQueue::wake_up()
{
	auto* thread_to_wake = m_threads.head();
	m_threads.pop_back();
	thread_to_wake->wake_up();
}

void WaitQueue::wake_up(size_t num)
{
	while ((num--) && (!m_threads.is_empty())) {
		wake_up();
	}
}

void WaitQueue::wake_up_all()
{
	while (!m_threads.is_empty()) {
		wake_up();
	}
}