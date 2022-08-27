#pragma once

#include <ipv4_address.h>
#include <types.h>

struct SocketAddress {
	IPv4Address ip;
	u16 port;
};

enum class SocketProtocol
{
	ProtocolUDP,
	ProtocolTCP
};