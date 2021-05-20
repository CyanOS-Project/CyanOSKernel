#pragma once

#include "Network/NetworkDefinitions.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/Semaphore.h"
#include <Bitmap.h>
#include <BufferView.h>
#include <IPv4Address.h>
#include <Reference.h>
#include <Result.h>
#include <Vector.h>

class Network;
class TCP;

class TCPSession
{
  public:
	NON_COPYABLE(TCPSession)
	DEFAULT_MOVE(TCPSession)

	enum class Type
	{
		Server,
		Client
	};
	TCPSession(Network& network, Bitmap& port_bitmap, Type type); // FIXME: should be private.
	Result<void> accept(u16 port);
	Result<void> connect(IPv4Address ip, u16 port);
	void close();
	Result<size_t> send(const BufferView& data);
	Result<size_t> receive(Buffer& data);
	SocketAddress address() const;

  private:
	struct TCPHeader {
		u16 src_port;
		u16 dest_port;
		u32 seq;
		u32 ack;
		uint8_t data_offset;
		uint8_t flags;
		u16 window_size;
		u16 checksum;
		u16 urgent_pointer;
	} __attribute__((packed));

	struct PseudoIPHeader {
		u8 src_ip[4];
		u8 dest_ip[4];
		u8 zeros;
		u8 protocol;
		u16 tcp_length;
	} __attribute__((packed));

	enum FLAGS : u8
	{
		FIN = Bit(0),
		SYN = Bit(1),
		RST = Bit(2),
		PSH = Bit(3),
		ACK = Bit(4),
		URG = Bit(5),
		ECE = Bit(6),
		CWR = Bit(7),
	};

	enum class State
	{
		LISTEN,
		SYN_SENT,
		SYN_RECEIVED,
		ESTABLISHED,
		FIN_WAIT1,
		FIN_WAIT2,
		CLOSE_WAIT,
		LAST_ACK,
		TIME_WAIT,
		CLOSED
	};
	void handle(IPv4Address src_ip, const BufferView& data);
	void handle_syn(IPv4Address src_ip, const BufferView& data);
	void handle_ack(u32 ack_number);
	void handle_rst();
	void handle_fin(ScopedLock<Spinlock>&);
	void handle_psh();
	void handle_data(const BufferView& data, size_t payload_offset, size_t payload_size);
	bool handle_out_of_order_packets(ScopedLock<Spinlock>&, u32 remote_sequence, size_t data_size);

	Result<void> send_ack();
	Result<void> send_syn(ScopedLock<Spinlock>&);
	Result<void> send_fin(ScopedLock<Spinlock>&);
	Result<void> send_payload(ScopedLock<Spinlock>&, const BufferView&);

	Result<void> wait_for_syn(ScopedLock<Spinlock>&);
	Result<void> wait_for_packet(ScopedLock<Spinlock>&);

	void end_connection();

	Result<void> send_and_wait_ack(ScopedLock<Spinlock>& lock, const BufferView& data, u8 flags);
	Result<void> send_packet(const BufferView& data, u8 flags);

	bool is_packet_ok(const BufferView& data);
	bool is_in_order_packet(u32 remote_sequence);
	bool is_in_window_packet(u32 remote_sequence, size_t data_size);
	bool is_buffer_available(size_t requested_size);
	bool is_packet_for_me(IPv4Address ip, const BufferView& data);
	u16 tcp_checksum(const BufferView& data);
	constexpr u8 to_data_offset(size_t value) { return (number_of_words<u32>(value) & 0xF) << 4; }
	constexpr size_t from_data_offset(u8 value) { return (value >> 4) * sizeof(u32); }

	static constexpr size_t MAX_WINDOW_SIZE = 8000;

	UniquePointer<Spinlock> m_lock;
	Reference<Network> m_network;
	Reference<Bitmap> m_ports;
	Type m_type;
	State m_state{State::CLOSED};
	Semaphore m_syn_waitqueue{0};
	WaitQueue m_ack_waitqueue{};
	WaitQueue m_data_push_waitqueue{};
	WaitQueue m_data_waitqueue{};
	IPv4Address m_remote_ip{};
	Buffer m_buffer{MAX_WINDOW_SIZE};
	size_t m_buffer_start_pointer{0};
	size_t m_buffer_written_pointer{0};
	u32 m_remote_sequence{0};
	u32 m_local_sequence{0};
	u32 m_last_ack{0};
	u16 m_local_port{0};
	u16 m_remote_port{0};
	u16 m_local_window_size{MAX_WINDOW_SIZE};
	u16 m_remote_window_size{1};

	friend TCP;
};

class TCP
{
  public:
	TCP(Network&);
	Result<TCPSession&> accept(u16 port);
	Result<TCPSession&> connect(IPv4Address ip, u16 port);
	void handle(IPv4Address src_ip, const BufferView& data);

	Network& m_network;
	Vector<TCPSession> m_connection_sessions;
	Bitmap m_ports{65535};
};
