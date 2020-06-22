#include "mutex.h"

Mutex::Mutex() : m_lock(new Semaphore(1))
{
}

Mutex::~Mutex()
{
	delete m_lock;
}

void Mutex::acquire()
{
	m_lock->acquire();
}

void Mutex::release()
{
	m_lock->release();
}
