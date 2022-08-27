#include "udp.h"
#include "devices/debug_port/logger.h"
#include "network/network.h"
#include "network/network_layer/ipv4.h"
#include "tasking/scoped_lock.h"
#include <algorithms.h>
#include <buffer.h>
#include <error_codes.h>
#include <network_algorithms.h>

// TODO: refactor network structures so one function will handle convertions to/from big endian.
UDP::UDP(Network& network) : m_network{network} {}

Result<size_t> UDP::send(SocketAddress dest, BufferView data)
{
	u16 src_port = m_ports.find_first_clear();

	if (auto result = send_segment(dest, src_port, data))
		return ResultError{result.error()};
	m_ports.clear(src_port);

	return data.size();
}

Result<size_t> UDP::send(SocketAddress dest, u16 src_port, BufferView data)
{
	if (m_ports.check_set(src_port)) {
		return ResultError{ERROR_PORT_ALREADY_IN_USE};
	}

	if (auto result = send_segment(dest, src_port, data))
		return ResultError{result.error()};

	m_ports.clear(src_port);

	return data.size();
}

Result<size_t> UDP::receive(u16 dest_port, BufferMutableView buffer)
{
	auto result = receive_segment(dest_port, buffer);
	if (result)
		return ResultError{result.error()};

	return result.value().data_size;
}

Result<size_t> UDP::receive(u16 dest_port, BufferMutableView buffer, SocketAddress& source_address)
{
	auto result = receive_segment(dest_port, buffer);
	if (result)
		return ResultError{result.error()};

	source_address.ip = result.value().src_ip;
	source_address.port = result.value().src_port;

	return result.value().data_size;
}

void UDP::handle(IPv4Address src_ip, BufferView data)
{
	ScopedLock local_lock{m_lock};

	auto& udp_segment = data.const_convert_to<UDPHeader>();

	u16 dest_port = network_word16(udp_segment.destination_port);
	auto connection = m_connections_list.find_if(
	    [dest_port](const Connection& connection) { return connection.dest_port == dest_port; });

	if (connection != m_connections_list.end()) {
		connection->src_port = network_word16(udp_segment.source_port);
		connection->src_ip = src_ip;
		connection->data_size = data.size() - sizeof(UDPHeader);
		// FIXME: check if it fits first;
		connection->buffer->fill_from(data.ptr() + sizeof(UDPHeader), 0, data.size() - sizeof(UDPHeader));

		connection->wait_queue.wake_up_all();
	} else {
		err() << "Received UDP packet with no known source port.";
	}
}

Result<void> UDP::send_segment(SocketAddress dest, u16 src_port, BufferView data)
{
	Buffer udp_raw_segment{sizeof(UDPHeader) + data.size()};
	auto& udp_segment = udp_raw_segment.convert_to<UDPHeader>();

	udp_segment.source_port = network_word16(src_port);
	udp_segment.destination_port = network_word16(dest.port);
	udp_segment.total_length = network_word16(sizeof(UDPHeader) + data.size());
	udp_segment.checksum = 0;

	udp_raw_segment.fill_from(data.ptr(), sizeof(UDPHeader), data.size());

	m_network.ipv4_provider().send(dest.ip, IPv4Protocols::UDP, udp_raw_segment);

	return {};
}

Result<UDP::DatagramInfo> UDP::receive_segment(u16 dest_port, BufferMutableView buffer)
{
	ScopedLock local_lock{m_lock};

	ASSERT(!m_connections_list.contains([dest_port](const Connection& i) { return dest_port == i.dest_port; }));
	// FIXME: handle overflow.
	auto& connection = *m_connections_list.emplace_back(dest_port, buffer);

	connection.wait_queue.wait(local_lock);

	DatagramInfo info;
	info.src_port = connection.src_port;
	info.src_ip = connection.src_ip;
	info.data_size = connection.data_size;

	m_connections_list.remove_if([&connection](const Connection& i) { return connection.dest_port == i.dest_port; });

	return info;
}
