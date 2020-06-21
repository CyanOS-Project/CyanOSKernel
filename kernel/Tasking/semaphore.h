#include "scheduler.h"
#include "utils/list.h"
#include "utils/types.h"

class Semaphore
{
  private:
	unsigned max_count;
	unsigned count;
	SpinLock spinlock;
	CircularList<ThreadControlBlock>* waiting_queue;

  public:
	Semaphore(int _max_count);
	~Semaphore();
	void acquire();
	void release();
};
