#pragma once

#include "FSNode.h"
#include "Tasking/SpinLock.h"
#include <List.h>
#include <PathParser2.h>
#include <Result.h>
#include <Types.h>
#include <UniquePointer.h>

class FileDescription;
class VFS
{
  public:
	static void setup();
	static Result<void> mount(UniquePointer<FSNode>&& fs_root);
	static Result<void> unmount();
	static Result<void> remove();
	static Result<void> make_directory();
	static Result<void> remove_directory();
	static Result<void> rename();
	static Result<void> chown();
	static Result<void> make_link();
	static Result<void> remove_link();
	static PathParser2 traverse_path(const StringView& path);

  private:
	static List<UniquePointer<FSNode>>* fs_roots;
	static Spinlock lock;

	static Result<FSNode&> traverse_parent_node(const PathParser2& parser);
	static Result<FSNode&> traverse_node(const PathParser2& parser);
	static Result<FSNode&> traverse_node_deep(const PathParser2& parser, size_t depth);
	static Result<FSNode&> get_node(const StringView& path, OpenMode mode, OpenFlags flags);
	static Result<FSNode&> create_new_node(const StringView& path, OpenMode mode, OpenFlags flags);
	static Result<FSNode&> open_existing_node(const StringView& path);
	static FSNode* get_root_node(const StringView& root_name);

	friend class FileDescription;
};
