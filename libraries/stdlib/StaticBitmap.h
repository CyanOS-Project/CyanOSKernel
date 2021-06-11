#pragma once
#include "Rule5.h"

#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#include "Types.h"
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif
// TODO: Do bitmap with allocation.
// TODO: implement find and set function.
#define MAX_BITMAP_SIZE       24576
#define CHECK_BIT(value, bit) ((value >> bit) & 1)
#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

template <size_t bitmap_size> class StaticBitmap
{
  private:
	static constexpr size_t bitmap_buffer_size = (bitmap_size / 8) + 1;
	uint8_t m_bitmap_data[bitmap_buffer_size];
	static_assert(bitmap_size <= MAX_BITMAP_SIZE);

  public:
	NON_COPYABLE(StaticBitmap)
	NON_MOVABLE(StaticBitmap)
	StaticBitmap();
	~StaticBitmap();
	void set(size_t position);
	void set_range(size_t position, size_t count);
	void clear(size_t position);
	void clear_range(size_t position, size_t count);
	size_t find_first_set(size_t start_search = 0) const;
	size_t find_first_set_range(size_t count, size_t start_search = 0) const;
	size_t find_first_clear(size_t start_search = 0) const;
	size_t find_first_clear_range(size_t count, size_t start_search = 0) const;
	bool check_set(size_t position) const;
	bool check_clear(size_t position) const;
};

template <size_t bitmap_size> StaticBitmap<bitmap_size>::StaticBitmap()
{
	memset(m_bitmap_data, 0, bitmap_buffer_size);
}

template <size_t bitmap_size> StaticBitmap<bitmap_size>::~StaticBitmap() {}

template <size_t bitmap_size> void StaticBitmap<bitmap_size>::set(size_t position)
{
	set_range(position, 1);
}

template <size_t bitmap_size> void StaticBitmap<bitmap_size>::set_range(size_t position, size_t count)
{
	if (position > bitmap_size) {
		return;
	}

	u32 current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] |= (1 << (current_position % 8));
		current_position++;
	}
}

template <size_t bitmap_size> void StaticBitmap<bitmap_size>::clear(size_t position)
{
	clear_range(position, 1);
}

template <size_t bitmap_size> void StaticBitmap<bitmap_size>::clear_range(size_t position, size_t count)
{
	if (position > bitmap_size) {
		return;
	}

	u32 current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] &= ~(1 << (current_position % 8));
		current_position++;
	}
}

template <size_t bitmap_size>
size_t StaticBitmap<bitmap_size>::find_first_clear_range(size_t count, size_t start_search) const
{
	size_t remaining_count = count;
	size_t bit_index = start_search;
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

template <size_t bitmap_size>
size_t StaticBitmap<bitmap_size>::find_first_set_range(size_t count, size_t start_search) const
{
	size_t remaining_count = count;
	size_t bit_index = start_search;
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

template <size_t bitmap_size> size_t StaticBitmap<bitmap_size>::find_first_clear(size_t start_search) const
{
	return find_first_clear_range(1, start_search);
}

template <size_t bitmap_size> size_t StaticBitmap<bitmap_size>::find_first_set(size_t start_search) const
{
	return find_first_set_range(1, start_search);
}

template <size_t SIZE> bool StaticBitmap<SIZE>::check_set(size_t position) const
{
	return CHECK_BIT(m_bitmap_data[position / 8], position % 8);
}

template <size_t SIZE> bool StaticBitmap<SIZE>::check_clear(size_t position) const
{
	return !CHECK_BIT(m_bitmap_data[position / 8], position % 8);
}