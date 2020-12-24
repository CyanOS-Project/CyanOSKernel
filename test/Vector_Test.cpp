#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#include <gtest/gtest.h>
#include <stdlib/Vector.h>

struct TestStruct2 {
	int a;
	int b;
	TestStruct2(int t_a, int t_b) : a{t_a}, b{t_b} {}
	bool operator==(const TestStruct2& other) const
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
	Vector<TestStruct2> vec;
	vec.emplace_back(1, 1);
	vec.emplace_front(2, 2);
	vec.emplace_back(3, 3);

	EXPECT_TRUE(vec[0] == TestStruct2(2, 2));
	EXPECT_TRUE(vec[1] == TestStruct2(1, 1));
	EXPECT_TRUE(vec[2] == TestStruct2(3, 3));
	EXPECT_EQ(vec.size(), 3);

	vec.insert(++vec.begin(), TestStruct2(4, 4));

	EXPECT_TRUE(vec[0] == TestStruct2(2, 2));
	EXPECT_TRUE(vec[1] == TestStruct2(4, 4));
	EXPECT_TRUE(vec[2] == TestStruct2(1, 1));
	EXPECT_TRUE(vec[3] == TestStruct2(3, 3));
	EXPECT_EQ(vec.size(), 4);

	EXPECT_TRUE(vec.head() == TestStruct2(2, 2));
	EXPECT_TRUE(vec.tail() == TestStruct2(3, 3));
}

TEST(Vector_Test, Iteration)
{
	TestStruct2 raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	Vector<TestStruct2> vec;
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
	TestStruct2 raw_list[] = {{1, 2}, {3, 4}, {5, 6}};
	Vector<TestStruct2> vec;
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
	TestStruct2 raw_list1[] = {{1, 2}, {3, 4}, {5, 6}};
	TestStruct2 raw_list2[] = {{10, 20}, {30, 40}, {50, 60}, {70, 80}};
	Vector<TestStruct2> list1;
	Vector<TestStruct2> list2;
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

TEST(Vector_Test, Capacity)
{
	Vector<TestStruct2> list(4, 5);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 0);

	list.emplace_back(1, 1);
	list.emplace_back(1, 1);
	list.emplace_back(1, 1);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 3);

	list.emplace_back(1, 1);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 4);

	list.emplace_back(1, 1);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 5);

	list.erase(list.begin() + 1);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 4);
}
