#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <gtest/gtest.h>
#include <stdlib/PathView.h>

#define MAX_PATH_ELEMENT 256
TEST(PathView_Test, ElementsCounting)
{
	PathView path("/mnt/c/user/goku/rocks");
	EXPECT_EQ(path.count(), 5);
}

TEST(PathView_Test, GettingElements)
{
	PathView path("/mnt/fat32/c/users/goku/rocks");

	EXPECT_TRUE(path[0] == "mnt");
	EXPECT_TRUE(path[1] == "fat32");
	EXPECT_TRUE(path[2] == "c");
	EXPECT_TRUE(path[3] == "users");
	EXPECT_TRUE(path[4] == "goku");
	EXPECT_TRUE(path[5] == "rocks");
}