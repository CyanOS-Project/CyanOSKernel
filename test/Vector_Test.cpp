#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif
#include <gtest/gtest.h>
#include <stdlib/Vector.h>

TEST(Vector_Test, Iteration)
{
	Vector<int> vec;
	auto get_last_itr = [&vec]() {
		auto itr = vec.cbegin();
		auto tmp = itr;
		while (++tmp != vec.cend()) {
			itr = tmp;
		}
		return itr;
	};

	vec.push_back(0);
	vec.push_back(1);
	vec.push_back(2);

	EXPECT_TRUE(vec.cbegin() == vec.begin());
	EXPECT_TRUE(vec.cend() == vec.end());
	auto constant_iterator = vec.cbegin();
	constant_iterator = vec.begin();

	EXPECT_EQ(vec.size(), 3);
	EXPECT_FALSE(vec.is_empty());

	auto itr = vec.cbegin();
	EXPECT_EQ(*itr++, 0);
	EXPECT_EQ(*itr++, 1);
	EXPECT_EQ(*itr++, 2);
	EXPECT_EQ(itr, vec.cend());

	itr = vec.cbegin();
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*itr, 2);
	EXPECT_EQ(++itr, vec.cend());

	itr = get_last_itr();
	EXPECT_EQ(*itr--, 2);
	EXPECT_EQ(*itr--, 1);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(itr, vec.cbegin());

	itr = get_last_itr();
	EXPECT_EQ(*--itr, 1);
	EXPECT_EQ(*--itr, 0);
	EXPECT_EQ(itr, vec.cbegin());

	itr = vec.cbegin();
	EXPECT_EQ(itr + 0, vec.cbegin());
	EXPECT_EQ(*(itr + 0), 0);
	EXPECT_EQ(*(itr + 1), 1);
	EXPECT_EQ(*(itr + 2), 2);
	EXPECT_EQ(itr + 3, vec.cend());

	itr = vec.cbegin();
	EXPECT_EQ(*(vec.cbegin() += 0), 0);
	EXPECT_EQ(*(vec.cbegin() += 1), 1);
	EXPECT_EQ(*(vec.cbegin() += 2), 2);
	EXPECT_EQ((vec.cbegin() += 3), vec.cend());

	itr = get_last_itr();
	EXPECT_EQ(itr + 1, vec.cend());
	EXPECT_EQ(*(itr - 0), 2);
	EXPECT_EQ(*(itr - 1), 1);
	EXPECT_EQ(*(itr - 2), 0);

	EXPECT_EQ(*(get_last_itr() -= 0), 2);
	EXPECT_EQ(*(get_last_itr() -= 1), 1);
	EXPECT_EQ(*(get_last_itr() -= 2), 0);

	EXPECT_EQ(*(vec.cbegin() + 0), vec[0]);
	EXPECT_EQ(*(vec.cbegin() + 1), vec[1]);
	EXPECT_EQ(*(vec.cbegin() + 2), vec[2]);
}

