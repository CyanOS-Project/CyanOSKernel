#pragma once

#include "Tasking/Semaphore.h"
#include <BufferView.h>
#include <IPv4Address.h>
#include <Vector.h>

class Network;
class TCP;

class TCPSession
{
  public:
	NON_COPYABLE(TCPSession)
	TCPSession(Network& network);
	TCPSession(TCPSession&& network) = default;
	TCPSession& operator=(TCPSession&& network) = default;
	void accept(u16 port);
	void connect(IPv4Address ip, u16 port);
	void close();
	void send(const BufferView& data);
	void receive(BufferView& data);

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

	enum class ConnectionState
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
	void send_syn();
	void wait_for_syn();
	void send_ack();
	void wait_for_ack();
	void handle_fin();
	void send_packet(const BufferView& data, u8 flags);
	void send_control_packet(u8 flags);
	Buffer wait_for_packet();
	bool is_packet_ok(const BufferView& data);
	u16 tcp_checksum(const BufferView& data);
	bool is_packet_for_me(const IPv4Address& ip, const BufferView& data);
	constexpr u8 data_offset(size_t value) { return (number_of_words<u32>(value) & 0xF) << 4; }

	Network* m_network;
	Semaphore m_syn_semaphore;
	Semaphore m_ack_semaphore;
	Semaphore m_data_semaphore;
	size_t m_dest_sequence;
	size_t m_src_sequence;
	IPv4Address m_dest_ip;
	size_t m_dest_port;
	size_t m_src_port;

	friend TCP;
};

class TCP
{
  public:
	TCP(Network&);
	TCPSession& accept(u16 port);
	TCPSession& connect(IPv4Address ip, u16 port);
	void close(TCPSession&);
	void handle(const IPv4Address& src_ip, const BufferView& data);

	Network& m_network;
	Vector<TCPSession> m_connection_sessions;
};
