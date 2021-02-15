#pragma once
#include "NetworkAdapter.h"
#include <MACAddress.h>
#include <Types.h>

class EthernetNetworkAdapter : public NetworkAdapter
{
  public:
	EthernetNetworkAdapter() = default;
	void send_frame(ProtocolType type, const MACAddress& destination, const BufferView& data) override;

  protected:
	virtual void send_ethernet_frame(const BufferView& data) = 0;
	void handle_received_ethernet_frame(const BufferView& data);

  private:
	struct EthernetFrame {
		uint8_t dst_mac_addr[6];
		uint8_t src_mac_addr[6];
		u16 type;
		uint8_t data[1];
	} __attribute__((packed));
	static constexpr u16 ETHERNET_HEADER_SIZE = offsetof(EthernetFrame, data);
};