TEST(Vector_Test, Inserting)
{
	Vector<int> vec0;
	vec0.emplace(vec0.begin(), 4);     // [4]
	vec0.emplace(vec0.begin(), 1);     // [1, 4]
	vec0.emplace(vec0.begin() + 1, 2); // [1, 2, 4]
	vec0.emplace(vec0.begin() + 2, 3); // [1, 2, 3, 4]
	vec0.emplace(vec0.end(), 6);       // [1, 2, 3, 4, 6]
	vec0.emplace(vec0.end() - 1, 5);   // [1, 2, 3, 4, 5, 6]
	EXPECT_EQ(vec0.size(), 6);
	EXPECT_EQ(*(vec0.cbegin() + 0), 1);
	EXPECT_EQ(*(vec0.cbegin() + 1), 2);
	EXPECT_EQ(*(vec0.cbegin() + 2), 3);
	EXPECT_EQ(*(vec0.cbegin() + 3), 4);
	EXPECT_EQ(*(vec0.cbegin() + 4), 5);
	EXPECT_EQ(*(vec0.cbegin() + 5), 6);

	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	EXPECT_EQ(vec1.size(), 3);
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 2);
	EXPECT_EQ(*(vec1.cbegin() + 2), 3);

	Vector<int> vec2;
	vec2.push_front(1);
	vec2.push_front(2);
	vec2.push_front(3);
	EXPECT_EQ(vec2.size(), 3);
	EXPECT_EQ(*(vec2.cbegin() + 0), 3);
	EXPECT_EQ(*(vec2.cbegin() + 1), 2);
	EXPECT_EQ(*(vec2.cbegin() + 2), 1);

	Vector<int> vec3;
	vec3.emplace_back(1);
	vec3.emplace_back(2);
	vec3.emplace_back(3);
	EXPECT_EQ(vec3.size(), 3);
	EXPECT_EQ(*(vec3.cbegin() + 0), 1);
	EXPECT_EQ(*(vec3.cbegin() + 1), 2);
	EXPECT_EQ(*(vec3.cbegin() + 2), 3);

	Vector<int> vec4;
	vec4.emplace_front(1);
	vec4.emplace_front(2);
	vec4.emplace_front(3);
	EXPECT_EQ(vec4.size(), 3);
	EXPECT_EQ(*(vec4.cbegin() + 0), 3);
	EXPECT_EQ(*(vec4.cbegin() + 1), 2);
	EXPECT_EQ(*(vec4.cbegin() + 2), 1);

	Vector<int> vec5;
	vec5.push_back(1);
	vec5.push_back(2);
	vec5.push_back(3);
	EXPECT_EQ(vec5.size(), 3);
	EXPECT_EQ(*(vec5.cbegin() + 0), 1);
	EXPECT_EQ(*(vec5.cbegin() + 1), 2);
	EXPECT_EQ(*(vec5.cbegin() + 2), 3);

	vec5.insert(vec5.begin(), 4);
	EXPECT_EQ(vec5.size(), 4);
	EXPECT_EQ(*(vec5.cbegin() + 0), 4);
	EXPECT_EQ(*(vec5.cbegin() + 1), 1);
	EXPECT_EQ(*(vec5.cbegin() + 2), 2);
	EXPECT_EQ(*(vec5.cbegin() + 3), 3);

	vec5.insert(vec5.begin() + 1, 5);
	EXPECT_EQ(vec5.size(), 5);
	EXPECT_EQ(*(vec5.cbegin() + 0), 4);
	EXPECT_EQ(*(vec5.cbegin() + 1), 5);
	EXPECT_EQ(*(vec5.cbegin() + 2), 1);
	EXPECT_EQ(*(vec5.cbegin() + 3), 2);
	EXPECT_EQ(*(vec5.cbegin() + 4), 3);

	vec5.insert(vec5.end(), 6);
	EXPECT_EQ(vec5.size(), 6);
	EXPECT_EQ(*(vec5.cbegin() + 0), 4);
	EXPECT_EQ(*(vec5.cbegin() + 1), 5);
	EXPECT_EQ(*(vec5.cbegin() + 2), 1);
	EXPECT_EQ(*(vec5.cbegin() + 3), 2);
	EXPECT_EQ(*(vec5.cbegin() + 4), 3);
	EXPECT_EQ(*(vec5.cbegin() + 5), 6);
}

TEST(Vector_Test, Erasing)
{
	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);

	EXPECT_EQ(vec1.size(), 4); // [1, 2, 3, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 2);
	EXPECT_EQ(*(vec1.cbegin() + 2), 3);
	EXPECT_EQ(*(vec1.cbegin() + 3), 4);
	EXPECT_EQ(vec1.cbegin() + 4, vec1.cend());

	vec1.pop_front();
	EXPECT_EQ(vec1.size(), 3); // [2, 3, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 2);
	EXPECT_EQ(*(vec1.cbegin() + 1), 3);
	EXPECT_EQ(*(vec1.cbegin() + 2), 4);
	EXPECT_EQ(vec1.cbegin() + 3, vec1.cend());

	vec1.pop_front();
	EXPECT_EQ(vec1.size(), 2); // [3, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 3);
	EXPECT_EQ(*(vec1.cbegin() + 1), 4);
	EXPECT_EQ(vec1.cbegin() + 2, vec1.cend());

	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 1); // [3]
	EXPECT_EQ(*(vec1.cbegin() + 0), 3);
	EXPECT_EQ(vec1.cbegin() + 1, vec1.cend());

	vec1.erase(vec1.begin());
	EXPECT_EQ(vec1.size(), 0); // []
	EXPECT_EQ(vec1.cbegin(), vec1.cend());

	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);
	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 3); // [1, 3, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 3);
	EXPECT_EQ(*(vec1.cbegin() + 2), 4);
	EXPECT_EQ(vec1.cbegin() + 3, vec1.cend());

	vec1.erase(vec1.begin() + 1);
	EXPECT_EQ(vec1.size(), 2); // [1, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 4);
	EXPECT_EQ(vec1.cbegin() + 2, vec1.cend());

	vec1.clear();
	EXPECT_EQ(vec1.size(), 0); // []
	EXPECT_EQ(vec1.cbegin(), vec1.cend());

	vec1.push_back(2);
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(2);
	vec1.push_back(4);
	vec1.remove_if([&](auto& i) { return i == 2; });
	EXPECT_EQ(vec1.size(), 3); // [1, 3, 4]
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 3);
	EXPECT_EQ(*(vec1.cbegin() + 2), 4);
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
	EXPECT_EQ(*(vec1.cbegin() + 0), 1);
	EXPECT_EQ(*(vec1.cbegin() + 1), 3);
	EXPECT_EQ(*(vec1.cbegin() + 2), 4);
	EXPECT_EQ(*(vec2.cbegin() + 0), 5);
	EXPECT_EQ(*(vec2.cbegin() + 1), 6);
	EXPECT_EQ(*(vec2.cbegin() + 2), 2);
	EXPECT_EQ(*(vec2.cbegin() + 3), 7);
	EXPECT_EQ(*(vec2.cbegin() + 4), 8);

	vec2.splice(vec1, vec2.end() - 1, vec1.begin());
	// vec1: [8, 1, 3, 4] vec2: [5, 6, 2, 7]
	EXPECT_EQ(vec1.size(), 4);
	EXPECT_EQ(vec2.size(), 4);
	EXPECT_EQ(*(vec1.cbegin() + 0), 8);
	EXPECT_EQ(*(vec1.cbegin() + 1), 1);
	EXPECT_EQ(*(vec1.cbegin() + 2), 3);
	EXPECT_EQ(*(vec1.cbegin() + 3), 4);
	EXPECT_EQ(*(vec2.cbegin() + 0), 5);
	EXPECT_EQ(*(vec2.cbegin() + 1), 6);
	EXPECT_EQ(*(vec2.cbegin() + 2), 2);
	EXPECT_EQ(*(vec2.cbegin() + 3), 7);
}

