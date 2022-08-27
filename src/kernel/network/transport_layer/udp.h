#pragma once

#include "network/network_definitions.h"
#include "tasking/wait_queue.h"
#include <bitmap.h>
#include <buffer_view.h>
#include <ipv4_address.h>
#include <result.h>
#include <types.h>
#include <vector.h>

class Network;
class UDP
{

  public:
	UDP(Network&);
	Result<size_t> send(SocketAddress dest, u16 src_port, BufferView data);
	Result<size_t> send(SocketAddress dest, BufferView data);
	Result<size_t> receive(u16 dest_port, BufferMutableView buffer);
	Result<size_t> receive(u16 dest_port, BufferMutableView buffer, SocketAddress& source_address);
	void handle(IPv4Address src_ip, BufferView data);

  private:
	struct UDPHeader {
		u16 source_port;
		u16 destination_port;
		u16 total_length;
		u16 checksum;
	} __attribute__((packed));

	struct DatagramInfo {
		IPv4Address src_ip;
		u16 src_port;
		size_t data_size;
	};

	struct Connection {
		u16 dest_port;
		u16 src_port;
		IPv4Address src_ip;
		Reference<BufferMutableView> buffer;
		size_t data_size;
		WaitQueue wait_queue;
		Connection(u16 t_port, BufferMutableView& t_buffer) : dest_port{t_port}, buffer{t_buffer}, wait_queue{} {}
	};

	Result<void> send_segment(SocketAddress dest, u16 src_port, BufferView data);
	Result<DatagramInfo> receive_segment(u16 dest_port, BufferMutableView buffer);

	Spinlock m_lock;
	Network& m_network;
	Vector<Connection> m_connections_list;
	Bitmap m_ports{65535};
};
