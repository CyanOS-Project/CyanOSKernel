#pragma once
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
class IPv4
{
  public:
	static void initialize();
	static void send_ip_packet(IPv4Address destination, IPv4Protocols protocol, const BufferView& data);
	static IPv4Address IP();

  private:
	struct IPv4Header {
		u8 version : 4;
		u8 header_length : 4;
		u8 dscp : 6;
		u8 ecn : 2;
		u16 total_length;
		u16 id;
		u16 flags : 3;
		u16 fragment_offset : 14;
		u8 time_to_live;
		u8 protocol;
		u16 header_checksum;
		u8 src_ip[4];
		u8 dst_ip[4];
		u8 data[1];
	} __attribute__((packed));

	static constexpr u32 IPv4_HEADER_SIZE = offsetof(IPv4Header, data);
	static constexpr u8 IPv4_VERSION = 4;
	static constexpr u8 IPv4_HEADER_LENGTH = IPv4_HEADER_SIZE / sizeof(u32);
	static constexpr u8 IPv4_FLAGS_DONT_FRAGMENT = (1 << 1);
	static constexpr u8 IPv4_FLAGS_MORE_FRAGMENTS = (1 << 2);

	static IPv4Address device_ip_address;
	static IPv4Address gateway_ip_address;
	static IPv4Address subnet_mask;

	static void handle_ip_packet(const BufferView& data);
	static const MACAddress& destination_mac_lookup(IPv4Address address);
	static bool is_in_local_subnet(IPv4Address address);
	static bool is_packet_ok(const IPv4Header& packet);
	static u16 calculate_checksum(const BufferView& data);
};
