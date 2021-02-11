#include "Buffer.h"
#include "BufferView.h"
#include "Clib.h"

Buffer::Buffer(size_t size) : m_data{new u8[size]}, m_size{size} {}

Buffer::Buffer(u8* data, size_t size) : m_data{new u8[size]}, m_size{size}
{
	memcpy(m_data, data, m_size);
}

Buffer::Buffer(const Buffer& other, size_t dest_offset) :
    m_data{new u8[other.m_size + dest_offset]},
    m_size{other.m_size + dest_offset}
{
	ASSERT(this != &other);

	memcpy(m_data + dest_offset, other.m_data, other.m_size);
}

Buffer::Buffer(const BufferView& other, size_t dest_offset) :
    m_data{new u8[other.size() + dest_offset]},
    m_size{other.size() + dest_offset}
{
	other.copy_to(m_data + dest_offset, 0, other.size());
}

Buffer::Buffer(Buffer&& other) : m_data{other.m_data}, m_size{other.m_size}
{
	ASSERT(this != &other);

	other.m_data = nullptr;
}

Buffer& Buffer::operator=(const Buffer& other)
{
	ASSERT(this != &other);

	delete[] m_data;

	m_data = new u8[other.m_size];
	m_size = other.m_size;
	memcpy(m_data, other.m_data, m_size);

	return *this;
}

Buffer& Buffer::operator=(Buffer&& other)
{
	ASSERT(this != &other);

	delete[] m_data;

	m_data = other.m_data;
	m_size = other.m_size;

	other.m_data = nullptr;

	return *this;
}

Buffer::~Buffer()
{
	delete[] m_data;
}

void Buffer::resize(size_t new_size)
{
	ASSERT(new_size > m_size);
	if (new_size == m_size) {
		return;
	}

	u8* new_data = new u8[new_size];
	memcpy(new_data, m_data, m_size);

	delete[] m_data;

	m_data = new_data;
	m_size = new_size;
}

void Buffer::fill_from(const void* src, size_t dest_offset, size_t size)
{
	ASSERT((dest_offset + size) <= m_size);

	memcpy(m_data + dest_offset, src, size);
}

u8* Buffer::ptr()
{
	return m_data;
}

u8& Buffer::operator[](size_t index)
{
	return m_data[index];
}

void Buffer::copy_to(void* dest, size_t src_offset, size_t size) const
{
	ASSERT((src_offset + size) <= m_size);

	memcpy(dest, m_data + src_offset, size);
}

const u8* Buffer::ptr() const
{
	return m_data;
}

u8 Buffer::operator[](size_t index) const
{
	return m_data[index];
}

size_t Buffer::size() const
{
	return m_size;
}
