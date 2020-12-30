#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <gtest/gtest.h>
#include <stdlib/List.h>

auto get_iterator_at_pos = [](List<int>& lst, int pos) {
	auto itr = lst.begin();
	while (pos--) {
		itr++;
	}
	return itr;
};

TEST(List_Test, Iteration)
{
	// TODO: check two versions of find const/non-const
	List<int> list1;
	auto get_last_itr = [&list1]() {
		auto itr = list1.cbegin();
		auto tmp = itr;
		while (++tmp != list1.end()) {
			itr = tmp;
		}
		return itr;
	};

	list1.push_back(0);
	list1.push_back(1);
	list1.push_back(2);
	list1.push_back(3);
	list1.push_back(4);
	list1.push_back(5);

	EXPECT_TRUE(list1.cbegin() == list1.begin());
	EXPECT_TRUE(list1.cend() == list1.cend());
	auto constant_iterator = list1.cbegin();
	constant_iterator = list1.begin();

	auto itr = list1.cbegin();
	EXPECT_EQ(*itr++, 0);
	EXPECT_EQ(*itr++, 1);
	EXPECT_EQ(*itr++, 2);
	EXPECT_EQ(*itr++, 3);
	EXPECT_EQ(*itr++, 4);
	EXPECT_EQ(*itr++, 5);
	EXPECT_EQ(itr, list1.cend());

	itr = list1.cbegin();
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);
	EXPECT_EQ(*itr, 5);
	EXPECT_EQ(++itr, list1.cend());

	itr = get_last_itr();
	EXPECT_EQ(*itr--, 5);
	EXPECT_EQ(*itr--, 4);
	EXPECT_EQ(*itr--, 3);
	EXPECT_EQ(*itr--, 2);
	EXPECT_EQ(*itr--, 1);
	EXPECT_EQ(*itr, 0);
	itr = get_last_itr();
	EXPECT_EQ(*--itr, 4);
	EXPECT_EQ(*--itr, 3);
	EXPECT_EQ(*--itr, 2);
	EXPECT_EQ(*--itr, 1);
	EXPECT_EQ(*--itr, 0);

	// If you get to end(), you can come back by operator--.
	itr = get_last_itr();
	EXPECT_EQ(++itr, list1.cend());
	EXPECT_NE(--itr, list1.cend()); // Not Equal
	EXPECT_EQ(*itr, 5);
	EXPECT_EQ(*--itr, 4);

	itr = get_last_itr();
	EXPECT_EQ(*(itr++), 5);
	EXPECT_EQ(itr, list1.cend());
	EXPECT_EQ(itr--, list1.cend());
	EXPECT_NE(itr, list1.cend());
	EXPECT_EQ(*itr--, 5);
	EXPECT_EQ(*itr, 4);
}

TEST(List_Test, Removing)
{
	int raw_list[] = {0, 1, 2, 3, 4, 5};
	List<int> list1;

	list1.push_back(0);
	list1.push_back(1);
	list1.push_back(2);
	list1.push_back(3);
	list1.push_back(4);
	list1.push_back(5);
	// [0, 1, 2, 3, 4, 5].
	auto itr = list1.cbegin();
	EXPECT_EQ(list1.size(), 6);
	EXPECT_EQ(list1.first(), 0);
	EXPECT_EQ(list1.last(), 5);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);

	list1.remove(get_iterator_at_pos(list1, 2));
	// [0, 1, 3, 4, 5].
	itr = list1.cbegin();
	EXPECT_EQ(list1.size(), 5);
	EXPECT_EQ(list1.first(), 0);
	EXPECT_EQ(list1.last(), 5);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);

	list1.remove(get_iterator_at_pos(list1, 4));
	// [0, 1, 3, 4].
	itr = list1.cbegin();
	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list1.first(), 0);
	EXPECT_EQ(list1.last(), 4);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);

	list1.remove(get_iterator_at_pos(list1, 0));
	// [1, 3, 4].
	itr = list1.cbegin();
	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list1.first(), 1);
	EXPECT_EQ(list1.last(), 4);
	EXPECT_EQ(*itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);

	list1.remove(get_iterator_at_pos(list1, 0));
	list1.remove(get_iterator_at_pos(list1, 1));
	// [3].
	itr = list1.cbegin();
	EXPECT_EQ(list1.first(), 3);
	EXPECT_EQ(list1.last(), 3);
	EXPECT_EQ(list1.size(), 1);
	EXPECT_EQ(*itr, 3);
	// []
	list1.remove(get_iterator_at_pos(list1, 0));
	EXPECT_EQ(list1.size(), 0);

	for (auto&& i : raw_list) {
		list1.push_back(i);
	}
	// [0, 1, 2, 3, 4, 5]
	list1.pop_front();
	list1.pop_front();
	list1.pop_front();
	list1.pop_front();
	list1.pop_front();
	list1.pop_front();

	EXPECT_EQ(list1.size(), 0);
	EXPECT_EQ(list1.cbegin(), list1.cend());
}