TEST(Vector_Test, Capacity)
{
	Vector<int> list(4, 5);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 0);

	list.push_front(1);
	list.push_front(2);
	list.push_front(3);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 3);
	EXPECT_EQ(*(list.cbegin() + 0), 3);
	EXPECT_EQ(*(list.cbegin() + 1), 2);
	EXPECT_EQ(*(list.cbegin() + 2), 1);

	list.insert(list.begin() + 2, 5);
	EXPECT_EQ(list.capacity(), 4);
	EXPECT_EQ(list.size(), 4);
	EXPECT_EQ(*(list.cbegin() + 0), 3);
	EXPECT_EQ(*(list.cbegin() + 1), 2);
	EXPECT_EQ(*(list.cbegin() + 2), 5);
	EXPECT_EQ(*(list.cbegin() + 3), 1);

	list.insert(list.end() - 1, 6);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 5);
	EXPECT_EQ(*(list.cbegin() + 0), 3);
	EXPECT_EQ(*(list.cbegin() + 1), 2);
	EXPECT_EQ(*(list.cbegin() + 2), 5);
	EXPECT_EQ(*(list.cbegin() + 3), 6);
	EXPECT_EQ(*(list.cbegin() + 4), 1);

	list.erase(list.begin() + 1);
	EXPECT_EQ(list.capacity(), 9);
	EXPECT_EQ(list.size(), 4);
	EXPECT_EQ(*(list.cbegin() + 0), 3);
	EXPECT_EQ(*(list.cbegin() + 1), 5);
	EXPECT_EQ(*(list.cbegin() + 2), 6);
	EXPECT_EQ(*(list.cbegin() + 3), 1);

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

TEST(Vector_Test, MovingAndCopying)
{
	Vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);
	vec1.push_back(4);

	Vector<int> vec2 = vec1;
	EXPECT_EQ(vec1.capacity(), 4);
	EXPECT_EQ(vec1.size(), 4);
	EXPECT_EQ(vec2.capacity(), 4);
	EXPECT_EQ(vec2.size(), 4);
	EXPECT_EQ(vec1[0], 1);
	EXPECT_EQ(vec1[1], 2);
	EXPECT_EQ(vec1[2], 3);
	EXPECT_EQ(vec1[3], 4);
	EXPECT_EQ(vec2[0], 1);
	EXPECT_EQ(vec2[1], 2);
	EXPECT_EQ(vec2[2], 3);
	EXPECT_EQ(vec2[3], 4);

	Vector<int> vec3 = move(vec1);
	EXPECT_EQ(vec3.capacity(), 4);
	EXPECT_EQ(vec3.size(), 4);
	EXPECT_EQ(vec3[0], 1);
	EXPECT_EQ(vec3[1], 2);
	EXPECT_EQ(vec3[2], 3);
	EXPECT_EQ(vec3[3], 4);

	Vector<int> vec4;
	vec4 = vec3;
	EXPECT_EQ(vec3.capacity(), 4);
	EXPECT_EQ(vec3.size(), 4);
	EXPECT_EQ(vec3[0], 1);
	EXPECT_EQ(vec3[1], 2);
	EXPECT_EQ(vec3[2], 3);
	EXPECT_EQ(vec3[3], 4);
	EXPECT_EQ(vec4.capacity(), 4);
	EXPECT_EQ(vec4.size(), 4);
	EXPECT_EQ(vec4[0], 1);
	EXPECT_EQ(vec4[1], 2);
	EXPECT_EQ(vec4[2], 3);
	EXPECT_EQ(vec4[3], 4);

	Vector<int> vec5;
	vec5 = move(vec4);
	EXPECT_EQ(vec5.capacity(), 4);
	EXPECT_EQ(vec5.size(), 4);
	EXPECT_EQ(vec5[0], 1);
	EXPECT_EQ(vec5[1], 2);
	EXPECT_EQ(vec5[2], 3);
	EXPECT_EQ(vec5[3], 4);
}