#pragma once

#include "assert.h"
#include "rule5.h"
#include "type_traits.h"
#include "types.h"

class BufferMutableView
{

  public:
	DEFAULT_CONSTRUCTOR(BufferMutableView)
	DEFAULT_DESTRUCTOR(BufferMutableView)

	BufferMutableView& operator=(const BufferMutableView&) = default;
	BufferMutableView(const BufferMutableView&, size_t src_offset = 0, size_t src_size = 0);
	BufferMutableView(void*, size_t);

	void fill_from(const void* src, size_t dest_offset, size_t size);
	template <typename T> void fill_by(T& src, size_t dest_offset)
	{
		ASSERT((dest_offset + sizeof(T)) <= m_size);
		memcpy(m_data + dest_offset, &src, sizeof(T));
	}
	u8* ptr();
	u8& operator[](size_t index);

	template <typename T> T& convert_to()
	{
		ASSERT(sizeof(T) <= m_size);

		return *reinterpret_cast<T*>(m_data);
	}

	void copy_to(void* dest, size_t src_offset, size_t size) const;
	const u8* ptr() const;
	u8 operator[](size_t index) const;
	size_t size() const;
	template <typename T> const T& const_convert_to() const
	{
		ASSERT(sizeof(T) <= m_size);

		return *reinterpret_cast<const RemoveConst<T>*>(m_data);
	}

  protected:
	u8* m_data{nullptr};
	size_t m_size{0};
};