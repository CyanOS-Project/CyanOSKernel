#include "Network/Network.h"
#include "UDP.h"
#include <Types.h>

class DHCP
{

  public:
	DHCP(Network& network);
	void get_my_ip();

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
	};

	struct OptionClientID : public OptionHeader {
		u8 hardware_type;
		u8 hardware_id[6];
	};

	struct OptionSubnetMask : public OptionHeader {
		u32 subnet_mask;
	};

	struct OptionRouterIPs : public OptionHeader {
		u32 router_ip[1];
	};

	struct OptionDNSServers : public OptionHeader {
		u32 server_ip[1];
	};

	struct OptionDomainName : public OptionHeader {
		u8 domain_name[1];
	};

	struct OptionMessageType : public OptionHeader {
		u8 type;
	};

	struct OptionParameterRequestList : public OptionHeader {
		u8 param[8];
	};

	struct OptionRequestedIP : public OptionHeader {
		u8 address[4];
	};

	struct OptionPadding {
		u8 code;
	};

	struct OptionEnd {
		u8 code;
	};

	void send_dhcp_discovery();
	void send_dhcp_request();

	enum class DHCP_OP
	{
		Request = 1,
		Reply = 2
	};
	Buffer make_dhcp_segment(const IPv4Address& requested_ip, DCHPMessageType type);

	Network& m_network;
};
