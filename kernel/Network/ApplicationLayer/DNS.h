
#include "Network/TransportLayer/UDP.h"
#include <IPv4Address.h>
#include <StringView.h>

class DNS
{
  public:
	DNS(UDP& udp, IPv4Address dns_server);
	Result<IPv4Address> reslove(StringView domain_name);

  private:
	Buffer convert_to_dns_notation(StringView domain_name);
	Result<void> send_dns_query(StringView domain_name);
	Result<IPv4Address> get_dns_response();
	Result<IPv4Address> select_dns_response();

	struct DNSHeader {
		u16 id;
		u16 flags;
		u16 questions_count;
		u16 answers_count;
		u16 name_server_count;
		u16 additional_record_count;
	} __attribute__((packed));

	static constexpr u8 DNS_MESSAGE_FLAG_STANDARD_QUERY = 0;
	static constexpr u8 DNS_MESSAGE_FLAG_RECURSIVE_DESIRED = Bit(24);
	static constexpr u8 DNS_MESSAGE_FLAG_RECURSIVE_AVAILABLE = Bit(23);
	static constexpr u8 DNS_MESSAGE_FLAG_RESPONSE = Bit(0);
	static constexpr u8 DNS_MESSAGE_FLAG_QUERY = 0;

	UDP& m_udp;
	IPv4Address m_dns_server;
};
