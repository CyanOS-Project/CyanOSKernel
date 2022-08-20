#pragma once

template <class T> struct RemoveReference_t {
	using type = T;
};
template <class T> struct RemoveReference_t<T&> {
	using type = T;
};
template <class T> struct RemoveReference_t<T&&> {
	using type = T;
};
template <class T> using RemoveReference = typename RemoveReference_t<T>::type;

template <class T> struct RemoveConst_t {
	using type = T;
};
template <class T> struct RemoveConst_t<const T> {
	using type = T;
};
template <class T> using RemoveConst = typename RemoveConst_t<T>::type;

template <class T> struct AddReference_t {
	using Lvalue = T&;
	using Rvalue = T&&;
};
template <> struct AddReference_t<void> {
	using Lvalue = void;
	using Rvalue = void;
};
template <class T> using AddRvalueReference = typename AddReference_t<T>::Rvalue;
template <class T> using AddLvalueReference = typename AddReference_t<T>::Lvalue;

template <class T> AddRvalueReference<T> DeclareValue();

template <typename T>[[nodiscard]] constexpr decltype(auto) move(T&& t)
{
	return static_cast<RemoveReference<T>&&>(t);
}

template <class T>[[nodiscard]] constexpr decltype(auto) forward(RemoveReference<T>& param)
{
	return static_cast<T&&>(param);
}

template <class T>[[nodiscard]] constexpr decltype(auto) forward(RemoveReference<T>&& param)
{
	return static_cast<T&&>(param);
}