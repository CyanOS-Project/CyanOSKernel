#include "semaphore.h"
/*
Semaphore::Semaphore(int t_max_count) :
    m_max_count(t_max_count),
    m_count(0),
    m_waiting_queue(new CircularQueue<ThreadControlBlock>)
{
    spinlock_init(&m_spinlock);
}

Semaphore::~Semaphore()
{
    delete m_waiting_queue;
}

void Semaphore::acquire()
{
    spinlock_acquire(&m_spinlock);
    if (m_count < m_max_count) {
        m_count++;
        spinlock_release(&m_spinlock);
    } else {
        Scheduler::block_current_thread(ThreadState::BLOCKED_LOCK, m_waiting_queue);
        spinlock_release(&m_spinlock);
        Scheduler::yield();
        acquire();
    }
}

void Semaphore::release()
{
    if (!m_count)
        return;
    spinlock_acquire(&m_spinlock);
    if (!m_waiting_queue->is_empty()) {
        Scheduler::unblock_thread(m_waiting_queue);
        if (!m_waiting_queue->is_empty())
            m_waiting_queue->increment_head();
    }
    m_count--;
    spinlock_release(&m_spinlock);
}
*/