#pragma once

#include <IPv4Address.h>
#include <Types.h>

struct SocketAddress {
	IPv4Address ip;
	u16 port;
};

enum class SocketProtocol
{
	ProtocolUDP,
	ProtocolTCP
};