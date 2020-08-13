#pragma once

#include "Filesystem/FSNode.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/types.h"

class INode : public FSNode
{
  private:
	char m_filename[MAX_FILE_NAME];
	char* m_data;
	List<INode> m_children;

  public:
	explicit INode(const char* name, size_t size, char* data);
	INode(const INode& other) = default;
	INode& operator=(const INode& other) = delete;
	INode& operator=(INode&& other) = delete;
	~INode();

	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<FSNode&> create(const char* name, OpenMode mode, OpenFlags flags) override;
	Result<void> mkdir(const char* name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const char* file_name) override;

	friend class TarFS;
};