#pragma once

#include "file_description.h"
#include "filesystem/file_description.h"
#include "filesystem/process_description.h"
#include "tasking/spin_lock.h"
#include <algorithms.h>
#include <list.h>
#include <types.h>
#include <unique_pointer.h>

typedef size_t Handle;
enum class HandleType { Invalid, FileDescription, ProcessDescription };

class HandlesManager
{
	struct HandleEntry {
		HandleType type;
		union {
			FileDescription* file_description;
			ProcessDescription* process_description;
		};
		HandleEntry(HandleType t_type, FileDescription* t_file_description) :
		    type{t_type},
		    file_description{t_file_description}
		{
		}
		HandleEntry(HandleType t_type, ProcessDescription* t_process_description) :
		    type{t_type},
		    process_description{t_process_description}
		{
		}
	};

  public:
	HandlesManager();
	~HandlesManager();
	Handle add_handle(UniquePointer<FileDescription>&& file_description);
	Handle add_handle(UniquePointer<ProcessDescription>&& process_description);
	void remove_handle(Handle);
	FileDescription& get_file_description(Handle);
	ProcessDescription& get_process_description(Handle);
	HandleType check_handle(Handle);

  private:
	Spinlock m_lock;
	List<HandleEntry> handle_table;                                    // FIXME: convert it to a vector.
	List<UniquePointer<FileDescription>> file_description_table;       // TODO: should a vector ?
	List<UniquePointer<ProcessDescription>> process_description_table; // TODO: should a vector ?
};