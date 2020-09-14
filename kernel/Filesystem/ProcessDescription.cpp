#include "ProcessDescription.h"
#include "Tasking/Process.h"

Result<UniquePointer<ProcessDescription>> ProcessDescription::open(size_t pid, int access)
{
	UNUSED(access);

	auto result = Process::get_process_from_pid(pid);
	if (result.is_error())
		return ResultError(result.error());

	return UniquePointer<ProcessDescription>::make_unique(result.value());
}

ProcessDescription::ProcessDescription(Process& process) : m_process{process} {}

ProcessDescription::~ProcessDescription() {}

Result<int> ProcessDescription::wait_signal()
{
	return m_process.wait_for_signal();
}

Result<void> ProcessDescription::terminate_process(int status)
{
	m_process.terminate(status);
	return ResultError(ERROR_SUCCESS);
}