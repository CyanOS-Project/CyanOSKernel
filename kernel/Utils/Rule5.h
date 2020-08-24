#pragma once

#define NON_COPYABLE(x)                                                                                                \
	x(const x&) = delete;                                                                                              \
	x& operator=(const x&) = delete;

#define NON_MOVABLE(x)                                                                                                 \
	x(x&&) = delete;                                                                                                   \
	x& operator=(x&&) = delete;
