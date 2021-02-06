#include "ARP.h"
#include "Endianess.h"

void ARP::send(NetworkAdapter& network)
{
	ARPHeader arp{.hardware_type = to_big_endian(static_cast<uint16_t>(HardwareType::Ethernet)),
	              .protocol_type = to_big_endian(static_cast<uint16_t>(ProtocolType::IPv4)),
	              .hardware_addr_len = MAC_ADDRESS_LENGTH,
	              .protocol_addr_len = IP4_ADDRESS_LENGTH,
	              .opcode = to_big_endian(static_cast<uint16_t>(ARPCode::ARPRequest)),
	              .source_protocol_addr = {10, 2, 2, 1},
	              .destination_protocol_addr = {}};

	network.MAC().copy(arp.source_hw_addr);
	MACAddress::Broadcast.copy(arp.destination_hw_addr);

	network.send_frame(ProtocolType::ARP, MACAddress::Broadcast, &arp, sizeof(ARPHeader));
}
