#include "BufferView.h"
#include "Clib.h"

BufferView::BufferView(const Buffer& buff, size_t offset, size_t size) :
    m_data{buff.ptr() + offset},
    m_size{size == 0 ? (buff.size() - offset) : (size)}
{
	ASSERT((size + offset) < buff.size());
}

BufferView::BufferView(const BufferView& other, size_t offset, size_t size) :
    m_data{other.m_data + offset},
    m_size{size == 0 ? (other.m_size - offset) : (size)}
{
	ASSERT((size + offset) < other.m_size);
}

BufferView::BufferView(const void* data, size_t size) : m_data{reinterpret_cast<const u8*>(data)}, m_size{size} {}

BufferView& BufferView::operator=(const Buffer& other)
{
	m_data = other.ptr();
	m_size = other.size();

	return *this;
}

BufferView& BufferView::operator=(const BufferView& other)
{
	m_data = other.m_data;
	m_size = other.m_size;

	return *this;
}

void BufferView::copy_to(void* dest, size_t src_offset, size_t size) const
{
	ASSERT((src_offset + size) <= m_size);

	memcpy(dest, m_data + src_offset, size);
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