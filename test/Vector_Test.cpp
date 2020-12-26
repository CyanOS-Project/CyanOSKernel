#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#include <gtest/gtest.h>
#include <stdlib/Vector.h>

TEST(Vector_Test, Iteration)
{
	Vector<int> vec;
	auto get_last_itr = [&vec]() {
		auto itr = vec.begin();
		auto tmp = itr;
		while (++tmp != vec.end()) {
			itr = tmp;
		}
		return itr;
	};

	vec.push_back(0);
	vec.push_back(1);
	vec.push_back(2);

	EXPECT_EQ(vec.size(), 3);
	EXPECT_FALSE(vec.is_empty());

	auto itr = vec.begin();
	EXPECT_EQ(*itr++, 0);
	EXPECT_EQ(*itr++, 1);
	EXPECT_EQ(*itr++, 2);
	EXPECT_EQ(itr, vec.end());

	itr = vec.begin();
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*itr, 2);
	EXPECT_EQ(++itr, vec.end());

	itr = get_last_itr();
	EXPECT_EQ(*itr--, 2);
	EXPECT_EQ(*itr--, 1);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(itr, vec.begin());

	itr = get_last_itr();
	EXPECT_EQ(*--itr, 1);
	EXPECT_EQ(*--itr, 0);
	EXPECT_EQ(itr, vec.begin());

	itr = vec.begin();
	EXPECT_EQ(itr + 0, vec.begin());
	EXPECT_EQ(*(itr + 0), 0);
	EXPECT_EQ(*(itr + 1), 1);
	EXPECT_EQ(*(itr + 2), 2);
	EXPECT_EQ(itr + 3, vec.end());

	itr = vec.begin();
	EXPECT_EQ(*(vec.begin() += 0), 0);
	EXPECT_EQ(*(vec.begin() += 1), 1);
	EXPECT_EQ(*(vec.begin() += 2), 2);
	EXPECT_EQ((vec.begin() += 3), vec.end());

	itr = get_last_itr();
	EXPECT_EQ(itr + 1, vec.end());
	EXPECT_EQ(*(itr - 0), 2);
	EXPECT_EQ(*(itr - 1), 1);
	EXPECT_EQ(*(itr - 2), 0);

	EXPECT_EQ(*(get_last_itr() -= 0), 2);
	EXPECT_EQ(*(get_last_itr() -= 1), 1);
	EXPECT_EQ(*(get_last_itr() -= 2), 0);

	EXPECT_EQ(*(vec.begin() + 0), vec[0]);
	EXPECT_EQ(*(vec.begin() + 1), vec[1]);
	EXPECT_EQ(*(vec.begin() + 2), vec[2]);
}

TEST(Vector_Test, Inserting)
{
	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	EXPECT_EQ(vec1.size(), 3);
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 2);
	EXPECT_EQ(*(vec1.begin() + 2), 3);

	Vector<int> vec2;
	vec2.push_front(1);
	vec2.push_front(2);
	vec2.push_front(3);
	EXPECT_EQ(vec2.size(), 3);
	EXPECT_EQ(*(vec2.begin() + 0), 3);
	EXPECT_EQ(*(vec2.begin() + 1), 2);
	EXPECT_EQ(*(vec2.begin() + 2), 1);

	Vector<int> vec3;
	vec3.emplace_back(1);
	vec3.emplace_back(2);
	vec3.emplace_back(3);
	EXPECT_EQ(vec3.size(), 3);
	EXPECT_EQ(*(vec3.begin() + 0), 1);
	EXPECT_EQ(*(vec3.begin() + 1), 2);
	EXPECT_EQ(*(vec3.begin() + 2), 3);

	Vector<int> vec4;
	vec4.emplace_front(1);
	vec4.emplace_front(2);
	vec4.emplace_front(3);
	EXPECT_EQ(vec4.size(), 3);
	EXPECT_EQ(*(vec4.begin() + 0), 3);
	EXPECT_EQ(*(vec4.begin() + 1), 2);
	EXPECT_EQ(*(vec4.begin() + 2), 1);

	Vector<int> vec5;
	vec5.push_back(1);
	vec5.push_back(2);
	vec5.push_back(3);
	EXPECT_EQ(vec5.size(), 3);
	EXPECT_EQ(*(vec5.begin() + 0), 1);
	EXPECT_EQ(*(vec5.begin() + 1), 2);
	EXPECT_EQ(*(vec5.begin() + 2), 3);

	vec5.insert(vec5.begin(), 4);
	EXPECT_EQ(vec5.size(), 4);
	EXPECT_EQ(*(vec5.begin() + 0), 4);
	EXPECT_EQ(*(vec5.begin() + 1), 1);
	EXPECT_EQ(*(vec5.begin() + 2), 2);
	EXPECT_EQ(*(vec5.begin() + 3), 3);

	vec5.insert(vec5.begin() + 1, 5);
	EXPECT_EQ(vec5.size(), 5);
	EXPECT_EQ(*(vec5.begin() + 0), 4);
	EXPECT_EQ(*(vec5.begin() + 1), 5);
	EXPECT_EQ(*(vec5.begin() + 2), 1);
	EXPECT_EQ(*(vec5.begin() + 3), 2);
	EXPECT_EQ(*(vec5.begin() + 4), 3);

	vec5.insert(vec5.end(), 6);
	EXPECT_EQ(vec5.size(), 6);
	EXPECT_EQ(*(vec5.begin() + 0), 4);
	EXPECT_EQ(*(vec5.begin() + 1), 5);
	EXPECT_EQ(*(vec5.begin() + 2), 1);
	EXPECT_EQ(*(vec5.begin() + 3), 2);
	EXPECT_EQ(*(vec5.begin() + 4), 3);
	EXPECT_EQ(*(vec5.begin() + 5), 6);
}

