#pragma once
#include "Rule5.h"
#include "UniquePointer.h"

#ifdef __UNIT_TESTS
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#define ASSERT(x) assert(x)
#else
	#include "Clib.h"
	#include "Assert.h"
#endif
// TODO: implement find and set function.
#define MAX_m_bitmap_size     24576
#define CHECK_BIT(value, bit) ((value >> bit) & 1)
#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

class Bitmap
{
  private:
	const size_t m_bitmap_size;
	UniquePointer<u8[]> m_bitmap_data;

  public:
	NON_COPYABLE(Bitmap)
	NON_MOVABLE(Bitmap)
	Bitmap(size_t m_bitmap_size);
	~Bitmap();
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