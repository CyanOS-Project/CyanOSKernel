
#include "Bitmap.h"

Bitmap::Bitmap(size_t m_bitmap_size) :
    m_bitmap_size{m_bitmap_size},
    m_bitmap_data{UniquePointer<u8[]>::make_unique((m_bitmap_size / 8) + 1, u8{0})}
{
}

Bitmap::~Bitmap() {}

void Bitmap::set(size_t position)
{
	set_range(position, 1);
}

void Bitmap::set_range(size_t position, size_t count)
{
	if (position > m_bitmap_size) {
		return;
	}

	u32 current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] |= (1 << (current_position % 8));
		current_position++;
	}
}

void Bitmap::clear(size_t position)
{
	clear_range(position, 1);
}

void Bitmap::clear_range(size_t position, size_t count)
{
	if (position > m_bitmap_size) {
		return;
	}

	u32 current_position = position;
	for (size_t i = 0; i < count; i++) {
		m_bitmap_data[current_position / 8] &= ~(1 << (current_position % 8));
		current_position++;
	}
}

size_t Bitmap::find_first_clear_range(size_t count, size_t start_search) const
{
	size_t remaining_count = count;
	size_t bit_index = start_search;
	while (bit_index < m_bitmap_size) {
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

size_t Bitmap::find_first_set_range(size_t count, size_t start_search) const
{
	size_t remaining_count = count;
	size_t bit_index = start_search;
	while (bit_index < m_bitmap_size) {
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

size_t Bitmap::find_first_clear(size_t start_search) const
{
	return find_first_clear_range(1, start_search);
}

size_t Bitmap::find_first_set(size_t start_search) const
{
	return find_first_set_range(1, start_search);
}

bool Bitmap::check_set(size_t position) const
{
	return CHECK_BIT(m_bitmap_data[position / 8], position % 8);
}

bool Bitmap::check_clear(size_t position) const
{
	return !CHECK_BIT(m_bitmap_data[position / 8], position % 8);
}