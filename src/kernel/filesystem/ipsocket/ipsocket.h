#pragma once

#include "filesystem/fsnode.h"
#include "network/network_definitions.h"

Result<FSNode> create_socket(SocketAddress addr, SocketProtocol protocol, size_t flags);
Result<void> close_socket(FSNode& node);