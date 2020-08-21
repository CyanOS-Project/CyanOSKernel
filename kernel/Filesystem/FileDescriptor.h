#pragma once

#include "Arch/x86/spinlock.h"
#include "utils/List.h"
#include "utils/types.h"

class FileDescription;
class FileDescriptor
{
  private:
	Spinlock m_lock;
	List<FileDescription> m_file_description_table; // FIXME:should be vector.

  public:
	FileDescriptor();
	~FileDescriptor();
	unsigned add_descriptor(const FileDescription& file_description);
	void remove_descriptor(unsigned);
	FileDescription& get_description(unsigned);
};
