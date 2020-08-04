#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include "utils/list.h"
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

TEST(CircularQueue_Test, Iteration)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	CircularQueue<TestStruct> list;
	for (size_t i = 0; i < 3; i++) {
		list.push_back(raw_list[i]);
	}

	EXPECT_EQ(list.size(), 3);

	size_t index = 0;
	for (auto&& i : list) {
		EXPECT_TRUE(i == raw_list[index]);
		index++;
	}
}

TEST(CircularQueue_Test, Removing)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	CircularQueue<TestStruct> list;
	for (size_t i = 0; i < 3; i++) {
		list.push_back(raw_list[i]);
	}

	EXPECT_EQ(list.size(), 3);

	list.remove(1);
	EXPECT_EQ(list.size(), 2);
	EXPECT_TRUE(list[0] == raw_list[0]);
	EXPECT_TRUE(list[1] == raw_list[2]);

	list.remove(1);
	EXPECT_EQ(list.size(), 1);
	EXPECT_TRUE(list[0] == raw_list[0]);

	list.remove(0);
	EXPECT_EQ(list.size(), 0);
}

TEST(CircularQueue_Test, MovingBetweenLists)
{
	TestStruct raw_list1[] = {{1, 2}, {3, 4}, {5, 6}};
	TestStruct raw_list2[] = {{10, 20}, {30, 40}, {50, 60}, {70, 80}};
	CircularQueue<TestStruct> list1;
	CircularQueue<TestStruct> list2;
	for (size_t i = 0; i < 3; i++) {
		list1.push_back(raw_list1[i]);
	}
	for (size_t i = 0; i < 4; i++) {
		list2.push_back(raw_list2[i]);
	}

	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list2.size(), 4);

	list2.move_head_to_other_list(&list1);

	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list2.size(), 3);
	EXPECT_TRUE(list1[3] == raw_list2[0]);

	auto itr{list2.begin()};
	itr.move_cursor(2);
	list2.move_to_other_list(&list1, itr);

	EXPECT_EQ(list1.size(), 5);
	EXPECT_EQ(list2.size(), 2);
	EXPECT_TRUE(list1[4] == raw_list2[3]);
}