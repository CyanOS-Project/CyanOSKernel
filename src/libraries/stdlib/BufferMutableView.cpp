#include "BufferMutableView.h"
#include "Clib.h"

BufferMutableView::BufferMutableView(const BufferMutableView& other, size_t src_offset, size_t src_size) :
    m_data{other.m_data + src_offset},
    m_size{src_size == 0 ? (other.m_size - src_offset) : (src_size)}
{
	ASSERT((src_size + src_offset) <= other.m_size);
}

BufferMutableView::BufferMutableView(void* data, size_t size) : m_data{reinterpret_cast<u8*>(data)}, m_size{size} {}

void BufferMutableView::fill_from(const void* src, size_t dest_offset, size_t size)
{
	ASSERT((dest_offset + size) <= m_size);
	memcpy(m_data + dest_offset, src, size);
}

u8* BufferMutableView::ptr()
{
	return m_data;
}

u8& BufferMutableView::operator[](size_t index)
{
	ASSERT(index < m_size);
	return m_data[index];
}

void BufferMutableView::copy_to(void* dest, size_t src_offset, size_t size) const
{
	ASSERT((src_offset + size) <= m_size);

	memcpy(dest, m_data + src_offset, size);
}

const u8* BufferMutableView::ptr() const
{
	return m_data;
}

u8 BufferMutableView::operator[](size_t index) const
{
	ASSERT(index < m_size);
	return m_data[index];
}

size_t BufferMutableView::size() const
{
	return m_size;
}
