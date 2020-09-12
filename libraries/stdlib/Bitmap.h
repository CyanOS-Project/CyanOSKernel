#pragma once

#include "Types.h"

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
