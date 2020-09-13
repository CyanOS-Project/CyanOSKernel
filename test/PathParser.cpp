#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <PathParser.h>
#include <gtest/gtest.h>

#define MAX_PATH_ELEMENT 256
TEST(PathParser_Test, ElementsCounting)
{
	PathParser path("/mnt/c/user/goku/rocks");
	EXPECT_EQ(path.count(), 5);
}

TEST(PathParser_Test, GettingElements)
{
	PathParser path("/mnt/fat32/c/users/goku/rocks");

	EXPECT_TRUE(path.element(0) == "mnt");
	EXPECT_TRUE(path.element(1) == "fat32");
	EXPECT_TRUE(path.element(2) == "c");
	EXPECT_TRUE(path.element(3) == "users");
	EXPECT_TRUE(path.element(4) == "goku");
	EXPECT_TRUE(path.element(5) == "rocks");
}