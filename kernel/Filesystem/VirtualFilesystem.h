#pragma once

#include "Arch/x86/panic.h"
#include "FileDescriptor.h"
#include "utils/assert.h"
#include "utils/types.h"

class VFS
{
  private:
  public:
	VFS();
	~VFS();
	static int mount();
	static int mount_root();
	static int unmount();
	static int open(const char* path, int mode, int flags);
	static int remove();
	static int make_directory();
	static int remove_directory();
	static int rename();
	static int chown();
	static int make_link();
	static int remove_link();
};
