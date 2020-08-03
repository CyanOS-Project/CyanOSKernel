#include "WaitQueue.h"

WaitQueue::WaitQueue()
{
}

WaitQueue::~WaitQueue()
{
}

void WaitQueue::enqueue(Thread& thread)
{
	m_threads.push_back(thread);
}

void WaitQueue::wake_up()
{
	auto* thread_to_wake = m_threads.pop_front();
	if (thread_to_wake)
		thread_to_wake->wake_up_from_queue();
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