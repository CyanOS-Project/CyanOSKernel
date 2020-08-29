#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#include "Utils/Vector.h"
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

TEST(Vector_Test, Inserting)
{
	Vector<TestStruct> vec;
	vec.emplace_back(1, 1);
	vec.emplace_front(2, 2);
	vec.emplace_back(3, 3);
	EXPECT_TRUE(vec[0] == TestStruct({2, 2}));
	EXPECT_TRUE(vec[1] == TestStruct({1, 1}));
	EXPECT_TRUE(vec[2] == TestStruct({3, 3}));
	EXPECT_EQ(vec.size(), 3);

	vec.insert(++vec.begin(), TestStruct({4, 4}));

	EXPECT_TRUE(vec[0] == TestStruct({2, 2}));
	EXPECT_TRUE(vec[1] == TestStruct({4, 4}));
	EXPECT_TRUE(vec[2] == TestStruct({1, 1}));
	EXPECT_TRUE(vec[3] == TestStruct({3, 3}));
	EXPECT_EQ(vec.size(), 4);

	EXPECT_TRUE(vec.head() == TestStruct({2, 2}));
	EXPECT_TRUE(vec.tail() == TestStruct({3, 3}));
}

TEST(Vector_Test, Iteration)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	Vector<TestStruct> vec;
	for (size_t i = 0; i < 3; i++) {
		vec.push_back(raw_list[i]);
	}

	EXPECT_EQ(vec.size(), 3);
	EXPECT_FALSE(vec.is_empty());

	size_t index = 0;
	for (auto&& i : vec) {
		EXPECT_TRUE(i == raw_list[index]);
		index++;
	}
}

TEST(Vector_Test, Erasing)
{
	TestStruct raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	Vector<TestStruct> vec;
	for (size_t i = 0; i < 3; i++) {
		vec.push_back(raw_list[i]);
	}

	EXPECT_EQ(vec.size(), 3);
	EXPECT_FALSE(vec.is_empty());

	vec.erase(vec.begin() + 1);
	EXPECT_EQ(vec.size(), 2);
	EXPECT_TRUE(vec[0] == raw_list[0]);
	EXPECT_TRUE(vec[1] == raw_list[2]);

	vec.erase(vec.begin() + 1);
	EXPECT_EQ(vec.size(), 1);
	EXPECT_TRUE(vec[0] == raw_list[0]);

	vec.erase(vec.begin());
	EXPECT_EQ(vec.size(), 0);
	EXPECT_TRUE(vec.is_empty());

	for (size_t i = 0; i < 3; i++) {
		vec.push_back(raw_list[i]);
	}
	EXPECT_EQ(vec.size(), 3);
	EXPECT_FALSE(vec.is_empty());

	vec.clear();

	EXPECT_EQ(vec.size(), 0);
	EXPECT_TRUE(vec.is_empty());
}

TEST(Vector_Test, Splicing)
{
	TestStruct raw_list1[] = {{1, 2}, {3, 4}, {5, 6}};
	TestStruct raw_list2[] = {{10, 20}, {30, 40}, {50, 60}, {70, 80}};
	Vector<TestStruct> list1;
	Vector<TestStruct> list2;
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
	list2.splice(list1, itr + 2);

	EXPECT_EQ(list1.size(), 5);
	EXPECT_EQ(list2.size(), 2);
	EXPECT_TRUE(list1[4] == raw_list2[3]);
	EXPECT_FALSE(list1.is_empty());
	EXPECT_FALSE(list2.is_empty());
}
