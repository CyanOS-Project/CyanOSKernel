#pragma once

#include "FSNode.h"
#include "FileDescription.h"
#include "FileDescriptor.h"
#include "utils/List.h"
#include "utils/PathParser.h"
#include "utils/Result.h"
#include "utils/UniquePointer.h"
#include "utils/types.h"

class VFS
{
  private:
	static List<UniquePointer<FSNode>>* fs_roots;
	static Spinlock lock;

	static Result<FSNode&> traverse_parent_node(const StringView& path);
	static Result<FSNode&> traverse_node(const StringView& path);
	static Result<FSNode&> traverse_node_deep(PathParser& parser, size_t depth);
	static Result<FSNode&> open_node(const StringView& path, OpenMode mode, OpenFlags flags);
	static FSNode* get_root_node(const StringView& root_name);

  public:
	static void setup();
	static Result<UniquePointer<FileDescription>> open(const StringView& path, OpenMode mode, OpenFlags flags);
	static Result<void> mount(UniquePointer<FSNode>&& fs_root);
	static Result<void> unmount();
	static Result<void> remove();
	static Result<void> make_directory();
	static Result<void> remove_directory();
	static Result<void> rename();
	static Result<void> chown();
	static Result<void> make_link();
	static Result<void> remove_link();
};
