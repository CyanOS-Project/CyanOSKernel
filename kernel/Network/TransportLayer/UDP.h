#include <BufferView.h>
#include <IPv4Address.h>
#include <Types.h>

class UDP
{

  public:
	void send(const IPv4Address& dest_ip, u16 dest_port, const BufferView& data);
	void handle(const IPv4Address& src_ip, const BufferView& data);

  private:
	struct UDPHeader {
		u16 source_port;
		u16 destination_port;
		u16 total_length;
		u16 checksum;
	} __attribute__((packed));

	u16 calculate_checksum(const BufferView& data);
};
