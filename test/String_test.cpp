#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#define UNIT_TESTING
#define ASSERT(x) (void)ASSERT_TRUE(false)

#include "utils/String.h"
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(String_Test, Construction)
{
	const char* str_data = "hello world";
	String str = String(str_data);

	EXPECT_EQ(strcmp(str_data, str.c_str()), 0);
	EXPECT_EQ(strlen(str_data), strlen(str.c_str()));
	EXPECT_EQ(strlen(str_data), str.length());
}

TEST(FooTest, DoesThis2)
{
	EXPECT_EQ(true, false);
}

TEST(FooTest, DoesThis3)
{
	EXPECT_EQ(true, true);
}

TEST(FooTest, DoesThis4)
{
	EXPECT_EQ(true, true);
}
