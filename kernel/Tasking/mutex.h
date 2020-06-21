#include "semaphore.h"

class Mutex
{
  private:
	Semaphore* lock;

  public:
	Mutex();
	~Mutex();
	void acquire();
	void release();
};