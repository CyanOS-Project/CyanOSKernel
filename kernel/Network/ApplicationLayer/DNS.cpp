#include "DNS.h"
#include <NetworkAlgorithms.h>
#include <String.h>

DNS::DNS(UDP& udp, IPv4Address dns_server) : m_udp{udp}, m_dns_server{dns_server} {}

Result<IPv4Address> DNS::reslove(StringView domain_name)
{
	if (auto error = send_dns_query(domain_name))
		return ResultError{error.error()};

	if (auto error = get_dns_response())
		return ResultError{error.error()};
}

Result<void> DNS::send_dns_query(StringView domain_name)
{
	Buffer dns_notation = convert_to_dns_notation(domain_name);
	Buffer dns_request{sizeof(DNSHeader) + dns_notation.size() + 4};

	auto& dns_request_header = dns_request.convert_to<DNSHeader>();
	dns_request_header.id = 1;
	dns_request_header.flags =
	    network_word16(DNS_MESSAGE_FLAG_QUERY | DNS_MESSAGE_FLAG_STANDARD_QUERY | DNS_MESSAGE_FLAG_RECURSIVE_DESIRED);
	dns_request_header.questions_count = network_word16(1);
	dns_request_header.answers_count = 0;
	dns_request_header.name_server_count = 0;
	dns_request_header.additional_record_count = 0;

	dns_request.fill_from(dns_notation.ptr(), sizeof(DNSHeader), dns_notation.size());

	size_t header_offset = sizeof(DNSHeader) + dns_notation.size();
	dns_request[header_offset + 0] = 0x00;
	dns_request[header_offset + 1] = 0x01; // Type A.
	dns_request[header_offset + 2] = 0x00;
	dns_request[header_offset + 3] = 0x01; // Class IN.

	return m_udp.send(m_dns_server, 53, 53, dns_request);
}

Result<IPv4Address> DNS::get_dns_response()
{
	Buffer dns_response{1024};

	m_udp.receive(53, dns_response)
}

Result<IPv4Address> DNS::select_dns_response() {}

Buffer DNS::convert_to_dns_notation(StringView domain_name)
{
	Buffer dns_notation{domain_name.length() + 2};
	dns_notation.fill_from(domain_name.data(), 1, domain_name.length());

	// www.google.com
	// /3www/6google/3com

	size_t curr_dot = domain_name.find('.');
	dns_notation[0] = curr_dot;

	while (curr_dot != StringView::END) {
		size_t next_dot = domain_name.find('.', curr_dot + 1);
		if (next_dot != StringView::END) {
			dns_notation[curr_dot + 1] = next_dot - curr_dot - 1;
		} else {
			dns_notation[curr_dot + 1] = domain_name.length() - curr_dot - 1;
		}

		curr_dot = next_dot;
	}

	dns_notation[domain_name.length() + 1] = 0;

	return dns_notation;
}
