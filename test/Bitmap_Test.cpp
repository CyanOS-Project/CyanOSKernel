#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include "utils/bitmap.h"
#include <gtest/gtest.h>

TEST(Bitmap_Test, SearchForUsed)
{
	Bitmap bm(10);
	EXPECT_EQ(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(2);
	EXPECT_EQ(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(3, 4);
	EXPECT_EQ(bm.find_first_used(5), 2);
	bm.set_unused(2, 5);
	EXPECT_EQ(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(3);
	bm.set_used(6);
	EXPECT_EQ(bm.find_first_used(4), BITMAP_NO_BITS_LEFT);
}

TEST(Bitmap_Test, SearchForUnused)
{
	Bitmap bm(10);
	EXPECT_EQ(bm.find_first_unused(5), 0);
	bm.set_used(2);
	EXPECT_EQ(bm.find_first_unused(5), 3);
	bm.set_used(3, 4);
	EXPECT_EQ(bm.find_first_unused(5), BITMAP_NO_BITS_LEFT);
	bm.set_unused(2, 5);
	EXPECT_EQ(bm.find_first_unused(5), 0);
	bm.set_used(3);
	bm.set_used(6);
	EXPECT_EQ(bm.find_first_unused(4), BITMAP_NO_BITS_LEFT);
}