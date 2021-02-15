#pragma once

#include "UDP.h"
#include <IPv4Address.h>
#include <Types.h>

class Network;
class DHCP
{

  public:
	struct DHCPInformation {
		IPv4Address device_ip;
		IPv4Address gateway_ip;
		IPv4Address subnet_mask;
	};

	DHCP(Network& network);
	DHCPInformation request_dhcp_information();

  private:
	struct DHCPHeader {
		u8 op;
		u8 hardware_type;
		u8 hardware_addr_len;
		u8 hops;
		u32 xid;
		u16 seconds;
		u16 flags;
		u8 client_ip[4];
		u8 your_ip[4];
		u8 server_ip[4];
		u8 gateway_ip[4];
		u8 client_hardware_addr[16];
		u8 server_name[64];
		u8 file[128];
		u32 magic_cookie;
		u8 options[60];
	} __attribute__((packed));

	enum OptionCodes
	{
		Pading = 0,
		SubnetMask = 1,
		RouterIPs = 3,
		DNSServers = 6,
		DomainName = 15,
		NetworkTimeProtocolServers = 42,
		RequestedIP = 50,
		MessageType = 53,
		ParameterRequestList = 55,
		ClientIdentifier = 61,
		End = 255
	};

	enum class DCHPMessageType
	{
		Discover = 1,
		Offer = 2,
		Request = 3,
		Decline = 4,
		ACK = 5,
		NACK = 6,
		Release = 7,
		Inform = 8
	};

	struct OptionHeader {
		u8 code;
		u8 size;
	} __attribute__((packed));

	struct OptionClientID : public OptionHeader {
		u8 hardware_type;
		u8 hardware_id[6];
	} __attribute__((packed));

	struct OptionSubnetMask : public OptionHeader {
		u32 subnet_mask;
	} __attribute__((packed));

	struct OptionRouterIPs : public OptionHeader {
		u32 router_ip[1];
	} __attribute__((packed));

	struct OptionDNSServers : public OptionHeader {
		u32 server_ip[1];
	} __attribute__((packed));

	struct OptionDomainName : public OptionHeader {
		u8 domain_name[1];
	} __attribute__((packed));

	struct OptionMessageType : public OptionHeader {
		u8 type;
	} __attribute__((packed));

	struct OptionParameterRequestList : public OptionHeader {
		u8 param[8];
	} __attribute__((packed));

	struct OptionRequestedIP : public OptionHeader {
		u8 address[4];
	} __attribute__((packed));

	struct OptionPadding {
		u8 code;
	} __attribute__((packed));

	struct OptionEnd {
		u8 code;
	} __attribute__((packed));

	enum class DHCP_OP
	{
		Request = 1,
		Reply = 2
	};

	enum class DHCPState
	{
		Idle,
		Discovery,
		Offer,
		Request,
		Acknowledge
	};

	void handle_dhcp(const BufferView& buffer);
	void handle_dhcp_offer(const BufferView&);
	void handle_dhcp_ack(const BufferView&);
	void send_dhcp_discovery();
	void send_dhcp_request(const IPv4Address& requested_ip);
	Buffer make_dhcp_segment(const IPv4Address& requested_ip, DCHPMessageType type);
	void parse_dhcp_offer(const BufferView& buffer);
	DCHPMessageType get_dhcp_type(const BufferView&);

	Network& m_network;
	DHCPState m_state = DHCPState::Idle;
	IPv4Address m_device_ip;
	IPv4Address m_gateway_ip;
	IPv4Address m_subnet_mask;
};
