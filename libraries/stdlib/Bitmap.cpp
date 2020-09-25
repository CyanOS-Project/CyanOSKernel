#include "Bitmap.h"

#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif
Bitmap::Bitmap(size_t size) : m_size(size)
{
	ASSERT(size <= MAX_BITMAP_SIZE);
	m_bitmap_data = new uint8_t[size / 8];
	memset(m_bitmap_data, 0, sizeof(size / 8));
}

Bitmap::~Bitmap()
{
	delete m_bitmap_data;
}

void Bitmap::set_used(unsigned position)
{
	set_used(position, 1);
}

void Bitmap::set_used(unsigned position, unsigned count)
{
	if (position > m_size) {
		return;
	}

	uint32_t current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] |= (1 << (current_position % 8));
		current_position++;
	}
}

void Bitmap::set_unused(unsigned position)
{
	set_unused(position, 1);
}

void Bitmap::set_unused(unsigned position, unsigned count)
{
	if (position > m_size) {
		return;
	}

	uint32_t current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] &= ~(1 << (current_position % 8));
		current_position++;
	}
}

unsigned Bitmap::find_first_unused(unsigned count)
{
	size_t remaining_count = count;
	size_t bit_index = 0;
	while (bit_index < m_size) {
		if (m_bitmap_data[bit_index / 8] == 0xFF){
			bit_index += 8;
			continue;
		}

		if (!CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
			remaining_count--;
		else
			remaining_count = count;

		if (!remaining_count)
			return bit_index - count + 1;
		bit_index++;
	}
	return BITMAP_NO_BITS_LEFT;
}
unsigned Bitmap::find_first_used(unsigned count)
{
	size_t remaining_count = count;
	size_t bit_index = 0;
	while (bit_index < m_size) {
		if (m_bitmap_data[bit_index / 8] == 0){
			bit_index += 8;
			continue;
		}
		if (CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
			remaining_count--;
		else
			remaining_count = count;

		if (!remaining_count)
			return bit_index - count + 1;
		bit_index++;
	}
	return BITMAP_NO_BITS_LEFT;
}
unsigned Bitmap::find_first_unused()
{
	return find_first_unused(1);
}
unsigned Bitmap::find_first_used()
{
	return find_first_used(1);
}