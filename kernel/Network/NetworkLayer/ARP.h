#pragma once
#include "Network/LinkLayer/NetworkAdapter.h"
#include <Types.h>

class ARP
{

  public:
	ARP() = default;
	void test_send(NetworkAdapter& network);
	void handle_arp_packet(const void* data, size_t size);

  private:
	enum class HardwareType
	{
		Ethernet = 0x1,
		IEEE802 = 0x6,
		ARCNET = 0x7
	};
	enum class ARPCode
	{
		Request = 1,
		Reply = 2
	};
	static constexpr u8 MAC_ADDRESS_LENGTH = 0x6;
	static constexpr u8 IP4_ADDRESS_LENGTH = 0x4;

	struct ARPHeader {
		u16 hardware_type;
		u16 protocol_type;
		u8 hardware_addr_len;
		u8 protocol_addr_len;
		u16 opcode;
		u8 source_hw_addr[MAC_ADDRESS_LENGTH];
		u8 source_protocol_addr[IP4_ADDRESS_LENGTH];
		u8 destination_hw_addr[MAC_ADDRESS_LENGTH];
		u8 destination_protocol_addr[IP4_ADDRESS_LENGTH];
	} __attribute__((packed));
};
