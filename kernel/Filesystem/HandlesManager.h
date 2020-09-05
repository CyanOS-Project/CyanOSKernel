#pragma once

#include "FileDescription.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/SpinLock.h"
#include "Utils/Algorithms.h"
#include "Utils/List.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

typedef size_t Handle;

class HandlesManager
{
	struct HandleEntry {
		bool valid;
		UniquePointer<FileDescription> file_description;
	};

  public:
	HandlesManager();
	~HandlesManager();
	Handle add_file_description(UniquePointer<FileDescription>&& file_description);
	void remove_handle(Handle);
	FileDescription& get_file_description(Handle);
	bool check_handle(Handle);

  private:
	Spinlock m_lock;
	List<HandleEntry> handle_table; // FIXME:should be vector.
};