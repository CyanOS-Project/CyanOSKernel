#include "dhcp.h"
#include "network/network.h"
#include <network_algorithms.h>

DHCP::DHCP(Network& network) : m_network{network} {}

DHCP::DHCPInformation DHCP::request_dhcp_information()
{
	send_dhcp_discovery();

	Buffer received_data{2000};
	while (m_state != DHCPState::Acknowledge) {
		m_network.udp_provider().receive(68, received_data);
		handle_dhcp(received_data);
	}

	warn() << "The DCHP answered our request:\n"
	       << "My IP:       " << m_device_ip << "\nGateway IP:  " << m_gateway_ip << "\nSubnet mask: " << m_subnet_mask;

	return DHCPInformation{m_device_ip, m_gateway_ip, m_subnet_mask};
}

void DHCP::handle_dhcp(const BufferView& buffer)
{
	switch (get_dhcp_type(buffer)) {
		case DCHPMessageType::Offer:
			handle_dhcp_offer(buffer);
			break;

		case DCHPMessageType::ACK:
			handle_dhcp_ack(buffer);
			break;

		default:
			err() << "Something wrong with DCHP!";
	}
}

void DHCP::handle_dhcp_offer(const BufferView& buffer)
{
	parse_dhcp_offer(buffer);
	send_dhcp_request(m_device_ip);
	m_state = DHCPState::Offer;
}

void DHCP::handle_dhcp_ack(const BufferView& buffer)
{
	UNUSED(buffer);
	m_state = DHCPState::Acknowledge;
}

void DHCP::send_dhcp_discovery()
{
	Buffer dhcp_raw_segment = make_dhcp_segment(IPv4Address::Zero, DCHPMessageType::Discover);
	m_network.udp_provider().send(SocketAddress{IPv4Address::Broadcast, 67}, 68, dhcp_raw_segment);
	m_state = DHCPState::Discovery;
}

void DHCP::send_dhcp_request(IPv4Address requested_ip)
{
	Buffer dhcp_raw_segment = make_dhcp_segment(requested_ip, DCHPMessageType::Request);
	m_network.udp_provider().send(SocketAddress{IPv4Address::Broadcast, 67}, 68, dhcp_raw_segment);
	m_state = DHCPState::Request;
}

Buffer DHCP::make_dhcp_segment(IPv4Address requested_ip, DCHPMessageType type)
{
	Buffer dhcp_raw_segment{sizeof(DHCPHeader)};
	auto& dhcp_segment = dhcp_raw_segment.convert_to<DHCPHeader>();

	dhcp_segment.op = static_cast<u8>(DHCP_OP::Request);
	dhcp_segment.hardware_type = 1;
	dhcp_segment.hardware_addr_len = 6;
	dhcp_segment.hops = 0;
	dhcp_segment.xid = 0xDEADC0DE;
	dhcp_segment.seconds = 0;
	dhcp_segment.flags = network_word16(0x8000);

	m_network.device_mac().copy(dhcp_segment.client_hardware_addr);

	dhcp_segment.magic_cookie = network_word32(0x63825363);

	size_t options_offset = offsetof(DHCPHeader, options); // TODO: implement << operator in `Buffer`

	OptionMessageType message_type{};
	message_type.code = OptionCodes::MessageType;
	message_type.size = 1;
	message_type.type = static_cast<u8>(type);
	dhcp_raw_segment.fill_by(message_type, options_offset);
	options_offset += sizeof(message_type);

	OptionClientID client_id{};
	client_id.code = OptionCodes::ClientIdentifier;
	client_id.size = 7;
	client_id.hardware_type = 1;
	m_network.device_mac().copy(client_id.hardware_id);
	dhcp_raw_segment.fill_by(client_id, options_offset);
	options_offset += sizeof(client_id);

	OptionRequestedIP client_ip{};
	client_ip.code = OptionCodes::RequestedIP;
	client_ip.size = 4;
	requested_ip.copy(client_ip.address);
	dhcp_raw_segment.fill_by(client_ip, options_offset);
	options_offset += sizeof(client_ip);

	OptionParameterRequestList param_list{};
	param_list.code = OptionCodes::ParameterRequestList;
	param_list.size = 4;
	param_list.param[0] = OptionCodes::SubnetMask;
	param_list.param[1] = OptionCodes::RouterIPs;
	param_list.param[2] = OptionCodes::DNSServers;
	param_list.param[3] = OptionCodes::NetworkTimeProtocolServers;
	dhcp_raw_segment.fill_by(param_list, options_offset);
	options_offset += sizeof(param_list);

	OptionEnd message_end{};
	message_end.code = OptionCodes::End;
	dhcp_raw_segment.fill_by(message_end, options_offset);
	options_offset += sizeof(message_end);

	return dhcp_raw_segment;
}

DHCP::DCHPMessageType DHCP::get_dhcp_type(const BufferView& buffer)
{
	auto& dhcp_segment = buffer.const_convert_to<DHCPHeader>();

	const u8* options_offset = dhcp_segment.options;
	const u8* options_end = buffer.ptr() + sizeof(DHCPHeader);

	auto* header = reinterpret_cast<const OptionHeader*>(options_offset);

	while (options_offset < options_end && header->code != OptionCodes::End) {
		if (header->code == OptionCodes::MessageType) {
			return static_cast<DCHPMessageType>(reinterpret_cast<const OptionMessageType*>(header)->type);
		} else if (header->code == OptionCodes::Pading) {
			options_offset++;
		} else {
			options_offset += header->size;
		}
		header = reinterpret_cast<const OptionHeader*>(options_offset);
	}
	return DCHPMessageType::NACK;
}

void DHCP::parse_dhcp_offer(const BufferView& buffer)
{
	auto& dhcp_segment = buffer.const_convert_to<DHCPHeader>();
	m_device_ip = IPv4Address{dhcp_segment.your_ip};

	const u8* options_offset = dhcp_segment.options;
	const u8* options_end = buffer.ptr() + sizeof(DHCPHeader);

	auto* header = reinterpret_cast<const OptionHeader*>(options_offset);

	while (options_offset < options_end && header->code != OptionCodes::End) {
		if (header->code == OptionCodes::RouterIPs) {

			auto* option = reinterpret_cast<const OptionRouterIPs*>(header);
			m_gateway_ip = IPv4Address(option->router_ip[0]);

		} else if (header->code == OptionCodes::SubnetMask) {

			auto* option = reinterpret_cast<const OptionSubnetMask*>(header);
			m_subnet_mask = IPv4Address(option->subnet_mask);
		}

		if (header->code == OptionCodes::Pading) {
			options_offset++;
		} else {
			options_offset += header->size + sizeof(OptionHeader);
		}
		header = reinterpret_cast<const OptionHeader*>(options_offset);
	}
}