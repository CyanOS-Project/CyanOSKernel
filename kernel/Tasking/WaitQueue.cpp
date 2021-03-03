#include "WaitQueue.h"
#include "ScopedLock.h"
#include "Thread.h"
#include <TypeTraits.h>

WaitQueue::WaitQueue() : m_lock{UniquePointer<Spinlock>::make_unique()}, m_queue{} {}

WaitQueue::WaitQueue(WaitQueue&& other) : m_lock{move(other.m_lock)}, m_queue{move(other.m_queue)}
{
	other.is_moved = true;
}

WaitQueue& WaitQueue::operator=(WaitQueue&& other)
{
	m_lock = move(other.m_lock);
	m_queue = move(other.m_queue);
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

void WaitQueue::wake_up()
{
	ScopedLock local_lock(*m_lock);

	auto blocked_queue = m_queue.find_if([](auto& i) { return i.state == State::Blocked; });
	ASSERT(blocked_queue != m_queue.end());
	wake_up_queue(*blocked_queue);
}

void WaitQueue::wake_up_all()
{
	ScopedLock local_lock(*m_lock);

	auto find_first_blocked_queue = [](auto& i) { return i.state == State::Blocked; };

	auto queue = m_queue.begin();
	while (queue != m_queue.end()) {
		if (queue->state == State::Blocked) {
			wake_up_queue(*queue);
			queue = m_queue.find_if(find_first_blocked_queue);
		}
	}
}

void WaitQueue::handle_thread_timeout(Thread& thread)
{
	ScopedLock local_lock(*m_lock);

	auto queue = m_queue.find_if([tid = thread.tid()](auto& i) { return i.thread->tid() == tid; });
	ASSERT(queue != m_queue.end());

	queue->state = State::Timeout;
}

void WaitQueue::handle_thread_terminated(Thread& thread)
{
	ScopedLock local_lock(*m_lock);

	bool removed = m_queue.remove_if([tid = thread.tid()](auto& i) { return i.thread->tid() == tid; });
	ASSERT(removed);
}

void WaitQueue::wake_up_queue(Queue& blocked_queue)
{
	blocked_queue.state = State::WokenUp;
	blocked_queue.thread->wake_up();
}