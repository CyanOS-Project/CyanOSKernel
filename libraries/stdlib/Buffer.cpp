#include "Buffer.h"
#include "BufferView.h"

Buffer::Buffer(size_t size) : BufferMutableView{new u8[size]{}, size} {}

Buffer::Buffer(const Buffer& other, size_t dest_offset) :
    BufferMutableView{new u8[other.m_size + dest_offset], other.m_size + dest_offset}
{
	ASSERT(this != &other);

	memcpy(m_data + dest_offset, other.m_data, other.m_size);
}

Buffer::Buffer(const BufferView& other, size_t dest_offset) :
    BufferMutableView{new u8[other.size() + dest_offset], other.size() + dest_offset}
{
	other.copy_to(m_data + dest_offset, 0, other.size());
}

Buffer::Buffer(Buffer&& other) : BufferMutableView{other.m_data, other.m_size}
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
	m_data = nullptr;
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