#pragma once

#include "Filesystem/FSNode.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/String.h"
#include "utils/types.h"

class INode : public FSNode
{
  private:
	String m_filename;
	List<INode> m_children;
	char* m_data;
	Spinlock m_lock;

  public:
	explicit INode(const StringView& name, FSNode::NodeType type, size_t size, char* data);
	INode& operator=(const INode& other) = delete;
	INode& operator=(INode&& other) = delete;
	~INode();
	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<void> mkdir(const StringView& name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

	friend class TarFS;
};