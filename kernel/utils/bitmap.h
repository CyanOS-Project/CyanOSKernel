#include "Arch/x86/panic.h"
#include "assert.h"
#include "types.h"

#define MAX_BITMAP_SIZE       24576
#define CHECK_BIT(value, bit) ((value >> bit) & 1)
#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

class Bitmap
{
  private:
	uint8_t* m_bitmap_data;
	size_t m_size;

  public:
	Bitmap(size_t size);
	~Bitmap();
	void set_used(unsigned position);
	void set_unused(unsigned position);
	void set_used(unsigned position, unsigned count);
	void set_unused(unsigned position, unsigned count);
	unsigned find_first_unused(unsigned count);
	unsigned find_first_used(unsigned count);
	unsigned find_first_unused();
	unsigned find_first_used();
};

Bitmap::Bitmap(size_t size) : m_size(size)
{
	if (size < MAX_BITMAP_SIZE) {
		m_bitmap_data = new uint8_t[size / 8];
	} else {
		m_bitmap_data = nullptr;
		PANIC("size is higher than MAX_BITMAP_SIZE");
	}
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
	uint32_t first_found = 0;
	uint32_t remaining_count = count;
	for (size_t i = 0; i < sizeof(m_bitmap_data); i++) {
		if ((m_bitmap_data[i] == 0xFF) && (remaining_count != count)) {
			remaining_count = count;
		} else {
			for (size_t bit = 0; bit < 8; bit++) {
				if (!CHECK_BIT(m_bitmap_data[i], bit)) {
					if (remaining_count == count)
						first_found = bit + i * 8;
					if (!(--remaining_count))
						return first_found;
				} else if (remaining_count != count) {
					remaining_count = count;
				}
			}
		}
	}
	return BITMAP_NO_BITS_LEFT;
}
unsigned Bitmap::find_first_used(unsigned count)
{
	uint32_t first_found = 0;
	uint32_t remaining_count = count;
	for (size_t i = 0; i < sizeof(m_bitmap_data); i++) {
		if ((m_bitmap_data[i] == 0xFF) && (remaining_count != count)) {
			remaining_count = count;
		} else {
			for (size_t bit = 0; bit < 8; bit++) {
				if (CHECK_BIT(m_bitmap_data[i], bit)) {
					if (remaining_count == count)
						first_found = bit + i * 8;
					if (!(--remaining_count))
						return first_found;
				} else if (remaining_count != count) {
					remaining_count = count;
				}
			}
		}
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