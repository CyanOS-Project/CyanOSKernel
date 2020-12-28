#pragma once

template <typename T> concept EqualityComparable = requires(T x, T y)
{
	x == y;
	x != y;
};

template <typename T> concept GreaterComparable = requires(T x, T y)
{
	x > y;
};

template <typename T> concept LessComparable = requires(T x, T y)
{
	x < y;
};

template <typename T> concept GreaterOrEqualComparable = requires(T x, T y)
{
	x >= y;
};

template <typename T> concept LessOrEqualComparable = requires(T x, T y)
{
	x <= y;
};

template <typename T> concept Incrementable = requires(T x)
{
	x++;
	++x;
};

template <typename T> concept Decrementable = requires(T x)
{
	x--;
	--x;
};

template <typename T> concept AdditionalAssignmentable = requires(T x, int a)
{
	x += a;
	x += -a;
};

template <typename T> concept SubtractionalAssignmentable = requires(T x, int a)
{
	x -= a;
	x -= -a;
};

template <typename T> concept Addable = requires(T x, int a)
{
	x + a;
	x + (-a);
};

template <typename T> concept Subtractable = requires(T x, int a)
{
	x - a;
	x - (-a);
};

template <typename T> concept Dereferenceable = requires(T x)
{
	*x;
};

template <typename T> concept MemberDereferenceable = requires(T x)
{
	x.operator->();
};

template <typename T> concept Addressable = requires(T x)
{
	&x;
};

template <typename T> concept Subscriptable = requires(T x, int a)
{
	x[a];
};

template <typename From, typename To> concept Convertable_to = requires(From x)
{
	static_cast<To>(x);
};

template <typename T>
concept is_ForwardIterator = Incrementable<T>&& EqualityComparable<T>&& Dereferenceable<T>&& MemberDereferenceable<T>;

template <typename T> concept is_BidirectionalIteratorIterator = is_ForwardIterator<T>&& Decrementable<T>;

template <typename T> concept is_RandomAccessIterator = is_BidirectionalIteratorIterator<T>&& Decrementable<T>;

template <typename MutableIterator, typename ConstIterator>
concept validate_ForwardIterator = is_ForwardIterator<MutableIterator>&& is_ForwardIterator<ConstIterator>&&
    Convertable_to<MutableIterator, ConstIterator>;

template <typename MutableIterator, typename ConstIterator>
concept validate_BidirectionalIterator = is_BidirectionalIteratorIterator<MutableIterator>&&
    is_BidirectionalIteratorIterator<ConstIterator>&& Convertable_to<MutableIterator, ConstIterator>;

template <typename MutableIterator, typename ConstIterator>
concept validate_RandomAccessIterator = is_RandomAccessIterator<MutableIterator>&&
    is_RandomAccessIterator<ConstIterator>&& Convertable_to<MutableIterator, ConstIterator>;