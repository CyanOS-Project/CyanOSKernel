#include "network_algorithms.h"
#include "algorithms.h"
#include "endianess.h"

u16 checksum(const BufferView& data)
{
	auto* u16_array = reinterpret_cast<const u16*>(data.ptr());
	size_t u16_array_size = number_of_words<u16>(data.size());

	u32 sum = 0;
	for (size_t i = 0; i < u16_array_size; i++) {
		sum += to_big_endian<u16>(u16_array[i]);
	}

	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian<u16>(~u16(sum));
}

u16 network_word16(u16 value)
{
	return to_big_endian<u16>(value);
}

u32 network_word32(u32 value)
{
	return to_big_endian<u32>(value);
}