#pragma once

#include "fsnode.h"
#include "tasking/spin_lock.h"
#include <list.h>
#include <path_view.h>
#include <result.h>
#include <types.h>
#include <unique_pointer.h>

class FileDescription;
class VFS
{
  public:
	static Result<void> mount(UniquePointer<FSNode>&& fs_root);
	static Result<void> unmount();
	static bool check_exitsts(PathView path);
	static PathView resolve_path(PathView path);

  private:
	static List<UniquePointer<FSNode>> fs_roots;
	static Spinlock lock;

	static Result<FSNode&> traverse_node(PathView);
	static Result<FSNode&> get_node(PathView, OpenMode mode, OpenFlags flags);
	static Result<FSNode&> create_new_node(PathView, OpenMode mode, OpenFlags flags);
	static Result<FSNode&> open_existing_node(PathView);
	static FSNode* get_root_node(StringView root_name);

	friend class FileDescription;
};
