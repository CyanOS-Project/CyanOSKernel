#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <gtest/gtest.h>
#include <stdlib/IntrusiveList.h>

struct Int : public IntrusiveListNode<Int> {
	int x;
	Int(int a) : x{a} {};
	bool operator==(const Int& rhs) const { return x == rhs.x; }
};

TEST(IntrusiveList_Test, Iteration)
{
	Int raw_list[] = {0, 1, 2, 3, 4, 5};
	IntrusiveList<Int> list1;
	for (auto&& i : raw_list) {
		list1.push_back(i);
	}
	auto get_last_itr = [&list1]() {
		auto itr = list1.begin();
		auto tmp = itr;
		while (++tmp != list1.end()) {
			itr = tmp;
		}
		return itr;
	};

	auto itr = list1.begin();
	EXPECT_EQ(*itr++, 0);
	EXPECT_EQ(*itr++, 1);
	EXPECT_EQ(*itr++, 2);
	EXPECT_EQ(*itr++, 3);
	EXPECT_EQ(*itr++, 4);
	EXPECT_EQ(*itr++, 5);
	EXPECT_EQ(itr, list1.end());

	itr = list1.begin();
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);
	EXPECT_EQ(*itr, 5);
	EXPECT_EQ(++itr, list1.end());

	itr = get_last_itr();
	EXPECT_EQ(*itr--, 5);
	EXPECT_EQ(*itr--, 4);
	EXPECT_EQ(*itr--, 3);
	EXPECT_EQ(*itr--, 2);
	EXPECT_EQ(*itr--, 1);
	EXPECT_EQ(*itr--, 0);
	EXPECT_EQ(itr, list1.end());
	itr = get_last_itr();
	EXPECT_EQ(*--itr, 4);
	EXPECT_EQ(*--itr, 3);
	EXPECT_EQ(*--itr, 2);
	EXPECT_EQ(*--itr, 1);
	EXPECT_EQ(*--itr, 0);
	EXPECT_EQ(--itr, list1.end());

	// If you get to end(), you can't come back by ++ or --.
	// end() that is after the last element.
	itr = get_last_itr();
	EXPECT_EQ(++itr, list1.end());
	EXPECT_EQ(++itr, list1.end());
	EXPECT_EQ(--itr, list1.end());
	EXPECT_EQ(--itr, list1.end());

	itr = get_last_itr();
	EXPECT_EQ(*(itr++), 5);
	EXPECT_EQ(itr++, list1.end());
	EXPECT_EQ(itr, list1.end());
	EXPECT_EQ(itr--, list1.end());
	EXPECT_EQ(itr, list1.end());

	// end() that is before the first element.
	itr = list1.begin();
	EXPECT_EQ(--itr, list1.end());
	EXPECT_EQ(--itr, list1.end());
	EXPECT_EQ(++itr, list1.end());
	EXPECT_EQ(++itr, list1.end());

	itr = list1.begin();
	EXPECT_EQ(*(itr--), 0);
	EXPECT_EQ(itr--, list1.end());
	EXPECT_EQ(itr, list1.end());
	EXPECT_EQ(itr++, list1.end());
	EXPECT_EQ(itr, list1.end());
}

TEST(IntrusiveList_Test, Removing)
{
	Int raw_list[] = {0, 1, 2, 3, 4, 5};
	IntrusiveList<Int> list1;
	for (auto&& i : raw_list) {
		list1.push_back(i);
	}
	// [0, 1, 2, 3, 4, 5].
	auto itr = list1.begin();
	EXPECT_EQ(list1.size(), 6);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 2);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);

	list1.remove(raw_list[2]);
	// [0, 1, 3, 4, 5].
	itr = list1.begin();
	EXPECT_EQ(list1.size(), 5);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);
	EXPECT_EQ(*++itr, 5);

	list1.remove(raw_list[5]);
	// [0, 1, 3, 4].
	itr = list1.begin();
	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(*itr, 0);
	EXPECT_EQ(*++itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);

	list1.remove(raw_list[0]);
	// [1, 3, 4].
	itr = list1.begin();
	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(*itr, 1);
	EXPECT_EQ(*++itr, 3);
	EXPECT_EQ(*++itr, 4);

	list1.remove(raw_list[1]);
	list1.remove(raw_list[4]);
	// [3].
	itr = list1.begin();
	EXPECT_EQ(list1.size(), 1);
	EXPECT_EQ(*itr, 3);
	// []
	list1.remove(raw_list[3]);
	EXPECT_EQ(list1.size(), 0);
}

TEST(IntrusiveList_Test, MovingBetweenLists)
{
	Int raw_list[] = {0, 1, 2, 3, 4, 5};
	IntrusiveList<Int> list1;
	IntrusiveList<Int> list2;
	list1.push_back(raw_list[0]);
	list1.push_back(raw_list[2]);
	list1.push_back(raw_list[4]);

	list2.push_back(raw_list[1]);
	list2.push_back(raw_list[3]);
	list2.push_back(raw_list[5]);

	// List1: 0, 2, 4
	// List2: 1, 3, 5
	auto itr1 = list1.begin();
	auto itr2 = list2.begin();
	EXPECT_EQ(*itr1++, 0);
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr2++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);

	list2.remove(raw_list[1]);
	list1.push_back(raw_list[1]);
	// List1: 0, 2, 4, 1
	// List2: 3, 5
	itr1 = list1.begin();
	itr2 = list2.begin();
	EXPECT_EQ(list1.size(), 4);
	EXPECT_EQ(list2.size(), 2);
	EXPECT_EQ(*itr1++, 0);
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr1++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);

	list1.remove(raw_list[0]);
	list2.push_back(raw_list[0]);
	// List1: 2, 4, 1
	// List2: 3, 5, 0
	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list2.size(), 3);
	itr1 = list1.begin();
	itr2 = list2.begin();
	EXPECT_EQ(*itr1++, 2);
	EXPECT_EQ(*itr1++, 4);
	EXPECT_EQ(*itr1++, 1);
	EXPECT_EQ(*itr2++, 3);
	EXPECT_EQ(*itr2++, 5);
	EXPECT_EQ(*itr2++, 0);
}

TEST(IntrusiveList_Test, Finding)
{
	Int raw_list[] = {0, 1, 2, 3, 4, 5};
	IntrusiveList<Int> list1;
	IntrusiveList<Int> list2;
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
	EXPECT_EQ(list2.find(raw_list[0]), list1.end());
	EXPECT_EQ(list2.find(raw_list[2]), list1.end());
	EXPECT_EQ(list2.find(raw_list[4]), list1.end());

	list2.remove(raw_list[1]);
	list1.push_back(raw_list[1]);

	EXPECT_EQ(*list1.find(raw_list[1]), 1);
	EXPECT_EQ(list2.find(raw_list[1]), list1.end());
}