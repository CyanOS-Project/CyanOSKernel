#include "UDP.h"
#include "Network/NetworkLayer/IPv4.h"
#include <Algorithms.h>
#include <Buffer.h>
#include <Endianess.h>

void UDP::send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data)
{
	Buffer udp_raw_segment{sizeof(UDPHeader) + data.size()};
	auto& udp_segment = udp_raw_segment.convert_to<UDPHeader>();

	udp_segment.source_port = to_big_endian(u16(68));
	udp_segment.destination_port = to_big_endian(dest_port);
	udp_segment.total_length = to_big_endian(u16(sizeof(UDPHeader) + data.size()));
	udp_segment.checksum = 0;

	udp_raw_segment.fill_from(data.ptr(), sizeof(UDPHeader), data.size());

	udp_segment.checksum = calculate_checksum(udp_raw_segment);

	IPv4::send_ip_packet(dest_ip, IPv4Protocols::UDP, udp_raw_segment);
}

void UDP::handle(const IPv4Address& src_ip, const BufferView& data)
{
	// TODO
}

u16 UDP::calculate_checksum(const BufferView& data)
{
	auto* u16_array = reinterpret_cast<const u16*>(data.ptr());
	size_t u16_array_size = number_of_words<u16>(data.size());

	u32 sum = 0;
	for (size_t i = 0; i < u16_array_size; i++) {
		sum += to_big_endian(u16_array[i]);
	}

	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian(u16(~u16(sum)));
}