#pragma once
#include "Tasking/SpinLock.h"
#include "Tasking/WaitQueue.h"
#include <Buffer.h>
#include <Types.h>
#include <Vector.h>

class Network;
class ARP
{

  public:
	ARP(Network& network);
	MACAddress mac_address_lookup(IPv4Address lookup_ip);
	void handle(const BufferView& data);

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

	struct ARPEntry {
		IPv4Address ip;
		MACAddress mac;
		WaitQueue wait_queue;
		ARPEntry(IPv4Address _ip) : ip{_ip}, mac{}, wait_queue{} {}
		ARPEntry(IPv4Address _ip, MACAddress _mac) : ip{_ip}, mac{_mac}, wait_queue{} {}
		ARPEntry(ARPEntry&& other) : ip{other.ip}, mac{other.mac}, wait_queue{move(other.wait_queue)} {}
		ARPEntry& operator=(ARPEntry&& other)
		{
			ip = other.ip;
			mac = other.mac;
			wait_queue = move(other.wait_queue);
			return *this;
		}
	};

	Network& m_network;
	Spinlock m_lock;
	Vector<ARPEntry> m_arp_table;

	void send_arp_request(IPv4Address lookup_ip);
	void answer_arp_request(IPv4Address dest_ip, MACAddress dest_mac);
	void add_arp_entry(IPv4Address ip, MACAddress mac);
};
