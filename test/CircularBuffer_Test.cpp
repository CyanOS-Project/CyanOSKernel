#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <gtest/gtest.h>
#include <stdlib/CircularBuffer.h>

TEST(CircularBuffer_Test, Initialization)
{
	const int size = 10;
	CircularBuffer<int> cb(size);
	EXPECT_EQ(cb.capacity(), 10);
	EXPECT_EQ(cb.size(), 0);
	EXPECT_EQ(cb.available_size(), 10);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
}

TEST(CircularBuffer_Test, QueueingAndDequeueing)
{
	CircularBuffer<int> cb(10);
	cb.queue(1);
	cb.queue(2);
	cb.queue(3);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 3);
	EXPECT_EQ(cb.available_size(), 7);

	EXPECT_EQ(cb.front(), 1);
	EXPECT_EQ(cb.back(), 3);
	EXPECT_EQ(cb.dequeue(), 1);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 2);
	EXPECT_EQ(cb.available_size(), 8);

	EXPECT_EQ(cb.front(), 2);
	EXPECT_EQ(cb.back(), 3);
	EXPECT_EQ(cb.dequeue(), 2);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 1);
	EXPECT_EQ(cb.available_size(), 9);

	EXPECT_EQ(cb.front(), 3);
	EXPECT_EQ(cb.back(), 3);
	EXPECT_EQ(cb.dequeue(), 3);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 0);
	EXPECT_EQ(cb.available_size(), 10);
}

TEST(CircularBuffer_Test, Wraping)
{
	CircularBuffer<int> cb(5);
	cb.queue(1);
	cb.queue(2);
	cb.queue(3);
	cb.queue(4);
	cb.queue(5);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_TRUE(cb.is_full());
	EXPECT_EQ(cb.size(), 5);
	EXPECT_EQ(cb.available_size(), 0);

	EXPECT_EQ(cb.front(), 1);
	EXPECT_EQ(cb.back(), 5);
	EXPECT_EQ(cb.dequeue(), 1);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 4);
	EXPECT_EQ(cb.available_size(), 1);

	cb.queue(6);
	EXPECT_EQ(cb.front(), 2);
	EXPECT_EQ(cb.back(), 6);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_TRUE(cb.is_full());
	EXPECT_EQ(cb.size(), 5);
	EXPECT_EQ(cb.available_size(), 0);

	EXPECT_EQ(cb.dequeue(), 2);
	EXPECT_EQ(cb.dequeue(), 3);
	EXPECT_EQ(cb.dequeue(), 4);
	EXPECT_EQ(cb.dequeue(), 5);
	EXPECT_FALSE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.size(), 1);
	EXPECT_EQ(cb.available_size(), 4);

	EXPECT_EQ(cb.front(), 6);
	EXPECT_EQ(cb.back(), 6);
	EXPECT_EQ(cb.dequeue(), 6);
	EXPECT_TRUE(cb.is_empty());
	EXPECT_FALSE(cb.is_full());
	EXPECT_EQ(cb.available_size(), 5);
}

TEST(CircularBuffer_Test, MovingAndCopying)
{
	{
		CircularBuffer<int> cb(10);
		cb.queue(1);
		cb.queue(2);
		cb.queue(3);

		CircularBuffer<int> cb2 = cb;
		EXPECT_EQ(cb2.capacity(), 10);
		EXPECT_EQ(cb2.size(), 3);
		EXPECT_EQ(cb2.dequeue(), 1);
		EXPECT_EQ(cb2.dequeue(), 2);
		EXPECT_EQ(cb2.dequeue(), 3);

		EXPECT_EQ(cb.capacity(), 10);
		EXPECT_EQ(cb.size(), 3);
		EXPECT_EQ(cb.dequeue(), 1);
		EXPECT_EQ(cb.dequeue(), 2);
		EXPECT_EQ(cb.dequeue(), 3);
	}

	{
		CircularBuffer<int> cb(10);
		cb.queue(1);
		cb.queue(2);
		cb.queue(3);

		CircularBuffer<int> cb2 = move(cb);
		EXPECT_EQ(cb2.capacity(), 10);
		EXPECT_EQ(cb2.size(), 3);
		EXPECT_EQ(cb2.dequeue(), 1);
		EXPECT_EQ(cb2.dequeue(), 2);
		EXPECT_EQ(cb2.dequeue(), 3);
	}

	{
		CircularBuffer<int> cb(10);
		cb.queue(1);
		cb.queue(2);
		cb.queue(3);

		CircularBuffer<int> cb2(5);
		cb2.queue(5);
		cb2.queue(6);
		cb2.queue(7);
		cb2.queue(8);

		cb2 = cb;

		EXPECT_EQ(cb2.capacity(), 10);
		EXPECT_EQ(cb2.size(), 3);
		EXPECT_EQ(cb2.dequeue(), 1);
		EXPECT_EQ(cb2.dequeue(), 2);
		EXPECT_EQ(cb2.dequeue(), 3);

		EXPECT_EQ(cb.capacity(), 10);
		EXPECT_EQ(cb.size(), 3);
		EXPECT_EQ(cb.dequeue(), 1);
		EXPECT_EQ(cb.dequeue(), 2);
		EXPECT_EQ(cb.dequeue(), 3);
	}

	{
		CircularBuffer<int> cb(10);
		cb.queue(1);
		cb.queue(2);
		cb.queue(3);

		CircularBuffer<int> cb2(5);
		cb2.queue(5);
		cb2.queue(6);
		cb2.queue(7);
		cb2.queue(8);

		cb2 = move(cb);

		EXPECT_EQ(cb2.capacity(), 10);
		EXPECT_EQ(cb2.size(), 3);
		EXPECT_EQ(cb2.dequeue(), 1);
		EXPECT_EQ(cb2.dequeue(), 2);
		EXPECT_EQ(cb2.dequeue(), 3);
	}
}