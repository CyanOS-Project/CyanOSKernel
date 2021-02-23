#pragma once

#define NON_COPYABLE(x)                                                                                                \
	x(const x&) = delete;                                                                                              \
	x& operator=(const x&) = delete;

#define NON_MOVABLE(x)                                                                                                 \
	x(x&&) = delete;                                                                                                   \
	x& operator=(x&&) = delete;

#define DEFAULT_COPY(x)                                                                                                \
	x(const x&) = default;                                                                                             \
	x& operator=(const x&) = default;

#define DEFAULT_MOVE(x)                                                                                                \
	x(x&&) = default;                                                                                                  \
	x& operator=(x&&) = default;