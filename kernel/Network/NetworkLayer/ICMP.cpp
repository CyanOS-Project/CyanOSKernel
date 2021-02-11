#include "ICMP.h"
#include <Buffer.h>
#include <Endianess.h>

ICMP::ICMP(Network& network) : m_network{network} {}

void ICMP::send_echo_request(const IPv4Address& address)
{
	Buffer icmp_raw_packet{sizeof(EchoPacket)};
	auto& icmp_header = icmp_raw_packet.convert_to<EchoPacket>();

	icmp_header.type = static_cast<u8>(ICMPType::EchoRequest);
	icmp_header.code = 0;
	icmp_header.checksum = 0;

	icmp_header.echo_id = 'a';
	icmp_header.echo_seq = 'z';

	icmp_header.checksum = calculate_checksum(icmp_raw_packet);

	IPv4::send_ip_packet(address, IPv4Protocols::ICMP, icmp_raw_packet);
}

void ICMP::handle_icmp_reply()
{
	// TODO
}

u16 ICMP::calculate_checksum(const BufferView& data)
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