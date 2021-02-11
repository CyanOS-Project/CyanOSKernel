#pragma once
#include "Network/Network.h"
#include <BufferView.h>
#include <IPv4Address.h>
#include <Types.h>

class ICMP
{

  public:
	ICMP(Network&);

	void send_echo_request(const IPv4Address& address);
	void handle_icmp_reply();

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

	u16 calculate_checksum(const BufferView& data);

	Network& m_network;
};
