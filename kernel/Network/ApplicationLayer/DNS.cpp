#include "DNS.h"
#include <ErrorCodes.h>

DNS::DNS(UDP& udp, ICMP& icmp, IPv4Address dns_server) : m_udp{udp}, m_icmp{icmp}, m_dns_server{dns_server} {}

Result<IPv4Address> DNS::reslove(StringView domain_name)
{
	if (auto result = send_dns_query(domain_name))
		return ResultError{result.error()};

	return get_dns_response();
}

Result<void> DNS::send_dns_query(StringView domain_name)
{
	Buffer dns_notation = convert_to_dns_notation(domain_name);
	Buffer dns_request{sizeof(DNSHeader) + dns_notation.size() + 4};

	auto& dns_request_header = dns_request.convert_to<DNSHeader>();
	dns_request_header.set_id(1);
	dns_request_header.set_flags(DNS_MESSAGE_FLAG_QUERY | DNS_MESSAGE_FLAG_RECURSIVE_DESIRED);
	dns_request_header.set_questions_count(1);
	dns_request_header.set_answers_count(0);
	dns_request_header.set_name_server_count(0);
	dns_request_header.set_additional_record_count(0);

	dns_request.fill_from(dns_notation.ptr(), sizeof(DNSHeader), dns_notation.size());

	size_t header_offset = sizeof(DNSHeader) + dns_notation.size();
	dns_request[header_offset + 0] = 0x00;
	dns_request[header_offset + 1] = DNS_MESSAGE_TYPE_HOST_IP;
	dns_request[header_offset + 2] = 0x00;
	dns_request[header_offset + 3] = DNS_MESSAGE_CLASS_IN;

	return ResultError{m_udp.send(SocketAddress{m_dns_server, 53}, 53, dns_request)};
}

Result<IPv4Address> DNS::get_dns_response()
{
	Buffer dns_response{1024};

	if (auto result = m_udp.receive(53, dns_response))
		return ResultError{result.error()};

	auto& dns_header = dns_response.const_convert_to<DNSHeader>();

	if (dns_header.id() != 1)
		return get_dns_response();

	if (!(dns_header.flags() & DNS_MESSAGE_FLAG_RESPONSE))
		return get_dns_response();

	if (dns_header.answers_count() == 0)
		return get_dns_response();

	size_t buffer_offset = sizeof(DNSHeader);
	for (size_t i = 0; i < dns_header.questions_count(); i++) {
		buffer_offset += strlen(reinterpret_cast<const char*>(dns_response.ptr()) + buffer_offset) + 1 + 4;
	}

	BufferView answers_buffer{dns_response.ptr() + buffer_offset, dns_response.size() - buffer_offset};
	return select_dns_response(answers_buffer, dns_header.answers_count());
}

Result<IPv4Address> DNS::select_dns_response(const BufferView& answers_buffer, size_t number_of_answer)
{

	size_t buffer_offset = 0;

	for (size_t i = 0; i < number_of_answer; i++) {
		BufferView dns_answer_buffer{answers_buffer.ptr() + buffer_offset, sizeof(DNSAnswer)};

		auto& dns_answer = dns_answer_buffer.const_convert_to<DNSAnswer>();
		if (dns_answer.response_type() == DNS_MESSAGE_TYPE_HOST_IP &&
		    dns_answer.response_class() == DNS_MESSAGE_CLASS_IN) {

			if (test_ip(dns_answer.ip()))
				return dns_answer.ip();
		}
	}

	return ResultError{ERROR_DNS_UNRESOLVABLE};
}

bool DNS::test_ip(IPv4Address ip)
{
	// FIXME: test the ip first.
	return true;
	// return m_icmp.send_echo_request(ip);
}

Buffer DNS::convert_to_dns_notation(StringView domain_name)
{
	Buffer dns_notation{domain_name.length() + 2};
	dns_notation.fill_from(domain_name.data(), 1, domain_name.length());

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
