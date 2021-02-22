#include "TCP.h"
#include "Devices/DebugPort/Logger.h"
#include "Network/Network.h"
#include <Endianess.h>
#include <ErrorCodes.h>
#include <NetworkAlgorithms.h>

TCP::TCP(Network& network) : m_network{network} {}

void TCP::handle(IPv4Address src_ip, const BufferView& data)
{
	auto session = m_connection_sessions.find_if(
	    [&data, &src_ip](TCPSession& session) { return session.is_packet_for_me(src_ip, data); });
	if (session != m_connection_sessions.end()) {
		session->handle(src_ip, data);
	} else {
		warn() << "TCP packet received, with no known session, packet dropped.";
	}
}
TCPSession& TCP::accept(u16 port)
{
	auto& server = m_connection_sessions.emplace_back(m_network, TCPSession::Type::Server);
	server.accept(port);
	return server;
}

TCPSession& TCP::connect(IPv4Address ip, u16 port)
{
	auto& client = m_connection_sessions.emplace_back(m_network, TCPSession::Type::Client);
	client.connect(ip, port);
	return client;
}

void TCP::close(TCPSession&) {}

TCPSession::TCPSession(Network& network, Type type) :
    m_network{&network},
    m_type{type},
    m_state{State::Idle},
    m_syn_semaphore{0},
    m_ack_semaphore{0},
    m_data_semaphore{0}
{
}

Result<void> TCPSession::accept(u16 port)
{
	m_local_port = port;
	m_local_sequence = 0;
	m_state = State::Listen;

	wait_for_syn();

	if (m_state == State::Established) {
		return {};
	} else {
		warn() << "Connection Denied!";
		return ResultError{ERROR_CONNECTION_DENIED};
	}
}

Result<void> TCPSession::connect(IPv4Address ip, u16 port)
{
	m_remote_ip = ip;
	m_local_port = 5000;
	m_remote_port = port;
	m_local_sequence = 0;
	m_state = State::SYN_Sent;

	send_syn();

	if (m_state == State::Established) {
		return {};
	} else {
		warn() << "Connection Denied!";
		return ResultError{ERROR_CONNECTION_DENIED};
	}
}

void TCPSession::close() {}

Result<void> TCPSession::send(const BufferView& data)
{
	send_packet(data, 0);
	wait_for_ack();
}

Result<void> TCPSession::receive(BufferView& data)
{
	wait_for_packet();
	send_ack();
}

void TCPSession::handle(IPv4Address src_ip, const BufferView& data)
{
	if (!is_packet_ok(data)) {
		// FIXME: handle error here.
		return;
	}

	auto& tcp_header = data.const_convert_to<TCPHeader>();

	if (data.size() > (tcp_header.data_offset * sizeof(u32))) {
		handle_data(src_ip, data);
	}

	if (tcp_header.flags & FLAGS::SYN) {
		handle_syn(src_ip, data);
	}

	if (tcp_header.flags & FLAGS::RST) {
		handle_fin(src_ip, data);
	}

	if (tcp_header.flags & FLAGS::FIN) {
		handle_fin(src_ip, data);
	}

	if (tcp_header.flags & FLAGS::ACK) {
		handle_ack(src_ip, data);
	}
}

void TCPSession::handle_ack(IPv4Address src_ip, const BufferView& data)
{

	switch (m_state) {
		case State::SYN_Sent: {
			m_state = State::Established;
			m_ack_semaphore.release();
			break;
		}
		default:
			m_ack_semaphore.release();
	}
}

void TCPSession::handle_syn(IPv4Address src_ip, const BufferView& data)
{
	if (m_state == State::Listen) {
		m_state = State::SYN_Received;
		send_syn();
		wait_for_ack();
	} else if (m_state == State::SYN_Sent) {
		send_ack();
	} else {
		warn() << "Shouldn't happen!";
	}
}

void TCPSession::handle_rst(IPv4Address src_ip, const BufferView& data)
{
	end_connection();
}

void TCPSession::handle_fin(IPv4Address src_ip, const BufferView& data) {}

void TCPSession::handle_data(IPv4Address src_ip, const BufferView& data) {}

void TCPSession::end_connection()
{
	m_state = State::Closed;
	m_syn_semaphore.release();
	m_ack_semaphore.release();
	m_data_semaphore.release();
}

void TCPSession::send_syn()
{
	send_control_packet(FLAGS::SYN);
}

void TCPSession::wait_for_syn()
{
	m_syn_semaphore.acquire();
}

void TCPSession::send_ack()
{
	send_control_packet(FLAGS::ACK);
}

void TCPSession::wait_for_ack()
{
	m_ack_semaphore.acquire();
}

void TCPSession::send_control_packet(u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_remote_port);
	tcp_header.dest_port = network_word16(m_local_port);
	tcp_header.seq = network_word32(m_local_sequence);
	tcp_header.ack = network_word32(m_dest_sequence);
	tcp_header.data_offset = data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	// Add check sum of pseudo ip header.
	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send_ip_packet(m_remote_ip, IPv4Protocols::TCP,
	                                          BufferView{&tcp_header, sizeof(TCPHeader)});
}

void TCPSession::send_packet(const BufferView& data, u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_remote_port);
	tcp_header.dest_port = network_word16(m_local_port);
	tcp_header.seq = network_word32(m_local_sequence);
	tcp_header.ack = 0;
	tcp_header.data_offset = data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	// Add check sum of pseudo ip header.
	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send_ip_packet(m_remote_ip, IPv4Protocols::TCP,
	                                          BufferView{&tcp_header, sizeof(TCPHeader)});
}

Buffer TCPSession::wait_for_packet() {}

bool TCPSession::is_packet_ok(const BufferView& data)
{
	return true;
}

u16 TCPSession::tcp_checksum(const BufferView& data)
{
	PseudoIPHeader pseudo_ip;
	m_network->device_ip().copy(pseudo_ip.src_ip);
	m_remote_ip.copy(pseudo_ip.dest_ip);
	pseudo_ip.protocol = static_cast<u8>(IPv4Protocols::TCP);
	pseudo_ip.tcp_length = to_big_endian<u16>(data.size());
	pseudo_ip.zeros = 0;

	u32 sum = 0;

	auto* tcp_array = reinterpret_cast<const u16*>(data.ptr());
	size_t tcp_array_size = number_of_words<u16>(data.size());
	for (size_t i = 0; i < tcp_array_size; i++) {
		sum += to_big_endian<u16>(tcp_array[i]);
	}

	auto* pseudo_ipv4_array = reinterpret_cast<const u16*>(&pseudo_ip);
	size_t pseudo_ipv4_array_size = number_of_words<u16>(sizeof(PseudoIPHeader));
	for (size_t i = 0; i < pseudo_ipv4_array_size; i++) {
		sum += to_big_endian<u16>(pseudo_ipv4_array[i]);
	}

	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian<u16>(~u16(sum));
}

bool TCPSession::is_packet_for_me(IPv4Address ip, const BufferView& data)
{
	auto& tcp_header = data.const_convert_to<TCPHeader>();
	if (m_state == State::Listen) {
		if (to_big_endian<u16>(tcp_header.dest_port) == m_local_port) {
			return true;
		} else {
			return false;
		}
	} else {
		if ((to_big_endian<u16>(tcp_header.dest_port) == m_local_port) &&
		    (to_big_endian<u16>(tcp_header.src_port) == m_remote_port) && (ip == m_remote_ip)) {
			return true;
		} else {
			return false;
		}
	}
}