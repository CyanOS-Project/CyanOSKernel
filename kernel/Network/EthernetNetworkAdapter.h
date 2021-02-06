#pragma once
#include "NetworkAdapter.h"
#include <MACAddress.h>
#include <Types.h>

class EthernetNetworkAdapter : public NetworkAdapter
{
  public:
	EthernetNetworkAdapter() = default;
	void send_frame(ProtocolType type, const MACAddress& destination, const void* data, size_t len) override;

  protected:
	virtual void send_ethernet_frame(const void* data, size_t size) = 0;
	void handle_received_ethernet_frame(const void* data, size_t size);

  private:
	struct EthernetFrame {
		uint8_t dst_mac_addr[6];
		uint8_t src_mac_addr[6];
		uint16_t type;
		uint8_t data[1];
	} __attribute__((packed));
	static constexpr uint16_t ETHERNET_HEADER_SIZE = offsetof(EthernetFrame, data);
};
