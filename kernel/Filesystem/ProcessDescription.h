#pragma once
#include <Result.h>
#include <Rule5.h>
#include <UniquePointer.h>

class Process;
class ProcessDescription
{
  private:
	explicit ProcessDescription(Process& process);
	Process& m_process;

  public:
	NON_COPYABLE(ProcessDescription);
	NON_MOVABLE(ProcessDescription);
	static Result<UniquePointer<ProcessDescription>> open(size_t pid, int access);
	~ProcessDescription();
	Result<int> wait_signal();
	Result<void> terminate_process(int status);

	friend class UniquePointer<ProcessDescription>;
};