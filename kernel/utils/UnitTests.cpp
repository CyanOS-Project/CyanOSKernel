#include "UnitTests.h"

/************************************************************************************************
 * This is testing is temporary on the OS, until i find a proper way to do  testing in the host
 * **********************************************************************************************/

void UnitTest_CircularQueue()
{
	printf("\n\n");
	printf("************************* Testing: CircularQueue ****************\n");
	CircularQueue<int>* list = new CircularQueue<int>;
	CircularQueue<int>* list2 = new CircularQueue<int>;
	ASSERT_TEST(list->size(), 0);
	list->push_back(1);
	list->push_back(2);
	list->push_back(3);
	ASSERT_TEST(list->size(), 3);
	int index = 1;
	for (CircularQueue<int>::Iterator i = list->begin(); i != list->end(); i++) {
		ASSERT_TEST(*i, index);
		++index;
	}
	list->remove(1);
	ASSERT_TEST((*list)[0], 1);
	ASSERT_TEST((*list)[1], 3);
	list->move_head_to_other_list(list2);
	ASSERT_TEST(list->size(), 1);
	ASSERT_TEST(list2->size(), 1);
	ASSERT_TEST((*list)[0], 3);
	ASSERT_TEST((*list2)[0], 1);
	CircularQueue<int>::Iterator i = list->begin();
	i.move_cursor(0);
	list->move_to_other_list(list2, i);
	ASSERT_TEST(list->size(), 0);
	ASSERT_TEST(list2->size(), 2);
	ASSERT_TEST((*list2)[0], 1);
	ASSERT_TEST((*list2)[1], 3);
	printf("*****************************************************************\n");
	printf("\n\n");
}

void UnitTest_Bitmap()
{
	printf("\n\n");
	printf("************************* Testing: Bitmaps **********************\n");
	Bitmap bm(10);
	ASSERT_TEST(bm.find_first_unused(5), 0);
	ASSERT_TEST(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(2);
	ASSERT_TEST(bm.find_first_unused(5), 3);
	ASSERT_TEST(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(3, 4);
	ASSERT_TEST(bm.find_first_unused(5), BITMAP_NO_BITS_LEFT);
	ASSERT_TEST(bm.find_first_used(5), 2);
	bm.set_unused(2, 5);
	ASSERT_TEST(bm.find_first_unused(5), 0);
	ASSERT_TEST(bm.find_first_used(5), BITMAP_NO_BITS_LEFT);
	bm.set_used(2);
	bm.set_used(2);
	ASSERT_TEST(bm.find_first_unused(4), BITMAP_NO_BITS_LEFT);
	ASSERT_TEST(bm.find_first_used(4), BITMAP_NO_BITS_LEFT);
	printf("*****************************************************************\n");
	printf("\n\n");
}