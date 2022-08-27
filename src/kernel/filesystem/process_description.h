#pragma once
#include <result.h>
#include <rule5.h>
#include <unique_pointer.h>

class Process;
class ProcessDescription
{
  public:
	NON_COPYABLE(ProcessDescription)
	NON_MOVABLE(ProcessDescription)
	static Result<UniquePointer<ProcessDescription>> open(size_t pid, int access);
	~ProcessDescription();
	Result<int> wait_signal();
	Result<void> terminate_process(int status);

  private:
	explicit ProcessDescription(Process& process);
	Process& m_process;

	friend class UniquePointer<ProcessDescription>;
};