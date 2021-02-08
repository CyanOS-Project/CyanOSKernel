#include "ARP.h"
#include "Devices/DebugPort/Logger.h"
#include "Endianess.h"
#include "IPv4Address.h"

void ARP::test_send(NetworkAdapter& network)
{
	ARPHeader arp{
	    .hardware_type = to_big_endian(static_cast<uint16_t>(HardwareType::Ethernet)),
	    .protocol_type = to_big_endian(static_cast<uint16_t>(ProtocolType::IPv4)),
	    .hardware_addr_len = MAC_ADDRESS_LENGTH,
	    .protocol_addr_len = IP4_ADDRESS_LENGTH,
	    .opcode = to_big_endian(static_cast<uint16_t>(ARPCode::Request)),
	    .source_hw_addr = {},
	    .source_protocol_addr = {10, 0, 2, 15}, // static address for us until we get one from DHCP.
	    .destination_hw_addr = {},
	    .destination_protocol_addr = {10, 0, 2, 2} // IP 10.0.2.2 is Firewall/DHCP server in Qemu.
	};

	network.MAC().copy(arp.source_hw_addr);
	MACAddress::Zero.copy(arp.destination_hw_addr);

	network.send_frame(ProtocolType::ARP, MACAddress::Broadcast, &arp, sizeof(ARPHeader));

	info() << "ARP Request: Who owns " << IPv4Address{arp.destination_protocol_addr} << " ? Tell "
	       << IPv4Address{arp.source_protocol_addr};
}

void ARP::handle_arp_packet(const void* data, size_t size)
{
	if (size < sizeof(ARPHeader)) {
		warn() << "Insufficient ARP packet size!";
	}

	const ARPHeader& arp = *reinterpret_cast<const ARPHeader*>(data);

	switch (static_cast<ARPCode>(to_big_endian(arp.opcode))) {
		case ARPCode::Request:
			info() << "ARP Request: Who owns " << IPv4Address{arp.destination_protocol_addr} << " ? Tell "
			       << IPv4Address{arp.source_protocol_addr};
			break;

		case ARPCode::Reply:
			info() << "ARP Reply: Device " << MACAddress{arp.source_hw_addr} << " Owns " << arp.source_protocol_addr[0]
			       << "." << IPv4Address{arp.source_protocol_addr};
			break;
		default:
			warn() << "Unkown ARP code " << to_big_endian(arp.opcode) << "!";
			break;
	}
}