#include "math.h"

size_t abs(int num)
{
	if (num > 0) {
		return static_cast<size_t>(num);
	} else {
		return static_cast<size_t>(-num);
	}
}