#include "ICMP.h"
#include "Network/Network.h"
#include <Buffer.h>
#include <Endianess.h>

ICMP::ICMP(Network& network) : m_network{network}, m_lock{}, m_connection_list{} {}

void ICMP::send_echo_request(const IPv4Address& address)
{
	ScopedLock local_lock{m_lock};

	Buffer icmp_raw_packet{sizeof(EchoPacket)};
	auto& icmp_header = icmp_raw_packet.convert_to<EchoPacket>();

	icmp_header.type = static_cast<u8>(ICMPType::EchoRequest);
	icmp_header.code = 0;
	icmp_header.checksum = 0;
	icmp_header.echo_id = 'a';
	icmp_header.echo_seq = 'z';

	icmp_header.checksum = calculate_checksum(icmp_raw_packet);

	m_network.ipv4_provider().send_ip_packet(address, IPv4Protocols::ICMP, icmp_raw_packet);
	auto& connection = m_connection_list.emplace_back(address);

	connection.wait_queue.wait(local_lock);

	if (connection.state == State::SuccessfulReply) {
		info() << "Pinging " << address << " returned successful reply.";
	} else if (connection.state == State::BadReply) {
		info() << "Pinging " << address << " returned unsuccessful reply.";
	}
	m_connection_list.remove_if([&connection](const Connection& i) { return i.dest == connection.dest; });
}
void ICMP::handle_icmp_reply(const IPv4Address& source_ip, const BufferView& data)
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

	warn() << "ICMP packet received.";
}

void ICMP::handle_echo_reply(const IPv4Address& source_ip, const BufferView& data)
{
	ScopedLock local_lock{m_lock};

	auto& icmp_header = data.const_convert_to<EchoPacket>();

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

	if (calculate_checksum(data) != 0) {
		return false;
	}

	return true;
}

u16 ICMP::calculate_checksum(const BufferView& data)
{
	auto* u16_array = reinterpret_cast<const u16*>(data.ptr());
	size_t u16_array_size = number_of_words<u16>(data.size());

	u32 sum = 0;
	for (size_t i = 0; i < u16_array_size; i++) {
		sum += to_big_endian<u16>(u16_array[i]);
	}

	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + ((sum & 0xFFFF0000) >> 16);
	}

	return to_big_endian<u16>(~u16(sum));
}