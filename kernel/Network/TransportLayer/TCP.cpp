#include "TCP.h"
#include "Devices/DebugPort/Logger.h"
#include "Network/Network.h"
#include <Endianess.h>
#include <NetworkAlgorithms.h>

TCP::TCP(Network& network) : m_network{network} {}

void TCP::handle(const IPv4Address& src_ip, const BufferView& data)
{
	UNUSED(src_ip);
	UNUSED(data);

	warn() << "TCP packet received, No handler for that yet.";
}
TCPSession& TCP::accept(u16 port)
{
	auto& server = m_connection_sessions.emplace_back(m_network);
	server.accept(port);
	return server;
}

TCPSession& TCP::connect(IPv4Address ip, u16 port)
{
	auto& client = m_connection_sessions.emplace_back(m_network);
	client.connect(ip, port);
	return client;
}

void TCP::close(TCPSession&) {}

TCPSession::TCPSession(Network& network) :
    m_network{&network},
    m_syn_semaphore{0},
    m_ack_semaphore{0},
    m_data_semaphore{0}
{
}

void TCPSession::accept(u16 port)
{
	m_src_port = 5000;
	m_src_sequence = 0;

	wait_for_syn();
	send_ack();
	send_syn();
	wait_for_ack();
}

void TCPSession::connect(IPv4Address ip, u16 port)
{
	m_dest_ip = ip;
	m_dest_port = port;
	m_src_port = 5000;
	m_src_sequence = 0;

	send_syn();
	wait_for_ack();
	wait_for_syn();
	send_ack();
}

void TCPSession::close() {}

void TCPSession::send(const BufferView& data)
{
	send_packet(data, 0);
	wait_for_ack();
}

void TCPSession::receive(BufferView& data)
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

	if (tcp_header.flags & FLAGS::ACK) {
		m_ack_semaphore.release();
	}

	if (tcp_header.flags & FLAGS::SYN) {
		m_syn_semaphore.release();
	}

	if (tcp_header.flags & FLAGS::FIN) {
		handle_fin();
	}

	if (data.size() > (tcp_header.data_offset * sizeof(u32))) {
		m_data_semaphore.release();
	}
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

void TCPSession::handle_fin()
{
	send_ack();
	send_control_packet(FLAGS::FIN);
	wait_for_ack();
}

void TCPSession::send_control_packet(u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_src_port);
	tcp_header.dest_port = network_word16(m_dest_port);
	tcp_header.seq = network_word32(m_src_sequence);
	tcp_header.ack = network_word32(m_dest_sequence);
	tcp_header.data_offset = data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	// Add check sum of pseudo ip header.
	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send_ip_packet(m_dest_ip, IPv4Protocols::TCP,
	                                          BufferView{&tcp_header, sizeof(TCPHeader)});
}

void TCPSession::send_packet(const BufferView& data, u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_src_port);
	tcp_header.dest_port = network_word16(m_dest_port);
	tcp_header.seq = network_word32(m_src_sequence);
	tcp_header.ack = 0;
	tcp_header.data_offset = data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	// Add check sum of pseudo ip header.
	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send_ip_packet(m_dest_ip, IPv4Protocols::TCP,
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
	m_dest_ip.copy(pseudo_ip.dest_ip);
	pseudo_ip.protocol = static_cast<u8>(IPv4Protocols::TCP);
	pseudo_ip.tcp_length = to_big_endian<u16>(data.size());
	pseudo_ip.zeros = 0;

	u32 sum = 0;

	u32 sum1 = 0;
	auto* tcp_array = reinterpret_cast<const u16*>(data.ptr());
	size_t tcp_array_size = number_of_words<u16>(data.size());
	for (size_t i = 0; i < tcp_array_size; i++) {
		info() << Hex16(to_big_endian<u16>(tcp_array[i]));
		sum1 += to_big_endian<u16>(tcp_array[i]);
	}

	info() << "...................";

	u32 sum2 = 0;
	auto* pseudo_ipv4_array = reinterpret_cast<const u16*>(&pseudo_ip);
	size_t pseudo_ipv4_array_size = number_of_words<u16>(sizeof(PseudoIPHeader));
	for (size_t i = 0; i < pseudo_ipv4_array_size; i++) {
		info() << Hex16(to_big_endian<u16>(pseudo_ipv4_array[i]));
		sum2 += to_big_endian<u16>(pseudo_ipv4_array[i]);
	}

	sum = sum1 + sum2;
	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian<u16>(~u16(sum));
}