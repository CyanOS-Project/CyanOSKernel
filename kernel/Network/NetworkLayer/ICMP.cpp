#include "ICMP.h"
#include "Network/Network.h"
#include <Buffer.h>
#include <Endianess.h>
#include <ErrorCodes.h>
#include <NetworkAlgorithms.h>

ICMP::ICMP(Network& network) : m_network{network}, m_lock{}, m_connection_list{} {}

Result<void> ICMP::send_echo_request(IPv4Address address)
{
	ScopedLock local_lock{m_lock};

	Buffer icmp_raw_packet{sizeof(EchoPacket)};
	auto& icmp_header = icmp_raw_packet.convert_to<EchoPacket>();

	icmp_header.type = static_cast<u8>(ICMPType::EchoRequest);
	icmp_header.code = 0;
	icmp_header.checksum = 0;
	icmp_header.echo_id = 'a';
	icmp_header.echo_seq = 'z';

	icmp_header.checksum = checksum(icmp_raw_packet);

	m_network.ipv4_provider().send(address, IPv4Protocols::ICMP, icmp_raw_packet);
	auto& connection = *m_connection_list.emplace_back(address);

	connection.wait_queue.wait(local_lock, 1000);

	auto state = connection.state;

	m_connection_list.remove_if([&connection](const Connection& i) { return i.dest == connection.dest; });

	if (state == State::SuccessfulReply) {
		info() << "Pinging " << address << " returned successful reply.";
		return {};
	} else if (state == State::BadReply) {
		info() << "Pinging " << address << " returned unsuccessful reply.";
		return ResultError{ERROR_ECHO_BAD_REPLY};
	} else if (state == State::Sent) {
		info() << "Pinging " << address << "  timeout.";
		return ResultError{ERROR_ECHO_TIMEOUT};
	}

	ASSERT_NOT_REACHABLE();
	return {};
}
void ICMP::handle_icmp_reply(IPv4Address source_ip, const BufferView& data)
{
	auto& icmp_header = data.const_convert_to<EchoPacket>();

	switch (static_cast<ICMPType>(icmp_header.type)) {
		case ICMPType::EchoReply:
			handle_echo_reply(source_ip, data);
			break;

		default:
			err() << "Unknown ICMP packet received!";
			break;
	}
}

void ICMP::handle_echo_reply(IPv4Address source_ip, const BufferView& data)
{
	ScopedLock local_lock{m_lock};

	auto connection = m_connection_list.find_if([source_ip](const Connection& i) { return i.dest == source_ip; });
	if (connection == m_connection_list.end()) {
		return;
	}

	if (is_icmp_reply_ok(data)) {
		connection->state = State::SuccessfulReply;
	} else {
		connection->state = State::BadReply;
	}
	connection->wait_queue.wake_up_all();
}

bool ICMP::is_icmp_reply_ok(const BufferView& data)
{
	auto& icmp_header = data.const_convert_to<EchoPacket>();
	if (icmp_header.echo_id != 'a' || icmp_header.echo_seq != 'z') {
		return false;
	}

	if (checksum(data) != 0) {
		return false;
	}

	return true;
}
