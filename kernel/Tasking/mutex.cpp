#include "mutex.h"

Mutex::Mutex()
{
	lock = new Semaphore(1);
}

Mutex::~Mutex()
{
	delete lock;
}

void Mutex::acquire()
{
	lock->acquire();
}

void Mutex::release()
{
	lock->release();
}
