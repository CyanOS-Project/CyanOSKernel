#pragma once

#include "Tasking/WaitQueue.h"
#include <Bitmap.h>
#include <BufferView.h>
#include <IPv4Address.h>
#include <Result.h>
#include <Types.h>
#include <Vector.h>

class Network;
class UDP
{

  public:
	struct ConnectionInformation {
		u16 src_port;
		IPv4Address src_ip;
		size_t data_size;
	};

	UDP(Network&);
	Result<void> send(IPv4Address dest_ip, u16 dest_port, const BufferView& data);
	Result<void> send(IPv4Address dest_ip, u16 dest_port, u16 src_port, const BufferView& data);
	ConnectionInformation receive(u16 dest_port, Buffer& buffer);
	void handle(IPv4Address src_ip, const BufferView& data);

  private:
	struct UDPHeader {
		u16 source_port;
		u16 destination_port;
		u16 total_length;
		u16 checksum;
	} __attribute__((packed));

	struct Connection {
		u16 dest_port;
		u16 src_port;
		IPv4Address src_ip;
		Buffer* buffer;
		size_t data_size;
		WaitQueue wait_queue;
		Connection(u16 t_port, Buffer& t_buffer) : dest_port{t_port}, buffer{&t_buffer}, wait_queue{} {}
	};

	void send_segment(IPv4Address dest_ip, u16 dest_port, u16 src_port, const BufferView& data);

	Spinlock m_lock;
	Network& m_network;
	Vector<Connection> m_connections_list;
	Bitmap m_ports{65535};
};
