#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <StaticBitmap.h>
#include <gtest/gtest.h>

TEST(Bitmap_Test, SearchForUsed)
{
	StaticBitmap<10> bm;
	EXPECT_EQ(bm.find_first_set_range(1), BITMAP_NO_BITS_LEFT);
	EXPECT_EQ(bm.find_first_clear_range(10), 0);
	bm.set(2);
	EXPECT_EQ(bm.find_first_set_range(5), BITMAP_NO_BITS_LEFT);
	bm.set_range(3, 4);
	EXPECT_EQ(bm.find_first_set_range(5), 2);
	bm.clear_range(2, 5);
	EXPECT_EQ(bm.find_first_set_range(5), BITMAP_NO_BITS_LEFT);
	bm.set(3);
	bm.set(6);
	EXPECT_EQ(bm.find_first_set_range(4), BITMAP_NO_BITS_LEFT);
}

TEST(Bitmap_Test, SearchForUnused)
{
	StaticBitmap<10> bm;
	EXPECT_EQ(bm.find_first_clear_range(5), 0);
	bm.set(2);
	EXPECT_EQ(bm.find_first_clear_range(5), 3);
	bm.set_range(3, 4);
	EXPECT_EQ(bm.find_first_clear_range(5), BITMAP_NO_BITS_LEFT);
	bm.clear_range(2, 5);
	EXPECT_EQ(bm.find_first_clear_range(5), 0);
	bm.set(3);
	bm.set(6);
	EXPECT_EQ(bm.find_first_clear_range(4), BITMAP_NO_BITS_LEFT);
}