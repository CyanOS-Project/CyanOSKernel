#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <ArgumentParser.h>
#include <gtest/gtest.h>

#define MAX_PATH_ELEMENT 256
TEST(ArgumentParser_Test, ElementsCounting)
{
	ArgumentParser path1("cat");
	EXPECT_EQ(path1.count(), 1);

	ArgumentParser path2("cat arg1");
	EXPECT_EQ(path2.count(), 2);

	ArgumentParser path3("cat arg1 arg2 arg3");
	EXPECT_EQ(path3.count(), 4);
}

TEST(ArgumentParser_Test, GettingElements)
{
	ArgumentParser path1("command arg1 arg2 arg3");
	EXPECT_EQ(path1.count(), 4);
	EXPECT_EQ(path1[0], "command");
	EXPECT_EQ(path1[1], "arg1");
	EXPECT_EQ(path1[2], "arg2");
	EXPECT_EQ(path1[3], "arg3");

	ArgumentParser path2("command     arg1     arg2    arg3");
	EXPECT_EQ(path2.count(), 4);
	EXPECT_EQ(path2[0], "command");
	EXPECT_EQ(path2[1], "arg1");
	EXPECT_EQ(path2[2], "arg2");
	EXPECT_EQ(path2[3], "arg3");

	ArgumentParser path3("      command     arg1     arg2    arg3");
	EXPECT_EQ(path3.count(), 4);
	EXPECT_EQ(path3[0], "command");
	EXPECT_EQ(path3[1], "arg1");
	EXPECT_EQ(path3[2], "arg2");
	EXPECT_EQ(path3[3], "arg3");

	ArgumentParser path4("command arg1     arg2    arg3     ");
	EXPECT_EQ(path4.count(), 4);
	EXPECT_EQ(path4[0], "command");
	EXPECT_EQ(path4[1], "arg1");
	EXPECT_EQ(path4[2], "arg2");
	EXPECT_EQ(path4[3], "arg3");

	ArgumentParser path5("   command arg1     arg2    arg3     ");
	EXPECT_EQ(path4.count(), 4);
	EXPECT_EQ(path4[0], "command");
	EXPECT_EQ(path4[1], "arg1");
	EXPECT_EQ(path4[2], "arg2");
	EXPECT_EQ(path4[3], "arg3");
}