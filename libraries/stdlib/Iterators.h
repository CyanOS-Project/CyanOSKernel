#pragma once
#include "TypeTraits.h"
#include "Types.h"

// TODO: Replace this with concepts.

template <typename T> struct ForwardIterator {
	ForwardIterator()
	{
		static_assert(sizeof(decltype(DeclareValue<T>()++)), "!");
		static_assert(sizeof(decltype(++DeclareValue<T>())), "!");
		static_assert(sizeof(decltype(DeclareValue<T>() == DeclareValue<T>())), "!");
		static_assert(sizeof(decltype(DeclareValue<T>() != DeclareValue<T>())), "!");
		static_assert(sizeof(decltype(*DeclareValue<T>())), "!");
		static_assert(sizeof(decltype(DeclareValue<T>().operator->())), "!");
	}
};

template <typename T> struct BidirectionalIterator : public ForwardIterator<T> {
	BidirectionalIterator()
	{
		static_assert(sizeof(decltype(DeclareValue<T>()--)), "!");
		static_assert(sizeof(decltype(--DeclareValue<T>())), "!");
	}
};

template <typename T> struct RandomAccessIterator : public BidirectionalIterator<T> {
	RandomAccessIterator()
	{
		static_assert(sizeof(decltype(DeclareValue<T>() + 0)), "!");
		static_assert(sizeof(decltype(DeclareValue<T>() - 0)), "!");
	}
};