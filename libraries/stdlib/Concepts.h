#pragma once

template <class, class> constexpr bool is_equal = false;
template <class T> constexpr bool is_equal<T, T> = true;
template <typename T1, typename T2> concept EqualTo = is_equal<T1, T2>;

template <class> constexpr bool is_ptr = false;
template <class T> constexpr bool is_ptr<T*> = true;
template <typename T> concept IsPointer = is_ptr<T>;

template <class> constexpr bool is_lvalue_reference = false;
template <class T> constexpr bool is_lvalue_reference<T&> = true;
template <typename T> concept IslvalueReference = is_lvalue_reference<T>;

template <class> constexpr bool is_rvalue_reference = false;
template <class T> constexpr bool is_rvalue_reference<T&&> = true;
template <typename T> concept IsRvalueReference = is_rvalue_reference<T>;

template <typename T> concept EqualityComparable = requires(T x, T y)
{
	{
		x == y
	}
	->EqualTo<bool>;
	{
		x != y
	}
	->EqualTo<bool>;
};

template <typename T> concept GreaterComparable = requires(T x, T y)
{
	{
		x > y
	}
	->EqualTo<bool>;
};

template <typename T> concept LessComparable = requires(T x, T y)
{
	{
		x < y
	}
	->EqualTo<bool>;
};

template <typename T> concept GreaterOrEqualComparable = requires(T x, T y)
{
	{
		x >= y
	}
	->EqualTo<bool>;
};

template <typename T> concept LessOrEqualComparable = requires(T x, T y)
{
	{
		x <= y
	}
	->EqualTo<bool>;
};

template <typename T> concept Incrementable = requires(T x)
{
	{
		x++
	}
	->EqualTo<T>;
	{
		++x
	}
	->EqualTo<T&>;
};

template <typename T> concept Decrementable = requires(T x)
{
	{
		x--
	}
	->EqualTo<T>;
	{
		--x
	}
	->EqualTo<T&>;
};

template <typename T> concept AdditionalAssignmentable = requires(T x, int a)
{
	{
		x += a
	}
	->EqualTo<T&>;
};

template <typename T> concept SubtractionalAssignmentable = requires(T x, int a)
{
	{
		x -= a
	}
	->EqualTo<T&>;
};

template <typename T> concept Addable = requires(T x, int a)
{
	{
		x + a
	}
	->EqualTo<T>;
};

template <typename T> concept Subtractable = requires(T x, int a)
{
	{
		x - a
	}
	->EqualTo<T>;
};

template <typename T> concept Dereferenceable = requires(T x)
{
	{
		*x
	}
	->IslvalueReference<>;
};

template <typename T> concept MemberDereferenceable = requires(T x)
{
	{
		x.operator->()
	}
	->IsPointer<>;
};

template <typename T> concept Addressable = requires(T x)
{
	{
		&x
	}
	->IsPointer<>;
};

template <typename T> concept Subscriptable = requires(T x, int a)
{
	{
		x[a]
	}
	->IslvalueReference<>;
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