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
    m_lock{UniquePointer<Spinlock>::make_unique()},
    m_network{&network},
    m_type{type}
{
}

Result<void> TCPSession::accept(u16 port)
{
	ScopedLock local_lock{*m_lock};

	m_local_port = port;
	m_state = State::LISTEN;

	if (auto error = wait_for_syn(local_lock))
		return error;

	m_state = State::SYN_RECEIVED;

	send_ack_syn();

	if (auto error = wait_for_ack(local_lock))
		return error;

	m_state = State::ESTABLISHED;

	return {};
}

Result<void> TCPSession::connect(IPv4Address ip, u16 port)
{
	ScopedLock local_lock{*m_lock};

	m_remote_ip = ip;
	m_remote_port = port;
	m_local_port = 5000;

	send_syn();
	m_state = State::SYN_SENT;

	if (auto error = wait_for_syn(local_lock))
		return error;

	send_ack();

	m_state = State::ESTABLISHED;

	return {};
}

void TCPSession::close()
{
	ScopedLock local_lock{*m_lock};

	m_state = State::FIN_WAIT1;
	send_fin();
	wait_for_ack(local_lock);
	m_state = State::FIN_WAIT2;
}

Result<void> TCPSession::send(const BufferView& data)
{
	ScopedLock local_lock{*m_lock};

	if (m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	send_packet(data, 0);
	wait_for_ack(local_lock);

	return {};
}

Result<void> TCPSession::receive(Buffer& data)
{
	ScopedLock local_lock{*m_lock};

	if (m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	m_initial_remote_sequence = m_remote_sequence;
	m_buffer = &data;

	wait_for_packet(local_lock);

	m_buffer = nullptr;

	if (m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}
	return {};
}

void TCPSession::handle(IPv4Address src_ip, const BufferView& data)
{
	ScopedLock local_lock{*m_lock};

	auto& tcp_header = data.const_convert_to<TCPHeader>();

	if (m_state == State::CLOSED) {
		return;
	}

	if (!is_packet_ok(data)) {
		// FIXME: handle error here.
		return;
	}
	if (!handle_out_of_order_packets(local_lock, network_word32(tcp_header.seq))) {
		return;
	}

	if (data.size() > from_data_offset(tcp_header.data_offset)) {
		handle_data(data);
	}

	if (tcp_header.flags & FLAGS::SYN) {
		handle_syn(src_ip, data);
	}

	if (tcp_header.flags & FLAGS::PSH) {
		handle_psh();
	}

	if (tcp_header.flags & FLAGS::RST) {
		handle_rst();
	}

	if (tcp_header.flags & FLAGS::FIN) {
		handle_fin(local_lock);
	}

	if (tcp_header.flags & FLAGS::ACK) {
		handle_ack();
	}

	m_data_waitqueue.wake_up_all();
}

void TCPSession::handle_syn(IPv4Address src_ip, const BufferView& data)
{
	auto& tcp_header = data.const_convert_to<TCPHeader>();

	if (m_state == State::LISTEN | m_state == State::SYN_SENT) {

		m_remote_sequence = network_word32(tcp_header.seq) + 1;
		m_remote_port = network_word16(tcp_header.src_port);
		m_remote_ip = IPv4Address{src_ip};
		m_syn_waitqueue.wake_up_all();
	} else {
		err() << "Shouldn't happen!";
	}
}

void TCPSession::handle_ack()
{
	m_ack_waitqueue.wake_up_all();
}

void TCPSession::handle_rst()
{
	end_connection();
}

void TCPSession::handle_fin(ScopedLock<Spinlock>& lock)
{
	if (m_state == State::FIN_WAIT2) {
		m_remote_sequence++;
		send_ack();
		m_state = State::CLOSED;
	} else if (m_state == State::ESTABLISHED) {
		m_state = State::CLOSE_WAIT;
		m_remote_sequence++;
		send_ack();
		send_fin();

		m_state = State::LAST_ACK;
		wait_for_ack(lock);

		m_state = State::CLOSED;
		end_connection();
	} else {
		warn() << "This shouldn't happen, received FIN outside FIN_WAIT2 or ESTABLISHED states!";
	}
}

void TCPSession::handle_psh()
{
	m_data_push_waitqueue.wake_up_all();
}

void TCPSession::handle_data(const BufferView& data)
{
	if (is_buffer_available()) {
		auto& tcp_header = data.const_convert_to<TCPHeader>();
		size_t header_size = from_data_offset(tcp_header.data_offset);
		size_t payload_size = data.size() - header_size;
		size_t data_offset = m_remote_sequence - m_initial_remote_sequence;

		m_buffer->fill_from(data.ptr() + header_size, data_offset, payload_size);
		m_remote_sequence += payload_size;

		send_ack();
	}
}

bool TCPSession::handle_out_of_order_packets(ScopedLock<Spinlock>& lock, u32 remote_sequence)
{
	if (m_state != State::ESTABLISHED) {
		return true;
	}

	while (!is_in_order_packet(remote_sequence) && is_in_window_packet(remote_sequence)) {
		// FIXME: wait for some more out of order packets before sending ack (for retransmition)
		send_ack();

		m_data_waitqueue.wait(lock);
	}
	return is_in_window_packet(remote_sequence);
}

Result<void> TCPSession::send_syn()
{
	auto result = send_control_packet(FLAGS::SYN);
	m_local_sequence++;
	return result;
}

Result<void> TCPSession::send_ack_syn()
{
	auto result = send_control_packet(FLAGS::SYN | FLAGS::ACK);
	m_local_sequence++;
	return result;
}

Result<void> TCPSession::send_ack()
{
	return send_control_packet(FLAGS::ACK);
}

Result<void> TCPSession::send_fin()
{
	return send_control_packet(FLAGS::FIN);
}

Result<void> TCPSession::wait_for_ack(ScopedLock<Spinlock>& lock)
{
	m_ack_waitqueue.wait(lock);
	if (m_state == State::CLOSED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}
	return {};
}

Result<void> TCPSession::wait_for_syn(ScopedLock<Spinlock>& lock)
{
	m_syn_waitqueue.wait(lock);
	if (m_state == State::CLOSED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}
	return {};
}

Result<void> TCPSession::wait_for_packet(ScopedLock<Spinlock>& lock)
{
	m_data_push_waitqueue.wait(lock);
	if (m_state == State::CLOSED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}
	return {};
}

void TCPSession::end_connection()
{
	m_state = State::CLOSED;
	m_syn_waitqueue.wake_up_all();
	m_ack_waitqueue.wake_up_all();
	m_data_waitqueue.wake_up_all();
	m_data_push_waitqueue.wake_up_all();
}

Result<void> TCPSession::send_control_packet(u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_local_port);
	tcp_header.dest_port = network_word16(m_remote_port);
	tcp_header.seq = network_word32(m_local_sequence);
	tcp_header.ack = network_word32(m_remote_sequence);
	tcp_header.data_offset = to_data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send(m_remote_ip, IPv4Protocols::TCP, BufferView{&tcp_header, sizeof(TCPHeader)});
}

Result<void> TCPSession::send_packet(const BufferView& data, u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_local_port);
	tcp_header.dest_port = network_word16(m_remote_port);
	tcp_header.seq = network_word32(m_local_sequence);
	tcp_header.ack = 0;
	tcp_header.data_offset = to_data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(1000);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});

	m_network->ipv4_provider().send(m_remote_ip, IPv4Protocols::TCP, BufferView{&tcp_header, sizeof(TCPHeader)});

	m_local_sequence += data.size();
}

bool TCPSession::is_packet_ok(const BufferView& data)
{
	return true;
}

bool TCPSession::is_in_order_packet(u32 remote_sequence)
{
	return m_remote_sequence == remote_sequence;
}
bool TCPSession::is_in_window_packet(u32 remote_sequence)
{
	return (m_initial_remote_sequence <= remote_sequence) &&
	       ((m_initial_remote_sequence + MAX_WINDOW_SIZE) > remote_sequence);
}

bool TCPSession::is_buffer_available()
{
	return m_buffer;
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
	if (m_state == State::CLOSED) {
		return false;
	} else if (m_state == State::LISTEN) {
		return to_big_endian<u16>(tcp_header.dest_port) == m_local_port;
	} else {
		return (to_big_endian<u16>(tcp_header.dest_port) == m_local_port) &&
		       (to_big_endian<u16>(tcp_header.src_port) == m_remote_port) && (ip == m_remote_ip);
	}
}