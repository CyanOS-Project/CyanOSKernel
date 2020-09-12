#pragma once

#include "Filesystem/FSNode.h"
#include "List.h"
#include "Result.h"
#include "String.h"
#include "Tasking/SpinLock.h"
#include "Types.h"

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
	Result<size_t> read(FileDescription&, void* buff, size_t offset, size_t size) override;
	bool can_read(FileDescription&) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;
	Result<StringView> dir_query(size_t index) override;

	friend class TarFS;
};