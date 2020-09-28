#pragma once

#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif

#define MAX_BITMAP_SIZE       24576
#define CHECK_BIT(value, bit) ((value >> bit) & 1)
#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

template <size_t bitmap_size> class Bitmap
{
  private:
	uint8_t m_bitmap_data[bitmap_size / 8];
	static_assert(bitmap_size <= MAX_BITMAP_SIZE);

  public:
	Bitmap();
	~Bitmap();
	void set(size_t position);
	void set_range(size_t position, size_t count);
	void clear(size_t position);
	void clear_range(size_t position, size_t count);
	size_t find_first_set();
	size_t find_first_set_range(size_t count);
	size_t find_first_clear();
	size_t find_first_clear_range(size_t count);
};

template <size_t bitmap_size> Bitmap<bitmap_size>::Bitmap()
{
	memset(m_bitmap_data, 0, sizeof(bitmap_size / 8));
}

template <size_t bitmap_size> Bitmap<bitmap_size>::~Bitmap() {}

template <size_t bitmap_size> void Bitmap<bitmap_size>::set(unsigned position)
{
	set_range(position, 1);
}

template <size_t bitmap_size> void Bitmap<bitmap_size>::set_range(unsigned position, unsigned count)
{
	if (position > bitmap_size) {
		return;
	}

	uint32_t current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] |= (1 << (current_position % 8));
		current_position++;
	}
}

template <size_t bitmap_size> void Bitmap<bitmap_size>::clear(unsigned position)
{
	clear_range(position, 1);
}

template <size_t bitmap_size> void Bitmap<bitmap_size>::clear_range(unsigned position, unsigned count)
{
	if (position > bitmap_size) {
		return;
	}

	uint32_t current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] &= ~(1 << (current_position % 8));
		current_position++;
	}
}

template <size_t bitmap_size> size_t Bitmap<bitmap_size>::find_first_clear_range(unsigned count)
{
	size_t remaining_count = count;
	size_t bit_index = 0;
	while (bit_index < bitmap_size) {
		if (m_bitmap_data[bit_index / 8] == 0xFF) {
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

template <size_t bitmap_size> size_t Bitmap<bitmap_size>::find_first_set_range(unsigned count)
{
	size_t remaining_count = count;
	size_t bit_index = 0;
	while (bit_index < bitmap_size) {
		if (m_bitmap_data[bit_index / 8] == 0) {
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

template <size_t bitmap_size> size_t Bitmap<bitmap_size>::find_first_clear()
{
	return find_first_clear_range(1);
}

template <size_t bitmap_size> size_t Bitmap<bitmap_size>::find_first_set()
{
	return find_first_set_range(1);
}
