#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#include "List.h"
#include <gtest/gtest.h>

struct TestStruct {
	int a;
	int b;
	bool operator==(const TestStruct& other)
	{
		if (a != other.a)
			return false;
		if (b != other.b)
			return false;
		return true;
	}
};

TEST(List_Test, Iteration)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	List<TestStruct> list;
	for (size_t i = 0; i < 3; i++) {
		list.push_back(raw_list[i]);
	}

	EXPECT_EQ(list.size(), 3);
	EXPECT_FALSE(list.is_empty());

	size_t index = 0;
	for (auto&& i : list) {
		EXPECT_TRUE(i == raw_list[index]);
		index++;
	}
}

TEST(List_Test, Erasing)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	List<TestStruct> list;
	for (size_t i = 0; i < 3; i++) {
		list.push_back(raw_list[i]);
	}

	EXPECT_EQ(list.size(), 3);
	EXPECT_FALSE(list.is_empty());

	list.erase(++list.begin());
	EXPECT_EQ(list.size(), 2);
	EXPECT_TRUE(list[0] == raw_list[0]);
	EXPECT_TRUE(list[1] == raw_list[2]);

	list.erase(++list.begin());
	EXPECT_EQ(list.size(), 1);
	EXPECT_TRUE(list[0] == raw_list[0]);

	list.erase(list.begin());
	EXPECT_EQ(list.size(), 0);
	EXPECT_TRUE(list.is_empty());

	for (size_t i = 0; i < 3; i++) {
		list.push_back(raw_list[i]);
	}
	EXPECT_EQ(list.size(), 3);
	EXPECT_FALSE(list.is_empty());

	list.clear();

	EXPECT_EQ(list.size(), 0);
	EXPECT_TRUE(list.is_empty());
}

TEST(List_Test, Splicing)
{
	TestStruct raw_list1[] = {{1, 2}, {3, 4}, {5, 6}};
	TestStruct raw_list2[] = {{10, 20}, {30, 40}, {50, 60}, {70, 80}};
	List<TestStruct> list1;
	List<TestStruct> list2;
	for (size_t i = 0; i < 3; i++) {
		list1.push_back(raw_list1[i]);
	}
	for (size_t i = 0; i < 4; i++) {
		list2.push_back(raw_list2[i]);
	}

	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list2.size(), 4);
	EXPECT_FALSE(list1.is_empty());
	EXPECT_FALSE(list2.is_empty());

	list2.splice(list1, list2.begin());

	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list2.size(), 3);
	EXPECT_TRUE(list1[3] == raw_list2[0]);

	auto itr{list2.begin()};
	itr++;
	itr++;
	list2.splice(list1, itr);

	EXPECT_EQ(list1.size(), 5);
	EXPECT_EQ(list2.size(), 2);
	EXPECT_TRUE(list1[4] == raw_list2[3]);
	EXPECT_FALSE(list1.is_empty());
	EXPECT_FALSE(list2.is_empty());
}

TEST(List_Test, Inserting)
{
	List<TestStruct> list;
	list.emplace_back(1, 1);
	list.emplace_front(2, 2);
	list.emplace_back(3, 3);
	EXPECT_TRUE(list[0] == TestStruct({2, 2}));
	EXPECT_TRUE(list[1] == TestStruct({1, 1}));
	EXPECT_TRUE(list[2] == TestStruct({3, 3}));
	EXPECT_EQ(list.size(), 3);

	list.insert(++list.begin(), TestStruct({4, 4}));

	EXPECT_TRUE(list[0] == TestStruct({2, 2}));
	EXPECT_TRUE(list[1] == TestStruct({1, 1}));
	EXPECT_TRUE(list[2] == TestStruct({4, 4}));
	EXPECT_TRUE(list[3] == TestStruct({3, 3}));
	EXPECT_EQ(list.size(), 4);

	EXPECT_TRUE(list.head() == TestStruct({2, 2}));
	EXPECT_TRUE(list.tail() == TestStruct({3, 3}));
}