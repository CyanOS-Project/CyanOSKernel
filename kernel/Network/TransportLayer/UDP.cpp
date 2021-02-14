#include "UDP.h"
#include "Network/Network.h"
#include "Network/NetworkLayer/IPv4.h"
#include "Tasking/ScopedLock.h"
#include <Algorithms.h>
#include <Buffer.h>
#include <Endianess.h>

// TODO: refactor network structures so one function will handle convertions to/from big endian.
UDP::UDP(Network& network) : m_network{network} {}

void UDP::send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data)
{
	ScopedLock local_lock{m_lock};

	u16 source_port = m_available_ports_bitmap.find_first_clear();
	m_available_ports_bitmap.set(source_port);
	auto& connection = m_connections_list.emplace_back(source_port);

	send_segment(dest_ip, dest_port, source_port, data);

	connection.wait_queue.wait(local_lock);

	m_available_ports_bitmap.clear(source_port);
	m_connections_list.remove_if([&connection](const Connection& i) { return connection.port == i.port; });
}

void UDP::send_with_special_port(const IPv4Address& dest_ip, u16 dest_port, u16 src_port, const BufferView& data)
{
	ScopedLock local_lock{m_lock};

	ASSERT(m_available_ports_bitmap.check_clear(src_port));

	m_available_ports_bitmap.set(src_port);
	auto& connection = m_connections_list.emplace_back(src_port);

	send_segment(dest_ip, dest_port, src_port, data);

	connection.wait_queue.wait(local_lock);

	m_available_ports_bitmap.clear(src_port);
	m_connections_list.remove_if([&connection](const Connection& i) { return connection.port == i.port; });
}

void UDP::handle(const IPv4Address& src_ip, const BufferView& data)
{
	ScopedLock local_lock{m_lock};

	warn() << "UDP Packet received.";

	auto& udp_segment = data.const_convert_to<UDPHeader>();

	u16 dest_port = to_big_endian<u16>(udp_segment.destination_port);
	auto connection = m_connections_list.find_if([dest_port](const Connection& connection) {
		return connection.port == dest_port;
		info() << connection.port;
	});

	if (connection != m_connections_list.end()) {
		connection->wait_queue.wake_up_all();
	} else {
		err() << "Received UDP packet with no known source port.";
	}
}

void UDP::send_segment(const IPv4Address& dest_ip, u16 dest_port, u16 src_port, const BufferView& data)
{
	Buffer udp_raw_segment{sizeof(UDPHeader) + data.size()};
	auto& udp_segment = udp_raw_segment.convert_to<UDPHeader>();

	udp_segment.source_port = to_big_endian<u16>(src_port);
	udp_segment.destination_port = to_big_endian<u16>(dest_port);
	udp_segment.total_length = to_big_endian<u16>(sizeof(UDPHeader) + data.size());
	udp_segment.checksum = 0;

	udp_raw_segment.fill_from(data.ptr(), sizeof(UDPHeader), data.size());

	m_network.ipv4_provider().send_ip_packet(dest_ip, IPv4Protocols::UDP, udp_raw_segment);
}

u16 UDP::calculate_checksum(const BufferView& data)
{
	auto* u16_array = reinterpret_cast<const u16*>(data.ptr());
	size_t u16_array_size = number_of_words<u16>(data.size());

	u32 sum = 0;
	for (size_t i = 0; i < u16_array_size; i++) {
		sum += to_big_endian<u16>(u16_array[i]);
	}

	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian<u16>(~u16(sum));
}