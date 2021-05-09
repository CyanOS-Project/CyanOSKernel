#pragma once
#include "Tasking/SpinLock.h"
#include "Tasking/WaitQueue.h"
#include <BufferView.h>
#include <IPv4Address.h>
#include <Result.h>
#include <Types.h>
#include <Vector.h>

class Network;
class ICMP
{

  public:
	ICMP(Network& network);

	Result<void> send_echo_request(IPv4Address address);
	void handle_icmp_reply(IPv4Address source_ip, const BufferView& data);
	void handle_echo_reply(IPv4Address source_ip, const BufferView& data);
	bool is_icmp_reply_ok(const BufferView& data);

  private:
	struct ICMPHeader {
		u8 type;
		u8 code;
		u16 checksum;
	} __attribute__((packed));

	struct EchoPacket : public ICMPHeader {
		u16 echo_id;
		u16 echo_seq;
	} __attribute__((packed));

	enum class ICMPType
	{
		EchoReply = 0,
		EchoRequest = 8
	};
	enum class State
	{
		Sent,
		SuccessfulReply,
		BadReply
	};
	struct Connection {
		State state;
		IPv4Address dest;
		WaitQueue wait_queue;
		Connection(IPv4Address t_dest) : state{State::Sent}, dest{t_dest}, wait_queue{} {}
	};

	Network& m_network;
	Spinlock m_lock;
	Vector<Connection> m_connection_list;
};
