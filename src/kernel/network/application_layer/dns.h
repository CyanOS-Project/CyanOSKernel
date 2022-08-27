
#include "network/network_layer/icmp.h"
#include "network/transport_layer/udp.h"
#include <ipv4_address.h>
#include <network_algorithms.h>
#include <string_view.h>

class DNS
{
  public:
	DNS(UDP& udp, ICMP& icmp, IPv4Address dns_server);
	Result<IPv4Address> reslove(StringView domain_name);

  private:
	Buffer convert_to_dns_notation(StringView domain_name);
	Result<void> send_dns_query(StringView domain_name);
	Result<IPv4Address> get_dns_response();
	Result<IPv4Address> select_dns_response(const BufferView& answers_buffer, size_t number_of_answer);
	bool test_ip(IPv4Address ip);

	struct DNSHeader {
	  private:
		u16 m_id;
		u16 m_flags;
		u16 m_questions_count;
		u16 m_answers_count;
		u16 m_name_server_count;
		u16 m_additional_record_count;

	  public:
		u16 id() const { return network_word16(m_id); }
		u16 flags() const { return network_word16(m_flags); }
		u16 questions_count() const { return network_word16(m_questions_count); }
		u16 answers_count() const { return network_word16(m_answers_count); }
		u16 name_server_count() const { return network_word16(m_name_server_count); }
		u16 additional_record_count() const { return network_word16(m_additional_record_count); }

		void set_id(u16 value) { m_id = network_word16(value); }
		void set_flags(u16 value) { m_flags = network_word16(value); }
		void set_questions_count(u16 value) { m_questions_count = network_word16(value); }
		void set_answers_count(u16 value) { m_answers_count = network_word16(value); }
		void set_name_server_count(u16 value) { m_name_server_count = network_word16(value); }
		void set_additional_record_count(u16 value) { m_additional_record_count = network_word16(value); }

	} __attribute__((packed));

	struct DNSAnswer {
	  private:
		u16 m_name;
		u16 m_response_type;
		u16 m_response_class;
		u32 m_time_to_live;
		u16 m_data_length;
		u8 m_ip[4];

	  public:
		u16 name() const { return network_word16(m_name); }
		u16 response_type() const { return network_word16(m_response_type); }
		u16 response_class() const { return network_word16(m_response_class); }
		u32 time_to_live() const { return network_word32(m_time_to_live); }
		u16 data_length() const { return network_word16(m_data_length); }
		IPv4Address ip() const { return IPv4Address{m_ip}; }
	} __attribute__((packed));

	static constexpr u16 DNS_MESSAGE_FLAG_RESPONSE = Bit(15);
	static constexpr u16 DNS_MESSAGE_FLAG_RECURSIVE_DESIRED = Bit(8);
	static constexpr u16 DNS_MESSAGE_FLAG_RECURSIVE_AVAILABLE = Bit(7);
	static constexpr u16 DNS_MESSAGE_FLAG_QUERY = 0;
	static constexpr u8 DNS_MESSAGE_TYPE_HOST_IP = 1;
	static constexpr u8 DNS_MESSAGE_CLASS_IN = 1;

	UDP& m_udp;
	ICMP& m_icmp;
	IPv4Address m_dns_server;
};
