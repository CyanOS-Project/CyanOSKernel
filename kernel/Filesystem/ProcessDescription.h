#pragma once
#include "Tasking/Process.h"
#include <Rule5.h>

class ProcessDescription
{
  public:
	NON_COPYABLE(ProcessDescription);
	NON_MOVABLE(ProcessDescription);
	ProcessDescription();
	~ProcessDescription();

  private:
	Process& m_process;
};