TEST(Vector_Test, Erasing)
{
	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);

	EXPECT_EQ(vec1.size(), 4); // [1, 2, 3, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 2);
	EXPECT_EQ(*(vec1.begin() + 2), 3);
	EXPECT_EQ(*(vec1.begin() + 3), 4);
	EXPECT_EQ(vec1.begin() + 4, vec1.end());

	vec1.pop_front();
	EXPECT_EQ(vec1.size(), 3); // [2, 3, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 2);
	EXPECT_EQ(*(vec1.begin() + 1), 3);
	EXPECT_EQ(*(vec1.begin() + 2), 4);
	EXPECT_EQ(vec1.begin() + 3, vec1.end());

	vec1.pop_front();
	EXPECT_EQ(vec1.size(), 2); // [3, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 3);
	EXPECT_EQ(*(vec1.begin() + 1), 4);
	EXPECT_EQ(vec1.begin() + 2, vec1.end());

	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 1); // [3]
	EXPECT_EQ(*(vec1.begin() + 0), 3);
	EXPECT_EQ(vec1.begin() + 1, vec1.end());

	vec1.erase(vec1.begin());
	EXPECT_EQ(vec1.size(), 0); // []
	EXPECT_EQ(vec1.begin(), vec1.end());

	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);
	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 3); // [1, 3, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 3);
	EXPECT_EQ(*(vec1.begin() + 2), 4);
	EXPECT_EQ(vec1.begin() + 3, vec1.end());

	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 2); // [1, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 4);
	EXPECT_EQ(vec1.begin() + 2, vec1.end());

	vec1.clear();
	EXPECT_EQ(vec1.size(), 0); // []
	EXPECT_EQ(vec1.begin(), vec1.end());

	vec1.push_back(2);
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(2);
	vec1.push_back(4);
	vec1.remove_if([&](auto& i) { return i == 2; });
	EXPECT_EQ(vec1.size(), 3); // [1, 3, 4]
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 3);
	EXPECT_EQ(*(vec1.begin() + 2), 4);
}

TEST(Vector_Test, Splicing)
{
	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);

	Vector<int> vec2;
	vec2.push_back(5);
	vec2.push_back(6);
	vec2.push_back(7);
	vec2.push_back(8);
	// vec1: [1, 2, 3, 4] vec2: [5, 6, 7, 8]
	vec1.splice(vec2, vec1.begin() + 1, vec2.begin() + 2);
	// vec1: [1, 3, 4] vec2: [5, 6, 2, 7, 8]
	EXPECT_EQ(vec1.size(), 3);
	EXPECT_EQ(vec2.size(), 5);
	EXPECT_EQ(*(vec1.begin() + 0), 1);
	EXPECT_EQ(*(vec1.begin() + 1), 3);
	EXPECT_EQ(*(vec1.begin() + 2), 4);
	EXPECT_EQ(*(vec2.begin() + 0), 5);
	EXPECT_EQ(*(vec2.begin() + 1), 6);
	EXPECT_EQ(*(vec2.begin() + 2), 2);
	EXPECT_EQ(*(vec2.begin() + 3), 7);
	EXPECT_EQ(*(vec2.begin() + 4), 8);

	vec2.splice(vec1, vec2.end() - 1, vec1.begin());
	// vec1: [8, 1, 3, 4] vec2: [5, 6, 2, 7]
	EXPECT_EQ(vec1.size(), 4);
	EXPECT_EQ(vec2.size(), 4);
	EXPECT_EQ(*(vec1.begin() + 0), 8);
	EXPECT_EQ(*(vec1.begin() + 1), 1);
	EXPECT_EQ(*(vec1.begin() + 2), 3);
	EXPECT_EQ(*(vec1.begin() + 3), 4);
	EXPECT_EQ(*(vec2.begin() + 0), 5);
	EXPECT_EQ(*(vec2.begin() + 1), 6);
	EXPECT_EQ(*(vec2.begin() + 2), 2);
	EXPECT_EQ(*(vec2.begin() + 3), 7);
}

TEST(Vector_Test, Capacity)
{
	Vector<int> list(4, 5);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 0);

	list.push_back(1);
	list.push_back(1);
	list.push_back(1);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 3);

	list.push_back(1);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 4);

	list.push_back(1);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 5);

	list.erase(list.begin() + 1);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 4);

	list.reserve(5);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 4);

	list.reserve(10);
	EXPECT_EQ(list.capacity(), 10);
	EXPECT_EQ(list.size(), 4);

	for (size_t i = 0; i < 6; i++) {
		list.push_back(1);
	}
	EXPECT_EQ(list.capacity(), 10);
	EXPECT_EQ(list.size(), 10);

	list.push_back(1);
	EXPECT_EQ(list.capacity(), 15);
	EXPECT_EQ(list.size(), 11);
}
