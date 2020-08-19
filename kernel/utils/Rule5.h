#pragma once

#define NON_COPYABLE(x)                                                                                                \
	List(const List&) = delete;                                                                                        \
	x& operator=(const x&) = delete;

#define NON_MOVABLE(x)                                                                                                 \
	List(List&&) = delete;                                                                                             \
	x& operator=(x&&) = delete;
