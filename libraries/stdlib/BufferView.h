#pragma once

#include "Assert.h"
#include "Buffer.h"
#include "BufferView.h"
#include "TypeTraits.h"

class BufferView
{

  public:
	BufferView(const Buffer&, size_t offset = 0, size_t size = 0);
	BufferView(const BufferView&, size_t offset = 0, size_t size = 0);
	BufferView(const void*, size_t);
	BufferView& operator=(const Buffer&);
	BufferView& operator=(const BufferView&);
	~BufferView() = default;

	void copy_to(void* dest, size_t src_offset, size_t size) const;
	const u8* ptr() const;
	u8 operator[](size_t index) const;
	size_t size() const;
	template <typename T> const T& const_convert_to() const
	{
		ASSERT(sizeof(T) <= m_size);

		return *reinterpret_cast<const RemoveConst<T>*>(m_data);
	}

  private:
	const u8* m_data;
	size_t m_size;
};