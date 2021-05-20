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
Result<TCPSession&> TCP::accept(u16 port)
{
	auto& server = *m_connection_sessions.emplace_back(m_network, m_ports, TCPSession::Type::Server);

	if (auto result = server.accept(port)) {
		// FIXME: remove from `m_connection_sessions` list.
		return ResultError{result.error()};
	}

	return server;
}

Result<TCPSession&> TCP::connect(IPv4Address ip, u16 port)
{
	auto& client = *m_connection_sessions.emplace_back(m_network, m_ports, TCPSession::Type::Client);
	;
	if (auto result = client.connect(ip, port)) {
		// FIXME: remove from `m_connection_sessions` list.
		return ResultError{result.error()};
	}

	return client;
}

TCPSession::TCPSession(Network& network, Bitmap& bitmap, Type type) :
    m_lock{UniquePointer<Spinlock>::make_unique()},
    m_network{&network},
    m_ports{bitmap},
    m_type{type}
{
}

Result<void> TCPSession::accept(u16 port)
{
	ScopedLock local_lock{*m_lock};

	m_local_port = port;
	m_state = State::LISTEN;

	if (auto error = wait_for_syn(local_lock)) {
		end_connection();
		return error;
	}

	m_state = State::SYN_RECEIVED;

	if (auto error = send_syn(local_lock)) {
		end_connection();
		return error;
	}

	m_state = State::ESTABLISHED;

	return {};
}

Result<void> TCPSession::connect(IPv4Address ip, u16 port)
{
	ScopedLock local_lock{*m_lock};

	m_remote_ip = ip;
	m_remote_port = port;
	m_local_port = m_ports->find_first_clear();

	m_state = State::SYN_SENT;

	if (auto error = send_syn(local_lock)) {
		end_connection();
		return error;
	}

	if (auto error = wait_for_syn(local_lock)) {
		end_connection();
		return error;
	}

	if (auto error = send_ack()) {
		end_connection();
		return error;
	}

	m_state = State::ESTABLISHED;

	return {};
}

void TCPSession::close()
{
	ScopedLock local_lock{*m_lock};

	m_state = State::FIN_WAIT1;
	send_fin(local_lock);

	end_connection();
}

