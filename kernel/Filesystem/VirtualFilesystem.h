#pragma once

#include "Arch/x86/panic.h"
#include "FSNode.h"
#include "FileDescriptor.h"
#include "pipes/Pipe.h"
#include "ustar/INode.h"
#include "utils/ErrorCodes.h"
#include "utils/PathParser.h"
#include "utils/Result.h"
#include "utils/assert.h"
#include "utils/types.h"

#define MAX_FILE_NAME 64
class VFS
{
  private:
	static FSNode* m_root;
	static Result<FSNode&> traverse_parent_node(const char* path);
	static Result<FSNode&> traverse_node(const char* path);
	static Result<FSNode&> traverse_node_deep(PathParser& parser, size_t depth);

  public:
	VFS();
	~VFS();
	static Result<FileDescriptor*> open(const char* path, int mode, int flags);
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
