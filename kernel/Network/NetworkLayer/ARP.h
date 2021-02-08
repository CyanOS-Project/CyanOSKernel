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
	static constexpr uint8_t MAC_ADDRESS_LENGTH = 0x6;
	static constexpr uint8_t IP4_ADDRESS_LENGTH = 0x4;

	struct ARPHeader {
		uint16_t hardware_type;
		uint16_t protocol_type;
		uint8_t hardware_addr_len;
		uint8_t protocol_addr_len;
		uint16_t opcode;
		uint8_t source_hw_addr[MAC_ADDRESS_LENGTH];
		uint8_t source_protocol_addr[IP4_ADDRESS_LENGTH];
		uint8_t destination_hw_addr[MAC_ADDRESS_LENGTH];
		uint8_t destination_protocol_addr[IP4_ADDRESS_LENGTH];
	} __attribute__((packed));
};