Result<size_t> TCPSession::send(const BufferView& data)
{
	ScopedLock local_lock{*m_lock};

	if (m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	if (auto result = send_payload(local_lock, data)) {
		end_connection();
		return ResultError{result.error()};
	}

	return data.size();
}

Result<size_t> TCPSession::receive(Buffer& data)
{
	ScopedLock local_lock{*m_lock};

	if (m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	m_buffer_start_pointer = m_buffer_written_pointer;

	if (auto error = wait_for_packet(local_lock)) {
		end_connection();
		return ResultError{error.error()};
	}

	size_t data_size = m_buffer_written_pointer - m_buffer_start_pointer;
	if (data_size > data.size()) {
		return ResultError{ERROR_BUFFER_OVERFLOW};
	}

	if (data_size == 0 && m_state != State::ESTABLISHED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	data.fill_from(m_buffer.ptr() + m_buffer_start_pointer, 0, data_size);
	m_local_window_size += data_size;

	return data_size;
}

void TCPSession::handle(IPv4Address src_ip, const BufferView& data)
{
	ScopedLock local_lock{*m_lock};

	if (!is_packet_ok(data)) {
		// FIXME: handle error here.
		return;
	}

	auto& tcp_header = data.const_convert_to<TCPHeader>();
	size_t payload_offset = from_data_offset(tcp_header.data_offset);
	size_t payload_size = data.size() - payload_offset;

	m_remote_window_size = network_word16(tcp_header.window_size);

	if (m_state == State::CLOSED) {
		return;
	}

	if (!handle_out_of_order_packets(local_lock, network_word32(tcp_header.seq), payload_size)) {
		return;
	}

	if (payload_size > 0) {
		handle_data(data, payload_offset, payload_size);
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
		handle_ack(network_word32(tcp_header.ack));
	}

	m_data_waitqueue.wake_up_all();
}

SocketAddress TCPSession::address() const
{
	return SocketAddress{m_remote_ip, m_remote_port};
}

void TCPSession::handle_syn(IPv4Address src_ip, const BufferView& data)
{
	auto& tcp_header = data.const_convert_to<TCPHeader>();

	if ((m_state == State::LISTEN) || (m_state == State::SYN_SENT)) {

		m_remote_sequence = network_word32(tcp_header.seq) + 1;
		m_remote_port = network_word16(tcp_header.src_port);
		m_remote_ip = IPv4Address{src_ip};
		m_syn_waitqueue.release();
	} else {
		err() << "Shouldn't happen!";
	}
}

void TCPSession::handle_ack(u32 ack_number)
{
	m_last_ack = ack_number;
	m_ack_waitqueue.wake_up_all();
	// TODO: this will wake up all threads even the ones that didn't receive their ack, so it can be optimized by doing
	// an ordered queue of acks and their threads.
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

		m_state = State::LAST_ACK;
		send_fin(lock);

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

void TCPSession::handle_data(const BufferView& data, size_t payload_offset, size_t payload_size)
{
	if (is_buffer_available(data.size())) {

		m_buffer.fill_from(data.ptr() + payload_offset, m_buffer_written_pointer, payload_size);
		m_buffer_written_pointer += payload_size;
		m_remote_sequence += payload_size;
		m_local_window_size -= payload_size;

		send_ack();
	}
}

bool TCPSession::handle_out_of_order_packets(ScopedLock<Spinlock>& lock, u32 remote_sequence, size_t data_size)
{
	if (m_state != State::ESTABLISHED) {
		return true;
	}

	while (!is_in_order_packet(remote_sequence) && is_in_window_packet(remote_sequence, data_size)) {
		// FIXME: wait for some more out of order packets before sending ack (for retransmition)
		send_ack();

		m_data_waitqueue.wait(lock);
	}
	return is_in_window_packet(remote_sequence, data_size);
}

Result<void> TCPSession::send_syn(ScopedLock<Spinlock>& lock)
{
	return send_and_wait_ack(lock, BufferView{}, FLAGS::SYN);
}

Result<void> TCPSession::send_ack()
{
	return send_packet(BufferView{}, FLAGS::ACK);
}

Result<void> TCPSession::send_fin(ScopedLock<Spinlock>& lock)
{
	return send_and_wait_ack(lock, BufferView{}, FLAGS::FIN | FLAGS::ACK);
}

Result<void> TCPSession::send_payload(ScopedLock<Spinlock>& lock, const BufferView& payload)
{
	return send_and_wait_ack(lock, payload, FLAGS::ACK | FLAGS::PSH);
}

Result<void> TCPSession::wait_for_syn(ScopedLock<Spinlock>& lock)
{
	lock.release();
	m_syn_waitqueue.acquire();
	lock.acquire();

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
	m_syn_waitqueue.release();
	m_ack_waitqueue.wake_up_all();
	m_data_waitqueue.wake_up_all();
	m_data_push_waitqueue.wake_up_all();

	m_ports->clear(m_local_port);
}

Result<void> TCPSession::send_and_wait_ack(ScopedLock<Spinlock>& lock, const BufferView& data, u8 flags)
{
	u32 sent_sequence = m_local_sequence;
	size_t retransmitions = 0;

	auto ack_checker = [this, sent_sequence]() { return m_last_ack <= sent_sequence && m_state != State::CLOSED; };

	WaitQueue::State queue_state = WaitQueue::State::Blocked;

	do {
		if (auto error = send_packet(data, flags))
			return error;

		retransmitions++;
		queue_state = m_ack_waitqueue.wait_on_event(ack_checker, lock, 1000);
	} while ((retransmitions != 5) && (queue_state == WaitQueue::State::Timeout));

	if (queue_state == WaitQueue::State::Timeout) {
		return ResultError{ERROR_CONNECTION_TIMEOUT};
	}

	if (m_state == State::CLOSED) {
		return ResultError{ERROR_CONNECTION_CLOSED};
	}

	if ((flags & FLAGS::FIN) || (flags & FLAGS::SYN)) {
		m_local_sequence++;
	} else {
		m_local_sequence += data.size();
	}
	return {};
}

Result<void> TCPSession::send_packet(const BufferView& data, u8 flags)
{
	TCPHeader tcp_header;
	tcp_header.src_port = network_word16(m_local_port);
	tcp_header.dest_port = network_word16(m_remote_port);
	tcp_header.seq = network_word32(m_local_sequence);
	tcp_header.ack = network_word32(m_remote_sequence);
	tcp_header.data_offset = to_data_offset(sizeof(TCPHeader));
	tcp_header.flags = flags;
	tcp_header.window_size = network_word16(m_local_window_size);
	tcp_header.checksum = 0;
	tcp_header.urgent_pointer = 0;

	if (data.size() > 0) {
		Buffer segment{data.size() + sizeof(TCPHeader)};
		segment.fill_by(tcp_header, 0);
		segment.fill_from(data.ptr(), sizeof(TCPHeader), data.size());

		segment.convert_to<TCPHeader>().checksum = tcp_checksum(segment);

		m_network->ipv4_provider().send(m_remote_ip, IPv4Protocols::TCP, segment);
	} else {
		tcp_header.checksum = tcp_checksum(BufferView{&tcp_header, sizeof(TCPHeader)});
		m_network->ipv4_provider().send(m_remote_ip, IPv4Protocols::TCP, BufferView{&tcp_header, sizeof(TCPHeader)});
	}

	return {};
}

bool TCPSession::is_packet_ok(const BufferView& data)
{
	if (tcp_checksum(data) != 0) {
		return false;
	}

	return true;
}

bool TCPSession::is_in_order_packet(u32 remote_sequence)
{
	return m_remote_sequence == remote_sequence;
}
bool TCPSession::is_in_window_packet(u32 new_remote_sequence, size_t data_size)
{
	return (data_size + new_remote_sequence - m_remote_sequence) <= m_local_window_size;
}

bool TCPSession::is_buffer_available(size_t requested_size)
{
	return m_local_window_size >= requested_size;
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