#pragma once
#include <Types.h>

class IPv4
{
  public:
	void send_ip_packet();

  private:
	void handle_ip_packet();

	struct IPv4Header {
		u8 version : 4;
		u8 header_length : 4;
		u8 dscp : 6;
		u8 ecn : 2;
		u16 length;
		u16 id;
		u16 flags : 3;
		u16 fragment_offset : 14;
		u8 time_to_live;
		u8 protocol;
		u16 header_checksum;
		u8 src_ip[4];
		u8 dst_ip[4];
		u8 data[1];
	} __attribute__((packed));
};
