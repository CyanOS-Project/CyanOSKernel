#include "IPv4.h"
#include "ARP.h"
#include "Network/LinkLayer/NetworkAdapter.h"
#include <Algorithms.h>
#include <Buffer.h>
#include <Endianess.h>

IPv4Address IPv4::device_ip_address{};
IPv4Address IPv4::gateway_ip_address{};
IPv4Address IPv4::subnet_mask{};

void IPv4::initialize()
{
	// FIXME: Get an IP address using DHCP.
}

void IPv4::send_ip_packet(IPv4Address destination, IPv4Protocols protocol, const BufferView& data)
{
	Buffer ip_raw_packet{data, sizeof(IPv4Header)};
	auto& ip_packet = ip_raw_packet.convert_to<IPv4Header>();

	ip_packet.version_length = IPv4_VERSION_LENGTH;
	ip_packet.dscp_ecn = 0;
	ip_packet.total_length = to_big_endian(ip_raw_packet.size());
	ip_packet.id = 0;
	ip_packet.flags_fragment_offset = 0;
	ip_packet.time_to_live = 64;
	ip_packet.protocol = static_cast<u8>(protocol);
	ip_packet.header_checksum = 0;

	device_ip_address.copy(ip_packet.src_ip);
	destination.copy(ip_packet.dst_ip);

	ip_packet.header_checksum = calculate_checksum(BufferView{ip_raw_packet, 0, sizeof(IPv4Header)});

	auto& destination_mac = destination_mac_lookup(destination);

	NetworkAdapter::default_network_adapter->send_frame(ProtocolType::IPv4, destination_mac, ip_raw_packet);
}

void IPv4::handle_ip_packet(const BufferView& data)
{
	auto& ip_packet = data.const_convert_to<IPv4Header>();

	if (!is_packet_ok(ip_packet)) {
		return;
	}

	// Note: some packets have the optional fields so you have to calculate data field position right.

	switch (static_cast<IPv4Protocols>(ip_packet.protocol)) {
		case IPv4Protocols::ICMP: {
			break;
		}

		case IPv4Protocols::TCP: {
			break;
		}

		case IPv4Protocols::UDP: {
			break;
		}

		default:
			break;
	}
}

const MACAddress& IPv4::destination_mac_lookup(IPv4Address address)
{
	if (address == IPv4Address::Broadcast) {
		return MACAddress::Broadcast;
	} else if (is_in_local_subnet(address)) {
		return ARP::mac_address_lookup(address);
	} else {
		return ARP::mac_address_lookup(gateway_ip_address);
	}
}

bool IPv4::is_in_local_subnet(IPv4Address address)
{
	if (address.mask(subnet_mask) == address) {
		return true;
	} else {
		return false;
	}
}

bool IPv4::is_packet_ok(const IPv4Header& packet)
{
	// TODO: check integrity ip packet here.
	return true;
}

u16 IPv4::calculate_checksum(const BufferView& data)
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

IPv4Address IPv4::IP()
{
	return device_ip_address;
}
