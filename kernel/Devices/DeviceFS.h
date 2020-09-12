#pragma once

#include "Filesystem/FSNode.h"
#include "List.h"
#include "Tasking/SpinLock.h"
#include "Types.h"
#include "UniquePointer.h"

class DeviceFS : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc(const StringView& name);
	static Result<void> add_device(UniquePointer<FSNode>&& device);
	static Result<void> remove_device(const StringView& name);

	~DeviceFS();
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
	Result<StringView> dir_query(size_t index) override;

  private:
	// static Spinlock m_lock;
	static List<UniquePointer<FSNode>>* children;
	explicit DeviceFS(const StringView& name);
};
