#pragma once

#include "Tasking/ScopedLock.h"
#include "Tasking/Semaphore.h"
#include <BufferView.h>
#include <IPv4Address.h>
#include <Result.h>
#include <Vector.h>

class Network;
class TCP;

class TCPSession
{
  public:
	NON_COPYABLE(TCPSession)
	enum class Type
	{
		Server,
		Client
	};
	TCPSession(Network& network, Type type);
	TCPSession(TCPSession&&) = default;
	TCPSession& operator=(TCPSession&&) = default;
	Result<void> accept(u16 port);
	Result<void> connect(IPv4Address ip, u16 port);
	void close();
	Result<void> send(const BufferView& data);
	Result<void> receive(Buffer& data);

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
		Listen,
		SYN_Sent,
		SYN_Received,
		Established,
		FIN_Wait1,
		FIN_Wait2,
		Close_Wait,
		Last_Ack,
		Time_Wait,
		Closed
	};
	void handle(IPv4Address src_ip, const BufferView& data);
	void handle_syn(IPv4Address src_ip, const BufferView& data);
	void handle_ack();
	void handle_rst();
	void handle_fin();
	void handle_psh();
	void handle_data(const BufferView& data);

	void send_ack();
	void send_syn();
	void send_ack_syn();

	void wait_for_ack(ScopedLock<Spinlock>& lock);
	void wait_for_syn(ScopedLock<Spinlock>& lock);
	void wait_for_packet(ScopedLock<Spinlock>& lock);

	void end_connection();

	void send_packet(const BufferView& data, u8 flags);
	void send_control_packet(u8 flags);
	bool is_packet_ok(const BufferView& data);
	bool is_buffer_available();
	u16 tcp_checksum(const BufferView& data);
	bool is_packet_for_me(IPv4Address ip, const BufferView& data);
	constexpr u8 to_data_offset(size_t value) { return (number_of_words<u32>(value) & 0xF) << 4; }
	constexpr size_t from_data_offset(u8 value) { return (value >> 4) * sizeof(u32); }
	static const u16 WINDOW_SIZE = 1000;

	UniquePointer<Spinlock> m_lock;
	Network* m_network;
	Type m_type;
	Buffer* m_buffer{0};
	State m_state{State::Closed};
	Semaphore m_syn_semaphore{0};
	Semaphore m_ack_semaphore{0};
	Semaphore m_data_semaphore{0};
	IPv4Address m_remote_ip{};
	size_t m_remote_sequence{0};
	size_t m_local_sequence{0};
	size_t m_local_port{0};
	size_t m_remote_port{0};

	friend TCP;
};

class TCP
{
  public:
	TCP(Network&);
	TCPSession& accept(u16 port);
	TCPSession& connect(IPv4Address ip, u16 port);
	void close(TCPSession&);
	void handle(IPv4Address src_ip, const BufferView& data);

	Network& m_network;
	Vector<TCPSession> m_connection_sessions;
};
