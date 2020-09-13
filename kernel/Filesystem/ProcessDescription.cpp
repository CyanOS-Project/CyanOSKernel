#include "ProcessDescription.h"

Result<UniquePointer<ProcessDescription>> ProcessDescription::open(size_t pid, int access)
{
	auto result = Process::get_process_from_pid(pid);
	if (result.is_error())
		return ResultError(result.error());

	return UniquePointer<ProcessDescription>::make_unique(result.value());
}

ProcessDescription::ProcessDescription(const Process& process) : m_process{process} {}

ProcessDescription::~ProcessDescription() {}

int ProcessDescription::wait_signal() {}