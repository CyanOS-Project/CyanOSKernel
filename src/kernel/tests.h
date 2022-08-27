#pragma once

#include "devices/debug_port/logger.h"
#include "devices/timer/pit.h"
#include "filesystem/file_description.h"
#include "filesystem/virtual_filesystem.h"
#include "tasking/loader/pe.h"
#include "tasking/scheduler.h"
#include "tasking/semaphore.h"
#include "virtual_memory/memory.h"
#include <argument_parser.h>
#include <elfparser.h>
#include <path_view.h>
#include <static_bitmap.h>

void test_pipe1(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/fs/my_pipe2", OpenMode::OM_READ, OpenFlags::OF_CREATE_NEW);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();

		return;
	}
	char* buff = (char*)valloc(0, 0xc00, PAGE_READWRITE);
	memset(buff, 0, 4096);
	auto result = fd.value()->read(BufferMutableView{buff, 0xc00});
	dbg() << "got it, read";
	dbg() << buff;
	if (result.is_error())
		warn() << "error reading the file " << result.error();
	HLT();
}

void test_pipe2(uintptr_t arg)
{
	UNUSED(arg);
	Thread::current->sleep(1000);
	auto fd = FileDescription::open("/fs/my_pipe", OpenMode::OM_WRITE, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}
	char* buff = (char*)valloc(0, 0xc00, PAGE_READWRITE);
	memset(buff, 0, 4096);
	auto result = fd.value()->write(BufferView{"Hello there", 12});
	dbg() << "got it, write";
	dbg() << buff;
	if (result.is_error())
		warn() << "error writing the file " << result.error();
	HLT();
}

void test_keyboard(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/devices/keyboard", OpenMode::OM_WRITE, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}
	Buffer buff{1};
	while (true) {
		auto result = fd.value()->read(buff);
		Logger(DebugColor::Cyan) << buff[0];
	}
	HLT();
}

void test_keyboard2(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/devices/keyboard", OpenMode::OM_WRITE, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}
	Buffer buff{1};
	while (true) {
		auto result = fd.value()->read(buff);
		Logger(DebugColor::Red) << buff[0];
	}
	HLT();
}

void test_console(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/devices/console", OpenMode::OM_WRITE, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}
	auto result = fd.value()->write(BufferView{"Hello there", 12});
	HLT();
}

void test_server(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/Sockets/test", OpenMode::OM_SERVER, OpenFlags::OF_CREATE_NEW);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}

	auto connection_fd = fd.value()->accept();
	if (connection_fd.is_error()) {
		warn() << "error accepting the connection, error: " << fd.error();
		HLT();
		return;
	}

	auto result = connection_fd.value()->write(BufferView{"I'm the server", 15});

	Buffer buff{0x20};
	auto result2 = connection_fd.value()->read(buff);
	Logger(DebugColor::Bright_Magenta) << "I'm the server, message from server : " << &buff.const_convert_to<char>();
	while (true) {
		HLT();
	}
}

void test_client(uintptr_t arg)
{
	UNUSED(arg);
	Thread::current->sleep(100);
	auto fd = FileDescription::open("/Sockets/test", OpenMode::OM_CLIENT, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}

	// fd.value()->connect();
	if (fd.is_error()) {
		warn() << "error accepting the connection, error: " << fd.error();
		HLT();
		return;
	}

	auto result = fd.value()->write(BufferView{"I'm the client", 15});

	Buffer buff{0x20};
	auto result2 = fd.value()->read(buff);
	Logger(DebugColor::Bright_Magenta) << "I'm the client, message from server : " << &buff.const_convert_to<char>();
	while (true) {
		HLT();
	}
}

void test_ls(uintptr_t arg)
{
	UNUSED(arg);

	auto fd = FileDescription::open("/Tar/UserBinary", OpenMode::OM_READ, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		warn() << "error opening the file, error: " << fd.error();
		HLT();
		return;
	}
	FileInfo dir;
	memset(&dir, 0, sizeof(FileInfo));

	Result<void> result = fd.value()->dir_query(dir);
	if (result.is_error())
		err() << result.error();
	while (!result.is_error()) {
		dbg() << dir.file_name;
		result = fd.value()->dir_query(dir);
	}
	while (true) {
		HLT();
	}
}

void test_tid1(uintptr_t arg)
{
	UNUSED(arg);
	u32 tid, pid;
	char* exe_arg;
	asm("movl %%fs:0x0,%0" : "=r"(tid));
	asm("movl %%gs:0x0,%0" : "=r"(pid));
	asm("movl %%gs:0x8,%0" : "=r"(exe_arg));
	warn() << "PID:" << pid << " TID:" << tid << " Arg: " << exe_arg;
}

void test_tid2(uintptr_t arg)
{
	UNUSED(arg);
	u32 tid, pid;
	char* exe_arg;
	asm("movl %%fs:0x0,%0" : "=r"(tid));
	asm("movl %%gs:0x0,%0" : "=r"(pid));
	asm("movl %%gs:0x8,%0" : "=r"(exe_arg));
	warn() << "PID:" << pid << " TID:" << tid << "Arg: " << exe_arg;
}

void pathparser_test(uintptr_t arg)
{
	UNUSED(arg);
	PathView app_path("/drivers/folder1/folder2/folder3/myapp");

	PathView new_path("test2/test3.exe");
	new_path.set_absolute_path(app_path.sub_path(0, -2));
	warn() << new_path.full_path();
}

void test_elf()
{

	auto fd = FileDescription::open("/tar/bin/shell", OpenMode::OM_READ, OpenFlags::OF_OPEN_EXISTING);
	if (fd.is_error()) {
		err() << "error";
		return;
	}

	FileInfo file_info;
	fd.value()->file_query(file_info);

	Buffer buff{file_info.size};
	auto result = fd.value()->read(buff);

	ELFParser elf(buff);
	if (!elf.is_valid()) {
		info() << "Not valid ELF";
		return;
	}
	info() << elf.elf_header().e_phoff;
	info() << elf.elf_header().e_shoff;

	info() << "Sections:";
	for (size_t i = 0; i < elf.sections_number(); i++) {
		info() << "Virtual Address: " << Hex(elf.section_header_by_index(i).sh_addr)
		       << " Offset: " << Hex(elf.section_header_by_index(i).sh_offset)
		       << " Raw Size: " << Hex(elf.section_header_by_index(i).sh_size)
		       << " Type: " << Hex(elf.section_header_by_index(i).sh_type)
		       << " Name: " << elf.lookup_for_string(elf.section_header_by_index(i).sh_name);
	}

	info() << "Segments:";
	for (size_t i = 0; i < elf.programs_number(); i++) {
		info() << "Virtual Address: " << Hex(elf.program_header_by_index(i).p_vaddr)
		       << " Raw Size: " << Hex(elf.program_header_by_index(i).p_memsz)
		       << " Offset: " << Hex(elf.program_header_by_index(i).p_offset)
		       << " Raw Size: " << Hex(elf.program_header_by_index(i).p_filesz);
	}
}