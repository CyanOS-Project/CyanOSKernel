#pragma once
#include "Network/LinkLayer/NetworkAdapter.h"
#include "Network/NetworkLayer/ARP.h"
#include "Network/TransportLayer/DHCP.h"
#include <Buffer.h>
#include <BufferView.h>
#include <IPv4Address.h>
#include <Types.h>

enum class IPv4Protocols
{
	ICMP = 0x1,
	TCP = 0x6,
	UDP = 0x11,
};

class Network;
class IPv4
{
  public:
	IPv4(Network& network);
	void start();
	void send(IPv4Address destination, IPv4Protocols protocol, const BufferView& data);
	void handle(const BufferView& data);

	IPv4Address IP();

  private:
	struct IPv4Header {
		u8 version_length;
		u8 dscp_ecn;
		u16 total_length;
		u16 id;
		u16 flags_fragment_offset;
		u8 time_to_live;
		u8 protocol;
		u16 header_checksum;
		u8 src_ip[4];
		u8 dst_ip[4];
	} __attribute__((packed));

	static constexpr u8 IPv4_VERSION_LENGTH = (0x40 | (sizeof(IPv4Header) / sizeof(u32)));
	static constexpr u8 IPv4_FLAGS_DONT_FRAGMENT = (1 << 1);
	static constexpr u8 IPv4_FLAGS_MORE_FRAGMENTS = (1 << 2);

	MACAddress destination_mac_lookup(IPv4Address address);
	bool is_in_local_subnet(IPv4Address address);
	bool is_packet_ok(const IPv4Header& packet);

	constexpr inline size_t header_length(u8 value) { return (value & 0xF) * sizeof(u32); }

	Network& m_network;
	DHCP m_dhcp;
	IPv4Address m_device_ip_address{};
	IPv4Address m_gateway_ip_address{};
	IPv4Address m_subnet_mask{};
};
