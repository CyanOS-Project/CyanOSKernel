#pragma once

#include "Filesystem/FSNode.h"
#include "Utils/List.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

class DeviceFS : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc(const StringView& name);
	static Result<void> add_device(UniquePointer<FSNode>&& device);
	static Result<void> remove_device(const StringView& name);

	~DeviceFS();
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

  private:
	static List<UniquePointer<FSNode>>* children;
	explicit DeviceFS(const StringView& name);
};
