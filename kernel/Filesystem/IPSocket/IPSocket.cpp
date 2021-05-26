#include "IPSocket.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include <List.h>
#include <UniquePointer.h>

List<UniquePointer<FSNode>> sockets;

Result<FSNode> create_socket(SocketAddress addr, SocketProtocol protocol, size_t flags)
{
	if (protocol == SocketProtocol::ProtocolTCP) {
		return *sockets.emplace_back(UniquePointer<FSNode>::make_unique(TCPSocket{addr}));
	} else if (protocol == SocketProtocol::ProtocolUDP) {
		return *sockets.emplace_back(UniquePointer<FSNode>::make_unique(UDPSocket{addr}));
	} else {
		ASSERT_NOT_REACHABLE();
		return ResultError{ERROR_INVALID_PARAMETER};
	}
}

Result<FSNode> close_socket(FSNode& node) {}