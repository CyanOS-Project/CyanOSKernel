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
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<void> read(FileDescription&, void* buff, size_t offset, size_t size) override;
	Result<bool> can_read(FileDescription&) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

	friend class TarFS;
};