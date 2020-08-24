#pragma once

#include "Filesystem/FSNode.h"
#include "Tasking/SpinLock.h"
#include "Utils/List.h"
#include "Utils/Result.h"
#include "Utils/String.h"
#include "Utils/Types.h"

class INode : public FSNode
{
  private:
	char* m_data;
	Spinlock m_lock;
	List<INode> m_children;

  public:
	explicit INode(const StringView& name, NodeType type, size_t size, char* data);
	INode& operator=(const INode& other) = delete;
	INode& operator=(INode&& other) = delete;
	~INode();
	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

	friend class TarFS;
};