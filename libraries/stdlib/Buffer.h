#pragma once

#include "Assert.h"
#include "TypeTraits.h"
#include "Types.h"

class BufferView;

class Buffer
{

  public:
	Buffer(size_t size);
	Buffer(u8* data, size_t size);
	Buffer(const Buffer&, size_t dest_offset = 0);
	Buffer(const BufferView&, size_t dest_offset = 0);
	Buffer(Buffer&&);
	Buffer& operator=(const Buffer&);
	Buffer& operator=(Buffer&&);
	~Buffer();

	void resize(size_t size_t);
	void fill_from(const void* src, size_t dest_offset, size_t size);
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

  private:
	u8* m_data;
	size_t m_size;
};
