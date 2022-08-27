#pragma once

#include "filesystem/fsnode.h"
#include "tasking/spin_lock.h"
#include <list.h>
#include <types.h>
#include <unique_pointer.h>

class DeviceFS : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc(StringView name);
	static Result<void> add_device(UniquePointer<FSNode>&& device);
	static Result<void> remove_device(StringView name);

	~DeviceFS();
	Result<FSNode&> dir_lookup(StringView file_name) override;
	Result<FSNode&> dir_query(size_t index) override;

  private:
	// static Spinlock m_lock;
	static List<UniquePointer<FSNode>>* children;
	explicit DeviceFS(StringView name);
};
