#include "buffer_view.h"
#include "buffer_mutable_view.h"
#include "clib.h"

BufferView::BufferView(const Buffer& buff, size_t src_offset, size_t src_size) :
    m_data{buff.ptr() + src_offset},
    m_size{src_size == 0 ? (buff.size() - src_offset) : (src_size)}
{
	ASSERT((src_size + src_offset) <= buff.size());
}

BufferView::BufferView(const BufferView& other, size_t src_offset, size_t src_size) :
    m_data{other.m_data + src_offset},
    m_size{src_size == 0 ? (other.m_size - src_offset) : (src_size)}
{
	ASSERT((src_size + src_offset) <= other.m_size);
}

BufferView::BufferView(const BufferMutableView& other, size_t src_offset, size_t src_size) :
    m_data{other.ptr() + src_offset},
    m_size{src_size == 0 ? (other.size() - src_offset) : (src_size)}
{
	ASSERT((src_size + src_offset) <= other.size());
}

BufferView::BufferView(const void* data, size_t size) : m_data{reinterpret_cast<const u8*>(data)}, m_size{size} {}

void BufferView::copy_to(void* dest, size_t src_offset, size_t src_size) const
{
	ASSERT((src_offset + src_size) <= m_size);

	memcpy(dest, m_data + src_offset, src_size);
}

const u8* BufferView::ptr() const
{
	return m_data;
}

u8 BufferView::operator[](size_t index) const
{
	return m_data[index];
}

size_t BufferView::size() const
{
	return m_size;
}