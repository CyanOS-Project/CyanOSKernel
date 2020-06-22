#include "semaphore.h"

class Mutex
{
  private:
	Semaphore* m_lock;

  public:
	Mutex();
	~Mutex();
	void acquire();
	void release();
};