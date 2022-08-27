#include "ipv4.h"
#include "arp.h"
#include "network/link_layer/network_adapter.h"
#include "network/network.h"
#include "network/network_layer/icmp.h"
#include "network/transport_layer/tcp.h"
#include "network/transport_layer/udp.h"
#include <algorithms.h>
#include <buffer.h>
#include <network_algorithms.h>

IPv4::IPv4(Network& network) : m_network{network}, m_dhcp{network} {}

void IPv4::start()
{
	auto dhcp_info = m_dhcp.request_dhcp_information();
	m_device_ip_address = dhcp_info.device_ip;
	m_gateway_ip_address = dhcp_info.gateway_ip;
	m_subnet_mask = dhcp_info.subnet_mask;
}

void IPv4::send(IPv4Address destination, IPv4Protocols protocol, const BufferView& data)
{
	Buffer ip_raw_packet{data, sizeof(IPv4Header)};
	auto& ip_packet = ip_raw_packet.convert_to<IPv4Header>();

	ip_packet.version_length = IPv4_VERSION_LENGTH;
	ip_packet.dscp_ecn = 0;
	ip_packet.total_length = network_word16(ip_raw_packet.size());
	ip_packet.id = 1;
	ip_packet.flags_fragment_offset = 0x0040;
	ip_packet.time_to_live = 64;
	ip_packet.protocol = static_cast<u8>(protocol);
	ip_packet.header_checksum = 0;

	m_device_ip_address.copy(ip_packet.src_ip);
	destination.copy(ip_packet.dst_ip);

	ip_packet.header_checksum = checksum(BufferView{ip_raw_packet, 0, sizeof(IPv4Header)});

	auto destination_mac = destination_mac_lookup(destination);

	m_network.network_adapter().send_frame(ProtocolType::IPv4, destination_mac, ip_raw_packet);
}

void IPv4::handle(const BufferView& data)
{
	auto& ip_packet = data.const_convert_to<IPv4Header>();
	size_t packet_size = network_word16(ip_packet.total_length);

	if (!is_packet_ok(ip_packet)) {
		return;
	}

	size_t header_len = header_length(ip_packet.version_length);
	BufferView extracted_data{data, header_len, packet_size - header_len};

	switch (static_cast<IPv4Protocols>(ip_packet.protocol)) {
		case IPv4Protocols::ICMP: {
			m_network.icmp_provider().handle_icmp_reply(IPv4Address{ip_packet.src_ip}, extracted_data);
			break;
		}

		case IPv4Protocols::TCP: {
			m_network.tcp_provider().handle(IPv4Address{ip_packet.src_ip}, extracted_data);
			break;
		}

		case IPv4Protocols::UDP: {
			m_network.udp_provider().handle(IPv4Address{ip_packet.src_ip}, extracted_data);
			break;
		}

		default:
			warn() << "Unknown IPv4 packet.";
			break;
	}
}

MACAddress IPv4::destination_mac_lookup(IPv4Address address)
{
	if (address == IPv4Address::Broadcast) {
		return MACAddress::Broadcast;
	} else if (address == m_device_ip_address) {
		return m_network.device_mac();
	} else if (is_in_local_subnet(address)) {
		return m_network.arp_provider().mac_address_lookup(address);
	} else {
		return m_network.arp_provider().mac_address_lookup(m_gateway_ip_address);
	}
}

bool IPv4::is_in_local_subnet(IPv4Address address)
{
	return address.mask(m_subnet_mask) == m_device_ip_address.mask(m_subnet_mask);
}

bool IPv4::is_packet_ok(const IPv4Header& packet)
{
	if (checksum(BufferView{&packet, sizeof(IPv4Header)}) != 0) {
		return false;
	}

	return true;
}

IPv4Address IPv4::ip() const
{
	return m_device_ip_address;
}
