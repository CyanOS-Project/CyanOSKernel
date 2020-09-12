#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include "CircularBuffer.h"
#include <gtest/gtest.h>

TEST(CircularBuffer_Test, Initialization)
{
	const int size = 10;
	CircularBuffer<int> cb(size);
	EXPECT_EQ(cb.capacity(), 10);
	EXPECT_EQ(cb.size(), 0);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
}

TEST(CircularBuffer_Test, QueueingAndDequeueing)
{
	const int size = 10;
	CircularBuffer<int> cb(size);
	cb.queue(1);
	cb.queue(2);
	cb.queue(3);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 3);

	EXPECT_EQ(cb.dequeue(), 1);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 2);

	EXPECT_EQ(cb.dequeue(), 2);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 1);

	EXPECT_EQ(cb.dequeue(), 3);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 0);
}

TEST(CircularBuffer_Test, Wraping)
{
	const int size = 5;
	CircularBuffer<int> cb(size);
	cb.queue(1);
	cb.queue(2);
	cb.queue(3);
	cb.queue(4);
	cb.queue(5);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_TRUE(cb.is_full());
	EXPECT_EQ(cb.size(), 5);

	EXPECT_EQ(cb.dequeue(), 1);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 4);

	cb.queue(6);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_TRUE(cb.is_full());
	EXPECT_EQ(cb.size(), 5);

	EXPECT_EQ(cb.dequeue(), 2);
	EXPECT_EQ(cb.dequeue(), 3);
	EXPECT_EQ(cb.dequeue(), 4);
	EXPECT_EQ(cb.dequeue(), 5);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 1);

	EXPECT_EQ(cb.dequeue(), 6);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
}