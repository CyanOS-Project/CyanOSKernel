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
	static Result<FileDescriptor> open(const char* path, int mode, int flags);
	static Result<void> mount();
	static Result<void> mount_root(FSNode& node);
	static Result<void> unmount();
	static Result<void> remove();
	static Result<void> make_directory();
	static Result<void> remove_directory();
	static Result<void> rename();
	static Result<void> chown();
	static Result<void> make_link();
	static Result<void> remove_link();
};
