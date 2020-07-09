#pragma once

#include "utils/types.h"

class FSNode
{
  private:
	int permission;
	int owner;
	size_t size;

  public:
	FSNode();
	~FSNode();
	virtual int read();
	virtual int write();
	virtual int can_read();
	virtual int can_write();
	virtual int dir_lookup();
	virtual int rmdir();
	virtual int mkdir();
};
