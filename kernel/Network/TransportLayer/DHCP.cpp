#include "DHCP.h"
#include <Endianess.h>

DHCP::DHCP(Network& network) : m_network{network} {}

void DHCP::get_my_ip()
{
	Buffer dhcp_raw_segment = make_dhcp_segment(IPv4Address::Zero, DCHPMessageType::Discover);
	m_network.udp_provider().send_with_special_port(IPv4Address::Broadcast, 67, 68, dhcp_raw_segment);
}

void DHCP::send_dhcp_discovery() {}

void DHCP::send_dhcp_request() {}

Buffer DHCP::make_dhcp_segment(const IPv4Address& requested_ip, DCHPMessageType type)
{
	Buffer dhcp_raw_segment{sizeof(DHCPHeader)};
	auto& dhcp_segment = dhcp_raw_segment.convert_to<DHCPHeader>();

	dhcp_segment.op = static_cast<u8>(DHCP_OP::Request);
	dhcp_segment.hardware_type = 1;
	dhcp_segment.hardware_addr_len = 6;
	dhcp_segment.hops = 0;
	dhcp_segment.xid = 0xDEADC0DE;
	dhcp_segment.seconds = 0;
	dhcp_segment.flags = to_big_endian<u16>(0x8000);

	// dhcp_segment.client_ip
	// dhcp_segment.your_ip
	// dhcp_segment.server_ip
	// dhcp_segment.gateway_ip
	// dhcp_segment.client_hardware_addr
	// IPv4Address{10, 0, 2, 2}.copy(dhcp_segment.gateway_ip);
	m_network.device_mac().copy(dhcp_segment.client_hardware_addr);

	dhcp_segment.magic_cookie = to_big_endian<u32>(0x63825363);

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

	/*char host_name[10];
	host_name[0] = 12;
	host_name[1] = 4;
	host_name[2] = 'h';
	host_name[3] = 'h';
	host_name[4] = 'h';
	host_name[5] = 'h';
	dhcp_raw_segment.fill_from(host_name, options_offset, 6);
	options_offset += 6;*/

	OptionParameterRequestList param_list{};
	param_list.code = OptionCodes::ParameterRequestList;
	param_list.size = 4;
	param_list.param[0] = OptionCodes::SubnetMask;
	param_list.param[1] = OptionCodes::RouterIPs;
	param_list.param[2] = OptionCodes::DNSServers;
	param_list.param[3] = OptionCodes::NetworkTimeProtocolServers;

	/*param_list.param[4] = 0x2C;
	param_list.param[5] = 0x2E;
	param_list.param[6] = 0x2F;
	param_list.param[7] = 0x39;*/
	dhcp_raw_segment.fill_by(param_list, options_offset);
	options_offset += sizeof(param_list);

	/*OptionSubnetMask submetmask{};
	submetmask.code = OptionCodes::SubnetMask;
	submetmask.size = 4;
	dhcp_raw_segment.fill_by(submetmask, options_offset);
	options_offset += sizeof(submetmask);*/

	OptionEnd message_end{};
	message_end.code = OptionCodes::End;
	dhcp_raw_segment.fill_by(message_end, options_offset);
	options_offset += sizeof(message_end);

	return dhcp_raw_segment;
}