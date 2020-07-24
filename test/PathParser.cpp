#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include "utils/PathParser.h"
#include <gtest/gtest.h>

TEST(PathParser_Test, ElementsCounting)
{
	PathParser path("/mnt/c/user/goku/rocks");
	EXPECT_EQ(path.path_element_count(), 5);
}

TEST(PathParser_Test, GettingElements)
{
	PathParser path("/mnt/fat32/c/users/goku/rocks");
	char element[MAX_PATH];

	EXPECT_EQ(path.get_element(0, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "mnt"), 0);

	EXPECT_EQ(path.get_element(1, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "fat32"), 0);

	EXPECT_EQ(path.get_element(2, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "c"), 0);

	EXPECT_EQ(path.get_element(3, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "users"), 0);

	EXPECT_EQ(path.get_element(4, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "goku"), 0);

	EXPECT_EQ(path.get_element(5, element, MAX_PATH), 0);
	EXPECT_EQ(strcmp(element, "rocks"), 0);
}