#pragma once

template <class T> struct RemoveReference_t {
	using type = T;
};
template <class T> using RemoveReference = typename RemoveReference_t<T>::type;