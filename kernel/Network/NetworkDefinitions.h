#pragma once

#include <IPv4Address.h>
#include <Types.h>

struct SocketAddress {
	IPv4Address ip;
	u16 port;
};