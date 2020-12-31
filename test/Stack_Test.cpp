#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
#endif

#include <gtest/gtest.h>
#include <stdlib/Stack.h>

TEST(Stack_Test, Initialization)
{
	Stack<int> stack(10);
	EXPECT_EQ(stack.capacity(), 10);
	EXPECT_EQ(stack.size(), 0);
	EXPECT_TRUE(stack.is_empty());
	EXPECT_FALSE(stack.is_full());
}

TEST(Stack_Test, PushingAndPoping)
{
	Stack<int> stack(10);
	stack.push(1);
	stack.push(2);
	stack.push(3);
	EXPECT_FALSE(stack.is_empty());
	EXPECT_FALSE(stack.is_full());
	EXPECT_EQ(stack.size(), 3);

	EXPECT_EQ(stack.back(), 3);
	EXPECT_EQ(stack.pop(), 3);
	EXPECT_FALSE(stack.is_empty());
	EXPECT_FALSE(stack.is_full());
	EXPECT_EQ(stack.size(), 2);

	EXPECT_EQ(stack.back(), 2);
	EXPECT_EQ(stack.pop(), 2);
	EXPECT_FALSE(stack.is_empty());
	EXPECT_FALSE(stack.is_full());
	EXPECT_EQ(stack.size(), 1);

	EXPECT_EQ(stack.back(), 1);
	EXPECT_EQ(stack.pop(), 1);
	EXPECT_TRUE(stack.is_empty());
	EXPECT_FALSE(stack.is_full());
	EXPECT_EQ(stack.size(), 0);
}

TEST(Stack_Test, MovingAndCopying)
{
	{
		Stack<int> stack(10);
		stack.push(1);
		stack.push(2);
		stack.push(3);

		Stack<int> stack2 = stack;
		EXPECT_EQ(stack2.capacity(), 10);
		EXPECT_EQ(stack2.size(), 3);
		EXPECT_EQ(stack2.pop(), 3);
		EXPECT_EQ(stack2.pop(), 2);
		EXPECT_EQ(stack2.pop(), 1);

		EXPECT_EQ(stack.capacity(), 10);
		EXPECT_EQ(stack.size(), 3);
		EXPECT_EQ(stack.pop(), 3);
		EXPECT_EQ(stack.pop(), 2);
		EXPECT_EQ(stack.pop(), 1);
	}

	{
		Stack<int> stack(10);
		stack.push(1);
		stack.push(2);
		stack.push(3);

		Stack<int> stack2 = move(stack);
		EXPECT_EQ(stack2.capacity(), 10);
		EXPECT_EQ(stack2.size(), 3);
		EXPECT_EQ(stack2.pop(), 3);
		EXPECT_EQ(stack2.pop(), 2);
		EXPECT_EQ(stack2.pop(), 1);
	}

	{
		Stack<int> stack(10);
		stack.push(1);
		stack.push(2);
		stack.push(3);

		Stack<int> stack2(5);
		stack2.push(5);
		stack2.push(6);
		stack2.push(7);
		stack2.push(8);

		stack2 = stack;

		EXPECT_EQ(stack2.capacity(), 10);
		EXPECT_EQ(stack2.size(), 3);
		EXPECT_EQ(stack2.pop(), 3);
		EXPECT_EQ(stack2.pop(), 2);
		EXPECT_EQ(stack2.pop(), 1);

		EXPECT_EQ(stack.capacity(), 10);
		EXPECT_EQ(stack.size(), 3);
		EXPECT_EQ(stack.pop(), 3);
		EXPECT_EQ(stack.pop(), 2);
		EXPECT_EQ(stack.pop(), 1);
	}

	{
		Stack<int> stack(10);
		stack.push(1);
		stack.push(2);
		stack.push(3);

		Stack<int> stack2(5);
		stack2.push(5);
		stack2.push(6);
		stack2.push(7);
		stack2.push(8);

		stack2 = move(stack);

		EXPECT_EQ(stack2.capacity(), 10);
		EXPECT_EQ(stack2.size(), 3);
		EXPECT_EQ(stack2.pop(), 3);
		EXPECT_EQ(stack2.pop(), 2);
		EXPECT_EQ(stack2.pop(), 1);
	}
}