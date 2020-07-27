#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define _UNUSED_PARAM(x) x tmp __attribute__((__unused__))
#define UNUSED(expr)     (void)(expr)