#pragma once

#include "Assert.h"
#include "BufferMutableView.h"
#include "Clib.h"
#include "TypeTraits.h"
#include "Types.h"

class BufferView;

class Buffer : public BufferMutableView
{

  public:
	Buffer(size_t size);
	Buffer(const Buffer&, size_t dest_offset = 0);
	Buffer(const BufferView&, size_t dest_offset = 0);
	Buffer(Buffer&&);
	Buffer& operator=(const Buffer&);
	Buffer& operator=(Buffer&&);
	~Buffer();

	void resize(size_t size_t);
};
