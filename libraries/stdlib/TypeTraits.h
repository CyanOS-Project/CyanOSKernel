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

template <typename T>[[nodiscard]] constexpr RemoveReference<T>&& move(T&& t)
{
	return static_cast<RemoveReference<T>&&>(t);
}

template <class T>[[nodiscard]] constexpr T&& forward(RemoveReference<T>& param)
{
	return static_cast<T&&>(param);
}

template <class T>[[nodiscard]] constexpr T&& forward(typename RemoveReference_t<T>::type&& param)
{
	return static_cast<T&&>(param);
}