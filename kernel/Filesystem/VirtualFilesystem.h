#pragma once

#include "Arch/x86/panic.h"
#include "FSNode.h"
#include "FileDescriptor.h"
#include "utils/PathParser.h"
#include "utils/assert.h"
#include "utils/types.h"

class VFS
{
  private:
	static FSNode* m_root;
	static FSNode* traverse_parent_node(const char* path);
	static FSNode* traverse_node(const char* path);

  public:
	VFS();
	~VFS();
	static FileDescriptor* open(const char* path, int mode, int flags);
	static int mount();
	static int mount_root();
	static int unmount();
	static int remove();
	static int make_directory();
	static int remove_directory();
	static int rename();
	static int chown();
	static int make_link();
	static int remove_link();
};
