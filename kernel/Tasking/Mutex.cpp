#include "Mutex.h"

Mutex::Mutex() : m_lock(1)
{
}

Mutex::~Mutex()
{
}

void Mutex::acquire()
{
	m_lock.acquire();
}

void Mutex::release()
{
	m_lock.release();
}
