#pragma once

#include "Filesystem/FSNode.h"
#include "Network/NetworkDefinitions.h"

Result<FSNode> create_socket(SocketAddress addr, SocketProtocol protocol, size_t flags);
Result<void> close_socket(FSNode& node);