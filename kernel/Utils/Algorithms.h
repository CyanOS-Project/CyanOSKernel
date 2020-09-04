#pragma once
#include "Types.h"

// Credits: some of these function are partially copied from cppreference.com.

/*
TODO: algorithm functions:
- sort
- equal_range
- for_adject_pairs for_all_pairs
-  split
- make_heap push_heap pop_heap sort_heap is_heap
- rotate reverse is_sorted
- equal is_permutation
- uninitialized_fill uninitialized_copy uninitialized_mode
 */

template <typename T> T& max(const T& a, const T& b)
{
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

template <typename T> const T& min(const T& a, const T& b)
{
	if (a > b) {
		return b;
	} else {
		return a;
	}
}

inline size_t align_to(size_t num, size_t alignment, bool align_zero = true)
{
	if (num == 0)
		return align_zero ? alignment : 0;
	else if ((num % alignment) == 0)
		return num;
	else
		return num + alignment - (num % alignment);
}

template <typename T> struct RemoveReference {
	typedef T Type;
};
template <class T> struct RemoveReference<T&> {
	typedef T Type;
};
template <class T> struct RemoveReference<T&&> {
	typedef T Type;
};

template <typename T> constexpr T&& move(T& t)
{
	return static_cast<T&&>(t);
}

template <class T> constexpr T&& forward(typename RemoveReference<T>::Type& param)
{
	return static_cast<T&&>(param);
}

template <typename T> T& swap(const T& a, const T& b)
{
	T c(move(a));
	a = move(b);
	b = move(c);
}

template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	while (first != last) {
		*result = *first;
		++result;
		++first;
	}
	return result;
}

template <class InputIterator, class OutputIterator>
OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)
{
	while (first != last) {
		*result = move(*first);
		++result;
		++first;
	}
	return result;
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                     BidirectionalIterator2 d_last)
{
	while (first != last) {
		*(--d_last) = *(--last);
	}
	return d_last;
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                     BidirectionalIterator2 d_last)
{
	while (first != last) {
		*(--d_last) = move(*(--last));
	}
	return d_last;
}

template <class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred)
{
	while (first != last) {
		if (pred(*first))
			*d_first++ = *first;
		first++;
	}
	return d_first;
}

template <typename InIter, typename OutIter, typename Pred>
OutIter copy_while(InIter first, InIter last, OutIter dest, Pred pred)
{
	for (; first != last && pred(*first); ++first, ++dest)
		*dest = *first;
	return dest;
}

template <class InputIterator, class T> InputIterator find(InputIterator first, InputIterator last, const T& val)
{
	while (first != last) {
		if (*first == val)
			return first;
		++first;
	}
	return last;
}

template <class InputIt, class UnaryPredicate> constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate p)
{
	for (; first != last; ++first) {
		if (p(*first)) {
			return first;
		}
	}
	return last;
}

template <class InputIt, class UnaryPredicate>
constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
{
	for (; first != last; ++first) {
		if (!q(*first)) {
			return first;
		}
	}
	return last;
}

template <class InputIt, class UnaryPredicate> constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return find_if_not(first, last, p) == last;
}

template <class InputIt, class UnaryPredicate> constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return find_if(first, last, p) != last;
}

template <class InputIt, class UnaryPredicate> constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return find_if(first, last, p) == last;
}

template <class InputIterator, class T> unsigned count(InputIterator first, InputIterator last, const T& val)
{
	unsigned ret = 0;
	while (first != last) {
		if (*first == val)
			++ret;
		++first;
	}
	return ret;
}

template <class InputIt, class T> constexpr T accumulate(InputIt first, InputIt last, T init)
{
	for (; first != last; ++first) {
		init = move(init) + *first;
	}
	return init;
}