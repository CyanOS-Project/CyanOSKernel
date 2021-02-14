#pragma once

#include "Tasking/WaitQueue.h"
#include <Bitmap.h>
#include <BufferView.h>
#include <IPv4Address.h>
#include <Types.h>
#include <Vector.h>

class Network;
class UDP
{

  public:
	UDP(Network&);
	void send_with_special_port(const IPv4Address& dest_ip, u16 dest_port, u16 src_port, const BufferView& data);
	void send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data);
	void handle(const IPv4Address& src_ip, const BufferView& data);

  protected:
	void send_segment(const IPv4Address& dest_ip, u16 dest_port, u16 src_port, const BufferView& data);

  private:
	struct UDPHeader {
		u16 source_port;
		u16 destination_port;
		u16 total_length;
		u16 checksum;
	} __attribute__((packed));

	u16 calculate_checksum(const BufferView& data);

	struct Connection {
		u16 port;
		WaitQueue wait_queue;
		Connection(u16 t_port) : port{t_port}, wait_queue{} {}
	};

	Spinlock m_lock;
	Network& m_network;
	Bitmap<24575> m_available_ports_bitmap; // FIXME: get rid of 24576 limitation of bitmap.
	Vector<Connection> m_connections_list;
};
