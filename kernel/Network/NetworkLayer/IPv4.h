#pragma once
#include <Types.h>

class IPv4
{
  public:
	void send_ip_packet();

  private:
	void handle_ip_packet();

	struct IPv4Header {
		uint8_t version : 4;
		uint8_t header_length : 4;
		uint8_t dscp : 6;
		uint8_t ecn : 2;
		uint16_t length;
		uint16_t id;
		uint16_t flags : 3;
		uint16_t fragment_offset : 14;
		uint8_t time_to_live;
		uint8_t protocol;
		uint16_t header_checksum;
		uint8_t src_ip[4];
		uint8_t dst_ip[4];
		uint8_t data[1];
	} __attribute__((packed));
};
