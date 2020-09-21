#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#define UNIT_TESTING
#define ASSERT(x) (void)ASSERT_TRUE(false)

#include <gtest/gtest.h>
#include <stdlib/StringView.h>

TEST(StringView_Test, Construction)
{
	const char* str_data = "hello world";
	StringView str0 = StringView(str_data, 5);
	EXPECT_TRUE(str0 == "hello");
	EXPECT_EQ(strlen("hello"), str0.length());

	StringView str1 = StringView(str_data);
	EXPECT_TRUE(str1 == str_data);
	EXPECT_EQ(strlen(str_data), str1.length());

	StringView str2{str1};
	EXPECT_TRUE(str2 == str_data);
	EXPECT_EQ(strlen(str_data), str2.length());

	StringView str3{std::move(str1)};
	EXPECT_TRUE(str3 == str_data);
	EXPECT_EQ(strlen(str_data), str2.length());
}

TEST(StringView_Test, Assignment)
{
	const char* str_data = "Hello str0";
	StringView str1 = StringView("Hello str1");
	StringView str2 = StringView("Hello str2");
	StringView str3 = StringView("Hello str3");

	str1 = str_data;
	EXPECT_TRUE(str1 == str_data);
	EXPECT_EQ(strlen(str_data), str1.length());

	str2 = str1;
	EXPECT_TRUE(str1 == str2);
	EXPECT_EQ(str2.length(), str1.length());

	StringView str1_copy{str1};
	str2 = std::move(str1);
	EXPECT_TRUE(str1_copy == str2);
	EXPECT_EQ(str2.length(), str1_copy.length());
}

TEST(StringView_Test, Comparision)
{
	const char* str_data1 = "Hello earth";
	const char* str_data2 = "Hello mars";
	StringView str0 = StringView{str_data1};
	StringView str1 = StringView{str_data1};
	StringView str3 = StringView{str_data2};
	StringView str4 = StringView{str_data2};

	EXPECT_TRUE(str0 == str_data1);
	EXPECT_TRUE(str1 == str0);
	EXPECT_FALSE(str0 == str_data2);
	EXPECT_FALSE(str1 == str3);

	EXPECT_TRUE(str3 != str_data1);
	EXPECT_TRUE(str3 != str0);
	EXPECT_FALSE(str3 != str_data2);
	EXPECT_FALSE(str3 != str4);
}

TEST(StringView_Test, Searching)
{
	const char* str_data = "Hello earth and earthings, Mars and martians!";
	StringView str = StringView{str_data};

	// find
	ASSERT_EQ(str.find('e', 0), 1);
	ASSERT_EQ(str.find('e', 2), 6);
	ASSERT_EQ(str.find('!', 2), 44);
	ASSERT_EQ(str.find('#', 2), StringView::NOT_FOUND);

	ASSERT_EQ(str.find(StringView("earth"), 0), 6);
	ASSERT_EQ(str.find(StringView("earth"), 8), 16);
	ASSERT_EQ(str.find(StringView("!"), 0), 44);
	ASSERT_EQ(str.find(StringView("Pluto-people"), 0), StringView::NOT_FOUND);

	ASSERT_EQ(str.find("earth", 0), 6);
	ASSERT_EQ(str.find("earth", 8), 16);
	ASSERT_EQ(str.find("!", 0), 44);
	ASSERT_EQ(str.find("Pluto-people", 0), StringView::NOT_FOUND);

	// reverse find
	ASSERT_EQ(str.rfind('m'), 36);
	ASSERT_EQ(str.rfind('m', StringView::END), 36);
	ASSERT_EQ(str.rfind('s', 36), 30);
	ASSERT_EQ(str.rfind('H', 5), 0);
	ASSERT_EQ(str.rfind('#', StringView::END), StringView::NOT_FOUND);

	ASSERT_EQ(str.rfind(StringView("earth")), 16);
	ASSERT_EQ(str.rfind(StringView("earth"), 25), 16);
	ASSERT_EQ(str.rfind(StringView("earth"), 11), 6);
	ASSERT_EQ(str.rfind(StringView("!"), StringView::END), 44);
	ASSERT_EQ(str.rfind(StringView("Pluto-people"), 0), StringView::NOT_FOUND);

	ASSERT_EQ(str.rfind("earth"), 16);
	ASSERT_EQ(str.rfind("earth", 25), 16);
	ASSERT_EQ(str.rfind("earth", 11), 6);
	ASSERT_EQ(str.rfind("!", StringView::END), 44);
	ASSERT_EQ(str.rfind("Pluto-people", 0), StringView::NOT_FOUND);
}

TEST(StringView_Test, SubString)
{
	const char* str_data = "Hello earth and earthings, Mars and martians!";
	StringView str = StringView{str_data};
	ASSERT_TRUE(str.substr(6, 5) == "earth");
	ASSERT_TRUE(str.substr(0, 5) == "Hello");
	ASSERT_TRUE(str.substr(36, 9) == "martians!");
}