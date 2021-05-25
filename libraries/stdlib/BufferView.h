#pragma once

#include "Assert.h"
#include "Buffer.h"
#include "Rule5.h"
#include "TypeTraits.h"

class BufferMutableView;

class BufferView
{

  public:
	DEFAULT_CONSTRUCTOR(BufferView)
	DEFAULT_DESTRUCTOR(BufferView)

	BufferView& operator=(const BufferView&) = default;
	BufferView(const Buffer&, size_t src_offset = 0, size_t src_size = 0);
	BufferView(const BufferView&, size_t src_offset = 0, size_t src_size = 0);
	BufferView(const BufferMutableView&, size_t src_offset = 0, size_t src_size = 0);
	BufferView(const void*, size_t);

	void copy_to(void* dest, size_t src_offset, size_t src_size) const;
	const u8* ptr() const;
	u8 operator[](size_t index) const;
	size_t size() const;
	template <typename T> const T& const_convert_to() const
	{
		ASSERT(sizeof(T) <= m_size);

		return *reinterpret_cast<const RemoveConst<T>*>(m_data);
	}

  private:
	const u8* m_data{nullptr};
	size_t m_size{0};
};