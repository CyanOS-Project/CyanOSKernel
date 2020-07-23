#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#define UNIT_TESTING
#define ASSERT(x) (void)ASSERT_TRUE(false)

#include "utils/String.h"
#include <gtest/gtest.h>

TEST(String_Test, Construction)
{
	const char* str_data = "hello world";
	String str0 = String(str_data, 5);
	EXPECT_EQ(strcmp("hello", str0.c_str()), 0);
	EXPECT_EQ(strlen("hello"), str0.length());

	String str1 = String(str_data);
	EXPECT_EQ(strcmp(str_data, str1.c_str()), 0);
	EXPECT_EQ(strlen(str_data), str1.length());

	String str2{str1};
	EXPECT_EQ(strcmp(str_data, str2.c_str()), 0);
	EXPECT_EQ(strlen(str_data), str2.length());

	String str3{std::move(str1)};
	EXPECT_EQ(strcmp(str_data, str2.c_str()), 0);
	EXPECT_EQ(strlen(str_data), str2.length());
}

TEST(String_Test, Assignment)
{
	const char* str_data = "Hello str0";
	String str1 = String("Hello str1");
	String str2 = String("Hello str2");
	String str3 = String("Hello str3");

	str1 = str_data;
	EXPECT_EQ(strcmp(str_data, str1.c_str()), 0);
	EXPECT_EQ(strlen(str_data), str1.length());

	str2 = str1;
	EXPECT_EQ(strcmp(str2.c_str(), str1.c_str()), 0);
	EXPECT_EQ(str2.length(), str1.length());

	String str1_copy{str1};
	str2 = std::move(str1);
	EXPECT_EQ(strcmp(str2.c_str(), str1_copy.c_str()), 0);
	EXPECT_EQ(str2.length(), str1_copy.length());
}

TEST(String_Test, Insertion)
{
	const char* str_data = "(Hello str0)";
	String str0 = String{str_data};
	String str1 = String("Hello  str1");
	String str2 = String("Hello  str2");
	String str3 = String("Hello  str3");
	String str4 = String("Hello  str4");

	str1.insert(6, str0);
	EXPECT_TRUE(str1 == "Hello (Hello str0) str1");

	str2.insert(6, str_data);
	EXPECT_TRUE(str2 == "Hello (Hello str0) str2");

	str3.insert(6, str0, 7, 4);
	EXPECT_TRUE(str3 == "Hello str0 str3");

	str4.insert(6, str_data, 7, 4);
	EXPECT_TRUE(str4 == "Hello str0 str4");
}

TEST(String_Test, Comparision)
{
	const char* str_data1 = "Hello earth";
	const char* str_data2 = "Hello mars";
	String str0 = String{str_data1};
	String str1 = String{str_data1};
	String str3 = String{str_data2};
	String str4 = String{str_data2};

	EXPECT_TRUE(str0 == str_data1);
	EXPECT_TRUE(str1 == str0);
	EXPECT_FALSE(str0 == str_data2);
	EXPECT_FALSE(str1 == str3);

	EXPECT_TRUE(str3 != str_data1);
	EXPECT_TRUE(str3 != str0);
	EXPECT_FALSE(str3 != str_data2);
	EXPECT_FALSE(str3 != str4);
}

TEST(String_Test, Appending)
{
	const char* str_data = "Hello earth";
	String str = String{str_data};

	str += " and Mars!";
	EXPECT_TRUE(str == "Hello earth and Mars!");

	str = String{str_data};
	str += String(" and Mars!");
	EXPECT_TRUE(str == "Hello earth and Mars!");

	str = String{str_data};
	str.push_back('!');
	EXPECT_TRUE(str == "Hello earth!");

	str = String{str_data};
	str.pop_back();
	EXPECT_TRUE(str == "Hello eart");
}

TEST(String_Test, Searching)
{
	const char* str_data = "Hello earth and earthings, Mars and martians!";
	String str = String{str_data};

	// find
	ASSERT_EQ(str.find('e', 0), 1);
	ASSERT_EQ(str.find('e', 2), 6);
	ASSERT_EQ(str.find('!', 2), 44);
	ASSERT_EQ(str.find('#', 2), String::NOT_FOUND);

	ASSERT_EQ(str.find(String("earth"), 0), 6);
	ASSERT_EQ(str.find(String("earth"), 8), 16);
	ASSERT_EQ(str.find(String("!"), 0), 44);
	ASSERT_EQ(str.find(String("Pluto-people"), 0), String::NOT_FOUND);

	ASSERT_EQ(str.find("earth", 0), 6);
	ASSERT_EQ(str.find("earth", 8), 16);
	ASSERT_EQ(str.find("!", 0), 44);
	ASSERT_EQ(str.find("Pluto-people", 0), String::NOT_FOUND);

	// reverse find
	ASSERT_EQ(str.rfind('m'), 36);
	ASSERT_EQ(str.rfind('m', String::END), 36);
	ASSERT_EQ(str.rfind('s', 36), 30);
	ASSERT_EQ(str.rfind('H', 5), 0);
	ASSERT_EQ(str.rfind('#', String::END), String::NOT_FOUND);

	ASSERT_EQ(str.rfind(String("earth")), 16);
	ASSERT_EQ(str.rfind(String("earth"), 25), 16);
	ASSERT_EQ(str.rfind(String("earth"), 11), 6);
	ASSERT_EQ(str.rfind(String("!"), String::END), 44);
	ASSERT_EQ(str.rfind(String("Pluto-people"), 0), String::NOT_FOUND);

	ASSERT_EQ(str.rfind("earth"), 16);
	ASSERT_EQ(str.rfind("earth", 25), 16);
	ASSERT_EQ(str.rfind("earth", 11), 6);
	ASSERT_EQ(str.rfind("!", String::END), 44);
	ASSERT_EQ(str.rfind("Pluto-people", 0), String::NOT_FOUND);
}

TEST(String_Test, SubString)
{
	const char* str_data = "Hello earth and earthings, Mars and martians!";
	String str = String{str_data};
	ASSERT_TRUE(str.substr(6, 5) == "earth");
	ASSERT_TRUE(str.substr(0, 5) == "Hello");
	ASSERT_TRUE(str.substr(36, 9) == "martians!");
}