#include "Process.h"
#include "Arch/x86/context.h"
#include "Filesystem/VirtualFilesystem.h"
#include "Loader/pe.h"
#include "Thread.h"
#include "VirtualMemory/memory.h"
#include "utils/assert.h"

List<Process>* Process::processes;
Bitmap* Process::pid_bitmap;

void Process::setup()
{
	pid_bitmap = new Bitmap(MAX_BITMAP_SIZE);
	processes = new List<Process>;
}

Process& Process::create_new_process(const char* name, const char* path)
{
	// FIXME:lock it here
	auto& pcb = processes->emplace_back(name, path);
	Thread::create_thread(pcb, initiate_process, uintptr_t(&pcb));
	return pcb;
}

Process::Process(const char* name, const char* path) :
    m_pid{reserve_pid()},
    m_page_directory{Memory::create_new_virtual_space()},
    m_state{ProcessState::ACTIVE},
    m_parent{nullptr}
{
	size_t name_len = strlen(name) + 1;
	size_t path_len = strlen(path) + 1;
	// FIXME: memory leak
	char* proc_name = new char[name_len];
	char* proc_path = new char[path_len];
	memcpy(proc_name, name, name_len);
	memcpy(proc_path, path, path_len);
	m_name = proc_name;
	m_path = proc_path;
	m_parent = nullptr;
}

Process::~Process()
{
	delete[] m_name;
	delete[] m_path;
}

Result<uintptr_t> Process::load_executable(const char* path)
{
	auto fd = VFS::open(path, 0, 0);
	if (fd.is_error()) {
		printf("error opening the file %d\n", fd.error());
		return ResultError(fd.error());
	}
	auto file_info = fd.value().fstat();
	// FIXME: implement smart pointers and use it here.
	char* buff = static_cast<char*>(Memory::alloc(file_info.value().size, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE));
	memset(buff, 0, file_info.value().size);
	auto result = fd.value().read(buff, file_info.value().size);
	if (result.is_error())
		return ResultError(result.error());

	auto execable_entrypoint = PELoader::load(buff, file_info.value().size);
	if (execable_entrypoint.is_error())
		return ResultError(execable_entrypoint.error());
	Memory::free(buff, file_info.value().size, 0);

	return execable_entrypoint.value();
}

unsigned Process::reserve_pid()
{
	unsigned id = pid_bitmap->find_first_unused();
	pid_bitmap->set_used(id);
	return id;
}

void Process::initiate_process(uintptr_t __pcb)
{
	Process* pcb = reinterpret_cast<Process*>(__pcb);
	auto&& executable_entrypoint = pcb->load_executable(pcb->m_path);
	if (executable_entrypoint.is_error())
		return; // Remove thread
	// return ResultError(execable_entrypoint.error());
	void* thread_user_stack = Memory::alloc(STACK_SIZE, MEMORY_TYPE::WRITABLE);
	Context::enter_usermode(executable_entrypoint.value(), uintptr_t(thread_user_stack) + STACK_SIZE);
	ASSERT_NOT_REACHABLE();
}