#include "ARP.h"
#include "Devices/DebugPort/Logger.h"
#include "IPv4.h"
#include "IPv4Address.h"
#include "Network/Network.h"
#include "NetworkAlgorithms.h"

ARP::ARP(Network& network) : m_network{network} {}

MACAddress ARP::mac_address_lookup(IPv4Address lookup_ip)
{
	ScopedLock local_lock{m_lock};

	auto dest_mac = m_arp_table.find_if([&lookup_ip](const ARPEntry& arp_entry) { return arp_entry.ip == lookup_ip; });

	if (dest_mac != m_arp_table.end()) {
		return dest_mac->mac;
	} else {
		send_arp_request(lookup_ip);

		auto& new_arp_entry = m_arp_table.emplace_front(lookup_ip);
		new_arp_entry.wait_queue.wait(local_lock);

		return new_arp_entry.mac;
	}
}

void ARP::send_arp_request(IPv4Address lookup_ip)
{

	ARPHeader arp{.hardware_type = network_word16(static_cast<u16>(HardwareType::Ethernet)),
	              .protocol_type = network_word16(static_cast<u16>(ProtocolType::IPv4)),
	              .hardware_addr_len = MAC_ADDRESS_LENGTH,
	              .protocol_addr_len = IP4_ADDRESS_LENGTH,
	              .opcode = network_word16(static_cast<u16>(ARPCode::Request)),
	              .source_hw_addr = {},
	              .source_protocol_addr = {},
	              .destination_hw_addr = {},
	              .destination_protocol_addr = {}};

	m_network.device_ip().copy(arp.source_protocol_addr);
	m_network.device_mac().copy(arp.source_hw_addr);

	MACAddress::Zero.copy(arp.destination_hw_addr);
	lookup_ip.copy(arp.destination_protocol_addr);

	m_network.network_adapter().send_frame(ProtocolType::ARP, MACAddress::Broadcast,
	                                       BufferView{&arp, sizeof(ARPHeader)});

	info() << "ARP Request: Who owns " << IPv4Address{arp.destination_protocol_addr} << " ? Tell "
	       << IPv4Address{arp.source_protocol_addr};
}

void ARP::handle(const BufferView& data)
{
	if (data.size() < sizeof(ARPHeader)) {
		warn() << "Insufficient ARP packet size!";
	}

	const ARPHeader& arp = data.const_convert_to<ARPHeader>();

	switch (static_cast<ARPCode>(network_word16(arp.opcode))) {
		case ARPCode::Request: {
			info() << "ARP Request: Who owns " << IPv4Address{arp.destination_protocol_addr} << " ? Tell "
			       << IPv4Address{arp.source_protocol_addr} << ".";

			if (IPv4Address{arp.destination_protocol_addr} == m_network.device_ip()) {
				answer_arp_request(IPv4Address{arp.source_protocol_addr}, MACAddress{arp.source_hw_addr});
			}

			break;
		}
		case ARPCode::Reply: {
			info() << "ARP Reply: Device " << MACAddress{arp.source_hw_addr} << " Owns "
			       << IPv4Address{arp.source_protocol_addr} << ".";

			add_arp_entry(IPv4Address{arp.source_protocol_addr}, MACAddress{arp.source_hw_addr});

			break;
		}
		default:
			warn() << "Unkown ARP code " << network_word16(arp.opcode) << "!";
			break;
	}
}

void ARP::add_arp_entry(IPv4Address ip, MACAddress mac)
{
	ScopedLock local_lock{m_lock};

	auto arp_entry = m_arp_table.find_if([&ip](const ARPEntry& arp_entry) { return arp_entry.ip == ip; });

	if (arp_entry == m_arp_table.end()) {
		// We got a MAC we didn't ask for, add it to the table anyway.
		m_arp_table.emplace_back(ip, mac);
	} else {
		// Update the MAC address to the entry and wake up the waiting thread.
		arp_entry->mac = mac;
		arp_entry->wait_queue.wake_up_all();
	}
}

void ARP::answer_arp_request(IPv4Address dest_ip, MACAddress dest_mac)
{
	ARPHeader arp{.hardware_type = network_word16(static_cast<u16>(HardwareType::Ethernet)),
	              .protocol_type = network_word16(static_cast<u16>(ProtocolType::IPv4)),
	              .hardware_addr_len = MAC_ADDRESS_LENGTH,
	              .protocol_addr_len = IP4_ADDRESS_LENGTH,
	              .opcode = network_word16(static_cast<u16>(ARPCode::Reply)),
	              .source_hw_addr = {},
	              .source_protocol_addr = {},
	              .destination_hw_addr = {},
	              .destination_protocol_addr = {}};

	m_network.device_mac().copy(arp.source_hw_addr);
	m_network.device_ip().copy(arp.source_protocol_addr);

	dest_mac.copy(arp.destination_hw_addr);
	dest_ip.copy(arp.destination_protocol_addr);

	m_network.network_adapter().send_frame(ProtocolType::ARP, dest_mac, BufferView{&arp, sizeof(ARPHeader)});

	info() << "I'm " << m_network.device_mac() << " and I own " << m_network.device_ip() << ", Telling " << dest_ip
	       << ".";
}