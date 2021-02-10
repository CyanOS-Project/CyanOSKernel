#include "ARP.h"
#include "Devices/DebugPort/Logger.h"
#include "Endianess.h"
#include "IPv4Address.h"

Vector<MACAddress> ARP::arp_table{};

const MACAddress& ARP::mac_address_lookup(const IPv4Address& lookup_ip) {}

void ARP::send_arp_request(const IPv4Address& lookup_ip)
{
	auto& adapter = *NetworkAdapter::default_network_adapter;

	ARPHeader arp{.hardware_type = to_big_endian(static_cast<u16>(HardwareType::Ethernet)),
	              .protocol_type = to_big_endian(static_cast<u16>(ProtocolType::IPv4)),
	              .hardware_addr_len = MAC_ADDRESS_LENGTH,
	              .protocol_addr_len = IP4_ADDRESS_LENGTH,
	              .opcode = to_big_endian(static_cast<u16>(ARPCode::Request)),
	              .source_hw_addr = {},
	              .source_protocol_addr = {10, 0, 2, 15}, // static address for us until we get one from DHCP.
	              .destination_hw_addr = {},
	              .destination_protocol_addr = {}};

	adapter.MAC().copy(arp.source_hw_addr);

	MACAddress::Zero.copy(arp.destination_hw_addr);
	lookup_ip.copy(arp.destination_protocol_addr);

	adapter.send_frame(ProtocolType::ARP, MACAddress::Broadcast, BufferView{&arp, sizeof(ARPHeader)});

	info() << "ARP Request: Who owns " << IPv4Address{arp.destination_protocol_addr} << " ? Tell "
	       << IPv4Address{arp.source_protocol_addr};
}

void ARP::handle_arp_packet(const BufferView& data)
{
	if (data.size() < sizeof(ARPHeader)) {
		warn() << "Insufficient ARP packet size!";
	}

	const ARPHeader& arp = data.const_convert_to<ARPHeader>();

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