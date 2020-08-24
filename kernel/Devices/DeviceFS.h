#pragma once

#include "Filesystem/FSNode.h"
#include "Utils/List.h"
#include "Utils/UniquePointer.h"
#include "Utils/Types.h"

class DeviceFS : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc();
	static Result<void> add_device(UniquePointer<FSNode>&& device);

	DeviceFS();
	~DeviceFS();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<void> mkdir(const StringView& name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

  private:
	static List<UniquePointer<FSNode>>* children;
};
