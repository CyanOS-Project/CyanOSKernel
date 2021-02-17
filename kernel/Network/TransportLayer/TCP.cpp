#include "TCP.h"
#include "Devices/DebugPort/Logger.h"

TCP::TCP(Network& network) : m_network{network} {}

void TCP::send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data)
{
	UNUSED(dest_ip);
	UNUSED(dest_port);
	UNUSED(data);

	warn() << "Sending TCP ?, I don't support that yet :(";
}

void TCP::handle(const IPv4Address& src_ip, const BufferView& data)
{
	UNUSED(src_ip);
	UNUSED(data);

	warn() << "TCP packet received, No handler for that yet.";
}