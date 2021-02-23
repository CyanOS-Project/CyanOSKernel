#pragma once
#include "Rule5.h"
#include "TypeTraits.h"
#include "Types.h"

template <size_t index, typename Item> class TupleLeaf
{
  public:
	DEFAULT_COPY(TupleLeaf)
	DEFAULT_MOVE(TupleLeaf)
	template <typename T> TupleLeaf(T&& value) : m_value{forward<T>(value)} {};
	Item& get() { return m_value; }
	const Item& get() const { return m_value; }

  private:
	Item m_value;
};

template <size_t index, typename... TailItems> class TupleImpl
{
};

template <size_t index, typename HeadItem, typename... TailItems>
class TupleImpl<index, HeadItem, TailItems...> : public TupleLeaf<index, HeadItem>,
                                                 public TupleImpl<index + 1, TailItems...>
{
  public:
	DEFAULT_COPY(TupleImpl)
	DEFAULT_MOVE(TupleImpl)

	template <typename FirstArg, typename... RestArgs>
	TupleImpl(FirstArg&& arg, RestArgs&&... args) :
	    TupleLeaf<index, HeadItem>(forward<HeadItem>(arg)),
	    TupleImpl<index + 1, TailItems...>(forward<TailItems>(args)...)
	{
	}
};

template <typename HeadItem, typename... TailItems> class Tuple : public TupleImpl<0, HeadItem, TailItems...>
{
  public:
	template <typename... RestArgs>
	Tuple(RestArgs&&... args) : TupleImpl<0, HeadItem, TailItems...>(forward<RestArgs>(args)...)
	{
	}

	DEFAULT_COPY(Tuple)
	DEFAULT_MOVE(Tuple)

	template <size_t index> auto& get()
	{
		using type = typename ExtractType<index, HeadItem, TailItems...>::type;
		return (static_cast<TupleLeaf<index, type>&>(*this)).get();
	}

	template <size_t index> const auto& get() const
	{
		using type = typename ExtractType<index, HeadItem, TailItems...>::type;
		return (static_cast<TupleLeaf<index, type>&>(*this)).get();
	}

	template <typename... Args> bool operator==(const Tuple<Args...>& t2) const
	{
		return compare_tuple<sizeof...(Args) - 1>(t2);
	}

	template <typename... Args> bool operator!=(const Tuple<Args...>& t2) const { return !operator==(t2); }

  private:
	template <size_t index, typename L, typename... Args> struct ExtractType {
		using type = typename ExtractType<index - 1, Args...>::type;
	};

	template <typename L, typename... Args> struct ExtractType<0, L, Args...> {
		using type = L;
	};

	template <size_t index, typename... Args> bool compare_tuple(const Tuple<Args...>& t2)
	{
		if constexpr (index == 0) {
			return get<0>() == t2.get<0>();
		} else {
			return get<index>() == t2.get<index>() && compare_tuple<index - 1>(t2);
		}
	}
};