TEST(List_Test, MovingBetweenLists)
{
	int raw_list[] = {0, 1, 2, 3, 4, 5};
	List<int> list1;
	List<int> list2;

	list1.push_back(raw_list[0]);
	list1.push_back(raw_list[2]);
	list1.push_back(raw_list[4]);

	list2.push_back(raw_list[1]);
	list2.push_back(raw_list[3]);
	list2.push_back(raw_list[5]);

	// List1: 0, 2, 4
	// List2: 1, 3, 5
	auto itr1 = list1.cbegin();
	auto itr2 = list2.cbegin();
	EXPECT_EQ(*itr1++, 0);
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr2++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);

	list2.splice(list1, list2.begin());
	// List1: 0, 2, 4, 1
	// List2: 3, 5
	itr1 = list1.cbegin();
	itr2 = list2.cbegin();
	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list2.size(), 2);
	EXPECT_EQ(*itr1++, 0);
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr1++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);

	list1.splice(list2, get_iterator_at_pos(list1, 0));
	// List1: 2, 4, 1
	// List2: 3, 5, 0
	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list2.size(), 3);
	itr1 = list1.cbegin();
	itr2 = list2.cbegin();
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr1++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);
	EXPECT_EQ(*itr2++, 0);
}

TEST(List_Test, Finding)
{
	int raw_list[] = {0, 1, 2, 3, 4, 5};
	List<int> list1;
	List<int> list2;
	list1.push_back(raw_list[0]);
	list1.push_back(raw_list[2]);
	list1.push_back(raw_list[4]);

	list2.push_back(raw_list[1]);
	list2.push_back(raw_list[3]);
	list2.push_back(raw_list[5]);

	// List1: 0, 2, 4
	// List2: 1, 3, 5
	EXPECT_EQ(*list1.find(raw_list[0]), 0);
	EXPECT_EQ(*list1.find(raw_list[2]), 2);
	EXPECT_EQ(*list1.find(raw_list[4]), 4);
	EXPECT_EQ(list1.find(raw_list[1]), list1.end());
	EXPECT_EQ(list1.find(raw_list[3]), list1.end());
	EXPECT_EQ(list1.find(raw_list[5]), list1.end());
	EXPECT_EQ(*list2.find(raw_list[1]), 1);
	EXPECT_EQ(*list2.find(raw_list[3]), 3);
	EXPECT_EQ(*list2.find(raw_list[5]), 5);
	EXPECT_EQ(list2.find(raw_list[0]), list2.end());
	EXPECT_EQ(list2.find(raw_list[2]), list2.end());
	EXPECT_EQ(list2.find(raw_list[4]), list2.end());

	list2.remove(get_iterator_at_pos(list2, 0));
	list1.push_back(raw_list[1]);

	EXPECT_EQ(*list1.find(raw_list[1]), 1);
	EXPECT_EQ(list2.find(raw_list[1]), list2.end());
}

TEST(List_Test, MovingAndCopying)
{
	List<int> list1;
	list1.push_back(1);
	list1.push_back(2);
	list1.push_back(3);
	list1.push_back(4);

	List<int> list2 = list1;
	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list2.size(), 4);
	EXPECT_EQ(list1[0], 1);
	EXPECT_EQ(list1[1], 2);
	EXPECT_EQ(list1[2], 3);
	EXPECT_EQ(list1[3], 4);
	EXPECT_EQ(list2[0], 1);
	EXPECT_EQ(list2[1], 2);
	EXPECT_EQ(list2[2], 3);
	EXPECT_EQ(list2[3], 4);

	List<int> list3 = move(list1);
	EXPECT_EQ(list3.size(), 4);
	EXPECT_EQ(list3[0], 1);
	EXPECT_EQ(list3[1], 2);
	EXPECT_EQ(list3[2], 3);
	EXPECT_EQ(list3[3], 4);

	List<int> list4;
	list4 = list3;
	EXPECT_EQ(list3.size(), 4);
	EXPECT_EQ(list3[0], 1);
	EXPECT_EQ(list3[1], 2);
	EXPECT_EQ(list3[2], 3);
	EXPECT_EQ(list3[3], 4);
	EXPECT_EQ(list4.size(), 4);
	EXPECT_EQ(list4[0], 1);
	EXPECT_EQ(list4[1], 2);
	EXPECT_EQ(list4[2], 3);
	EXPECT_EQ(list4[3], 4);

	List<int> list5;
	list5 = move(list4);
	EXPECT_EQ(list5.size(), 4);
	EXPECT_EQ(list5[0], 1);
	EXPECT_EQ(list5[1], 2);
	EXPECT_EQ(list5[2], 3);
	EXPECT_EQ(list5[3], 4);
}