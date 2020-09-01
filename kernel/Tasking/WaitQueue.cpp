#include "WaitQueue.h"

WaitQueue::WaitQueue() : m_lock{}, m_threads{} {}

WaitQueue::~WaitQueue() {}

void WaitQueue::wait_on_event()
{
	Thread::current->block();
	m_threads.push_back(*Thread::current);
	Thread::yield();
}

void WaitQueue::wake_up()
{
	if (!m_threads.size()) {
		return;
	}
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